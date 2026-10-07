/*
 * This file is part of Cleanflight.
 *
 * Cleanflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Cleanflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Cleanflight.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include <stdbool.h>

#include <limits.h>

extern "C" {
    #include "platform.h"

    #include "drivers/serial.h"
    #include "drivers/serial_impl.h"
    #include "drivers/serial_softserial.h"
    #include "drivers/serial_uart.h"

    #include "io/serial.h"

    #include "pg/pg.h"
    #include "pg/pg_ids.h"
    #include "pg/rx.h"

    void serialInit(bool softserialEnabled);

    PG_REGISTER(rxConfig_t, rxConfig, PG_RX_CONFIG, 0);
    PG_REGISTER(serialPinConfig_t, serialPinConfig, PG_SERIAL_PIN_CONFIG, 0);
}

#include "unittest_macros.h"
#include "gtest/gtest.h"

TEST(IoSerialTest, TestFindPortConfig)
{
    // given
    serialInit(false);

    // when
    const serialPortConfig_t *portConfig = findSerialPortConfig(FUNCTION_MSP);

    // then
    EXPECT_EQ(NULL, portConfig);
}

TEST(IoSerialTest, ExternalUartNamesAndIdentifiersAreOneBased)
{
    const serialPortIdentifier_e first = SERIAL_PORT_UART_FIRST;
    EXPECT_EQ(51, serialPortIdentifierToExternal(first));
    EXPECT_STREQ("UART1", serialName(first, "unknown"));
    EXPECT_EQ(first, findSerialPortByName("UART1", NULL));
    EXPECT_EQ(SERIAL_PORT_NONE, findSerialPortByName("UART0", NULL));
    EXPECT_EQ(first, serialPortIdentifierFromExternal(51));

    const serialPortIdentifier_e second = (serialPortIdentifier_e)(first + 1);
    EXPECT_EQ(52, serialPortIdentifierToExternal(second));
    EXPECT_STREQ("UART2", serialName(second, "unknown"));
    EXPECT_EQ(second, findSerialPortByName("uart2", NULL));
    EXPECT_EQ(second, serialPortIdentifierFromExternal(52));
}

TEST(IoSerialTest, ExternalIdentifierRoundTripPreservesEveryConfiguredPort)
{
    for (unsigned i = 0; i < SERIAL_PORT_COUNT; i++) {
        const serialPortIdentifier_e internal = serialPortIdentifiers[i];
        const int external = serialPortIdentifierToExternal(internal);
        EXPECT_EQ(internal, serialPortIdentifierFromExternal(external));
        EXPECT_EQ(internal, findSerialPortByName(serialName(internal, "unknown"), NULL));
        EXPECT_GE(findSerialPortIndexByIdentifier(internal), 0);
    }
}

TEST(IoSerialTest, NonUartIdentifiersDoNotChange)
{
    for (int identifier : {-1, 20, 30, 31, 40, 70, 79, 99}) {
        EXPECT_EQ(identifier, serialPortIdentifierToExternal((serialPortIdentifier_e)identifier));
        EXPECT_EQ(identifier, serialPortIdentifierFromExternal(identifier));
    }
}

#if SERIAL_UART_FIRST_INDEX == 0
TEST(IoSerialTest, ZeroBasedHardwareAndResourceSlotsRemainUnchanged)
{
    EXPECT_EQ(50, SERIAL_PORT_UART0);
    EXPECT_EQ(0, serialResourceIndex(SERIAL_PORT_UART0));
    EXPECT_EQ(1, serialOwnerIndex(SERIAL_PORT_UART0));
    EXPECT_EQ(SERIAL_PORT_NONE, serialPortIdentifierFromExternal(50));
    EXPECT_EQ(SERIAL_PORT_UART15, serialPortIdentifierFromExternal(66));
    EXPECT_EQ(66, serialPortIdentifierToExternal(SERIAL_PORT_UART15));
}
#else
TEST(IoSerialTest, OneBasedHardwareIdentifiersRemainUnchanged)
{
    EXPECT_EQ(51, SERIAL_PORT_UART1);
    EXPECT_EQ(0, serialResourceIndex(SERIAL_PORT_UART1));
    EXPECT_EQ(1, serialOwnerIndex(SERIAL_PORT_UART1));
    EXPECT_EQ(SERIAL_PORT_UART15, serialPortIdentifierFromExternal(65));
    EXPECT_EQ(65, serialPortIdentifierToExternal(SERIAL_PORT_UART15));
}
#endif


struct ResetCalled {};
static const serialPort_t *hostPort = NULL;
static uint32_t fakeMillis = 0;
static int plusToSend = 0;

// STUBS
extern "C" {
    void delay(uint32_t) {}

    bool isSerialTransmitBufferEmpty(const serialPort_t *) { return true; }

    void systemResetToBootloader(void) {}

    bool telemetryCheckRxPortShared(const serialPortConfig_t *) { return false; }

    uint32_t serialRxBytesWaiting(const serialPort_t *p) { return p == hostPort ? plusToSend : 0; }
    uint8_t serialRead(serialPort_t *) { plusToSend--; return '+'; }
    void serialWrite(serialPort_t *, uint8_t) {}

    uint32_t millis(void) { return fakeMillis += 1000; }  // advance so the "+++" idle guard always passes
    void systemReset(void) { throw ResetCalled(); }

    serialPort_t *usbVcpOpen(void) { return NULL; }

    serialPort_t *uartOpen(serialPortIdentifier_e, serialReceiveCallbackPtr, void *, uint32_t, portMode_e, portOptions_e) {
      return NULL;
    }

    serialPort_t *softSerialOpen(serialPortIdentifier_e, serialReceiveCallbackPtr, void *, uint32_t, portMode_e, portOptions_e) {
      return NULL;
    }

    void serialSetCtrlLineStateCb(serialPort_t *, void (*)(void *, uint16_t ), void *) {}
    void serialSetCtrlLineState(serialPort_t *, uint16_t ) {}
    uint32_t serialTxBytesFree(const serialPort_t *) {return 1;}

    void serialSetBaudRateCb(serialPort_t *, void (*)(serialPort_t *context, uint32_t baud), serialPort_t *) {}

    void pinioSet(int, bool) {}
}

TEST(IoSerialTest, TestPassthroughEscape)
{
    // given
    serialPort_t left = {}, right = {};
    right.identifier = SERIAL_PORT_UART1;   // non-USB host -> "+++" escape enabled
    hostPort = &right;
    fakeMillis = 0;
    plusToSend = 3;
    // when "+++" arrives after an idle gap, then it must reboot out of passthrough
    EXPECT_THROW(serialPassthrough(&left, &right, NULL, NULL), ResetCalled);
}
