# ARROWGD460V5 (AROW)

Experimental GD32F460RG board profile adapted from the user-supplied `config.h` identifying `ARROWGD460V5` / `AROW`. This is a separate layout from HAKRCF460V2; do not flash one board's image onto the other.

## Build

Use the fork's required ARM GCC 13.3.1 toolchain:

```sh
make arm_sdk_install
make CONFIG_DIR=src/config-local CONFIG=ARROWGD460V5 EXTRA_FLAGS=-Werror
```

Artifact: `obj/betaflight_2026.12.0-alpha_GD32F460RG_ARROWGD460V5.hex`.

The local-board prerelease matrix builds HAKRCF460V2, ARROWGD460V5 and GD32H757V1STARTV1. Its outputs are draft prereleases, pending hardware validation and publication approval.

## Port numbering

The supplied config uses one-based UART/SPI/I2C names. This fork's GD32 hardware tables use zero-based names, so those names and peripheral references are translated without changing physical pins.

| Supplied board port | Firmware port | TX | RX |
|---|---|---|---|
| UART1 | UART0 / USART0 | PB6 | PB7 |
| UART2 | UART1 / USART1 | PA2 | PA3 |
| UART3 | UART2 / USART2 | PC10 | PC11 |
| UART4 | UART3 | None | PA1 |
| UART5 | UART4 | PC12 | PD2 |

UART3 is intentionally RX-only: PA0 is the LED strip pin. The supplied ESC sensor port UART4 becomes firmware UART3. No external inverter is defined; PC0 is voltage sensing, not inverter control. Inverted receiver protocols require the actual board's signal path to be checked.

## Sensors and buses

| Function | GD32 bus | Pins | Chip select / interrupt |
|---|---|---|---|
| Flash | SPI0 | PA5 SCK, PA6 MISO, PA7 MOSI | PC13 CS |
| MAX7456 | SPI1 | PB13 SCK, PB14 MISO, PC3 MOSI | PB12 CS |
| Gyro/accelerometer | SPI2 | PB3 SCK, PB4 MISO, PB5 MOSI | PA15 CS, PC4 EXTI |
| DPS310 / optional magnetometer | I2C0 | PB8 SCL, PB9 SDA | Board/device-specific address |

Sensor selections retained from the supplied config: ICM42688P, LSM6DSV16X, LSM6DSK320X and BMI270; gyro alignment CW90_DEG. Flash uses the M25P16-family driver. These definitions are not proof of the actually populated devices.

External ADC uses **ADC2, DMA option 1**, selecting GD32 DMA1 channel 1 (CLI DMA2 channel 1). Inputs remain PC0 voltage, PC1 current and PC2 RSSI, all supported on ADC2 by the GD32 driver. Voltage scale 210 and current scale 100 require hardware calibration.

This deliberately differs from the supplied ADC1/option-0 configuration: ADC1 option 0 conflicts with Motor 4 on DMA1 channel 2, and ADC1 option 1 conflicts with Motor 3 on DMA1 channel 3. ADC2 option 0 would instead compete with flash RX DMA; option 1 avoids these allocations. Existing saved settings must also select ADC2 option 1 (or be reset to the corrected board defaults); flashing alone preserves old ADC settings.

### DMA budget for per-channel DShot

Controller/channel numbers here are GD32's zero-based names; CLI DMA controller labels are one higher.

| Function | Timer/peripheral | DMA controller / channel |
|---|---|---|
| Motor 1, PC9 | TIMER7 CH3 | DMA1 / 7 |
| Motor 2, PC8 | TIMER7 CH2 | DMA1 / 4 |
| Motor 3, PC7 | TIMER7 CH1 | DMA1 / 3 |
| Motor 4, PC6 | TIMER7 CH0 | DMA1 / 2 |
| Motor 5, PB11 | TIMER1 CH3 | DMA0 / 7 |
| Motor 6, PB10 | TIMER1 CH2 | DMA0 / 1 |
| LED strip, PA0 | TIMER4 CH0 | DMA0 / 2 |
| External ADC | ADC2, option 1 | DMA1 / 1 |
| Flash TX / RX | SPI0, automatic allocation | DMA1 / 5, DMA1 / 0 |
| OSD TX / RX | SPI1 | DMA0 / 4, DMA0 / 3 |
| Gyro TX / RX | SPI2, automatic allocation | DMA0 / 5, DMA0 / 0 |

These routes are statically conflict-free for the declared motors/LED/ADC/SPI set with burst and bitbang DShot OFF. **Do not enable burst DShot with this ADC route without remapping DMA:** TIMER7's update request also uses DMA1 channel 1. UART DMA, SDIO, remapped outputs or different DShot modes require a fresh audit. Flash TX relies on automatic allocation choosing option 1 because option 0 is occupied by Motor 3. Conventional servo PWM does not claim a DMA route in this budget. Use `dma show` and output/ADC/logging tests on the actual board to validate runtime behavior.

## Outputs and defaults

- Motors 1–6: PC9, PC8, PC7, PC6, PB11, PB10.
- Servos 1–2: PA10, PA8.
- LED strip: PA0; status LED: PB0; inverted beeper: PB1.
- PINIO1: PC14, inverted, `12V BEC OFF`; PINIO2: PC15, inverted, `CAM SWITCH`.
- Timer alternate selections and DMA options are retained from the supplied config and correspond to entries in the GD32F4 timer table. Output timing and DMA function still need hardware tests.
- PID process denominator: 2. DShot bitbang and DShot burst defaults: OFF.
- GPS, position/altitude hold, servos, magnetometer, OSD and CRSF/SBUS/telemetry feature flags are retained as supplied, not functionally validated.

## Flashing and validation

Use only the board-specific HEX. Back up existing settings first. When migrating from a different bus-numbering convention, load this profile's defaults and selectively reapply settings; a full old dump can overwrite the corrected pin/bus assignments. SWD/ROM DFU flashing and recovery must be validated on this actual board; HAKRCF460V2 results do not establish ARROW compatibility.

Hardware status: **untested**. The board revision/package, populated sensor variant, flash identity/geometry, schematics and manufacturer metadata have not been independently confirmed beyond the supplied header. Before publication collect exact firmware version/status, boot/USB evidence, sensor readings, flash read/write evidence, ADC calibration, functional UART/receiver traffic, output traces and recovery evidence. GPS/hold, servos, telemetry, OSD, failsafe, arming and flight remain untested.

A pre-commit ARROW image passed a limited boot/USB/CLI smoke test on a **HAKRCF460V2**, using ARROW defaults after clearing saved configuration. CLI identified ARROWGD460V5 and exposed the intended resources; the DPS310 was detected on shared I2C pins. No gyro or SPI flash was detected, as expected for the different layout. The original HAKRC image/settings were restored and verified. This surrogate-board result is not validation of actual ARROW hardware or its outputs, and does not clear the release hardware gate.
