# GD32H757V1STARTV1 — Schematic vs Firmware Discrepancies

Board: GD32MC-BetaFlight-H757 (GD32H757VxTx, U1)
Source of truth: `GD32MC-BetaFlight-H757-V1.0.pdf` (connection list)
Audit date: 2026-08-13

This documents every mismatch found between the schematic and the board's
Betaflight target definition (`src/config-local/configs/GD32H757V1STARTV1/config.h`)
plus related silicon/SDK findings. All firmware-side corrections are committed to
the `gd32h7-dev` branch.

---

## 1. Pin mapping errors (config.h → corrected)

| Signal | config.h (wrong) | Schematic (correct) | MCU pin |
|---|---|---|---|
| LED0 | PE3 | **PE5** | 4 |
| LED1 | PE1 | **PE4** | 3 |
| BEEPER | PD3 | **PE3** | 2 |
| LED_STRIP | PB1 | **PA8** | 67 |
| SPI2_SCK | PB3 | **PC10** | 78 |
| SPI2_MISO | PB4 | **PC11** | 79 |
| SPI2_MOSI | PB5 | **PB2** | 36 |
| SPI3_SCK | PE2 | **PE12** | 42 |
| SPI3_MISO | PE5 | **PE13** | 43 |
| SPI3_MOSI | PE6 | **PE14** | 44 |
| GYRO_2_CS | PE4 | **PE11** | 41 |
| GYRO_2_EXTI | PC13 | **PC4** | 32 |
| ADC_VBAT | PC0 | **PC1** | 16 |
| ADC_CURR | PC1 | **PC3** | 18 |
| MOTOR1 | PE9 | **PA0** | 22 |
| MOTOR2 | PE11 | **PA1** | 23 |
| MOTOR3 | PE13 | **PA2** | 24 |
| MOTOR4 | PE14 | **PA3** | 25 |
| MOTOR5 | PC6 | **PB0** | 34 |
| MOTOR6 | PC7 | **PB1** | 35 |
| UART3_TX | PB8 | **PD1** | 82 |
| UART3_RX | PB9 | **PD0** | 81 |
| UART7_RX | PE0 | **PE2** | 1 |
| RX_PPM | PA10 | **PB9** | 96 |

Notes:

* The `SPI2`/`SPI3` pin errors meant the on-board flash (GD25Q128) and the
  second ICM42688P gyro were wired to pins the config never drove.
* The motor pins were entirely on the wrong timers/channels; `TIMER_PIN_MAPPING`
  was regenerated for TIMER1 (PA0–PA3), TIMER2 (PB0/PB1), TIMER7 (PC8/PC9).
* The LED/BEEPER/LED_STRIP pins were mutually scrambled (LED0 on the beeper pin,
  etc.).

## 2. Sensor identification errors

| Sensor | Schematic | config.h (wrong) | Fix |
|---|---|---|---|
| Barometer | **GDY1121** (GigaDevice) | SPL07-003 / DPS310 / BMP280 / MS5611 | `USE_BARO_GDY112X` |
| Magnetometer | QMC5883P (I2C1) | (absent) | `USE_MAG_QMC5883P`, `MAG_I2C_INSTANCE I2CDEV_1` |

The barometer is mislabeled "GDY1121/SPL07-003" on the schematic notes. The two
chips are **not** register-compatible:

| | GDY1121 | SPL07-003 (≈DPS310) |
|---|---|---|
| I2C address | **0x3A** (SA0 low) / 0x3B | 0x76 / 0x77 |
| Chip-ID register | 0x00 | 0x0D |
| Chip ID | 0xA0 | 0x11 |

Configuring the DPS310/SPL07-003 driver (scans 0x76) can therefore never find the
GDY1121. A `barometer_gdy112x` driver was ported in to fix this.

## 3. Silicon / SDK findings (not schematic, but related)

* **FMC (flash controller) base** is `0x52002000`, not `0x51102000` as the
  GD32H7xx SDK `gd32h7xx.h` defines (`FMC_BASE = AHB3 0x51000000 + 0x01002000`).
  Verified on hardware (unlock-key round-trip). Matches the STM32H7 layout and the
  Zephyr GD32H7 devicetree.
* **Flash size is 1 MB** (`0x08000000`–`0x08100000`), not the 2 MB assumed by the
  Betaflight linker script (`gd32h757xi_flash.ld`) and `target.mk`
  (`MCU_FLASH_SIZE := 2048`). Reads fault at `0x08100000`. 4 KB uniform erase
  sectors.
* **ADC numbering** is 0-based on GD32 (ADC0/1/2); `adcDeviceByInstance()` only
  handled ADC1+, so `adcInit()` silently returned early and never enabled the ADC
  clock (caused the boot hang).

## 4. Root-cause summary (the three boot/detection bugs)

1. **Boot hang** — `adcInit()` returned early (ADC0 not handled in
   `adcDeviceByInstance()`), so the ADC2 clock was never enabled and
   `adcInternalInit()` spun forever waiting for the end-of-injected-conversion
   flag.
2. **No gyro** — `spiHardware[]` had no GD32H7 entries, so SPI was never
   initialized (no clock, no pins) and the ICM42688P gyros were never detected.
3. **No baro** — the barometer is a GDY1121, not SPL07-003; the wrong driver was
   configured.

Firmware fixes for (1) and (2) are in commit `6fc70cf7c`; (3) is the GDY1121
driver port.
