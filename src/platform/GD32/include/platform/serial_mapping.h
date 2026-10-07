/*
 * This file is part of Betaflight.
 *
 * Betaflight is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Betaflight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Betaflight. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "io/serial.h"

#if SERIAL_UART_FIRST_INDEX != 0
#error "GD32 serial mapping requires zero-based internal UART identifiers"
#endif

// Array elements are constant addresses usable by the shared port-name table.
static const char gd32SerialUartNames[][7] = {
    "UART1", "UART2", "UART3", "UART4",
    "UART5", "UART6", "UART7", "UART8",
    "UART9", "UART10", "UART11", "UART12",
    "UART13", "UART14", "UART15", "UART16",
};

#define SERIAL_UART_NAME(index) gd32SerialUartNames[index]

static inline int platformSerialPortIdentifierToExternal(serialPortIdentifier_e identifier)
{
    if (identifier >= SERIAL_PORT_UART_FIRST && identifier <= SERIAL_PORT_UART15) {
        return identifier + 1;
    }
    return identifier;
}

static inline serialPortIdentifier_e platformSerialPortIdentifierFromExternal(int identifier)
{
    // Do not retain the obsolete UART0 wire ID as an alias for external UART1.
    if (identifier == SERIAL_PORT_UART_FIRST) {
        return SERIAL_PORT_NONE;
    }
    if (identifier > SERIAL_PORT_UART_FIRST && identifier <= SERIAL_PORT_UART15 + 1) {
        return (serialPortIdentifier_e)(identifier - 1);
    }
    return (serialPortIdentifier_e)identifier;
}
