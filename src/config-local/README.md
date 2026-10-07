# Fork-local board profiles

Build a profile using `make CONFIG_DIR=src/config-local CONFIG=<board> EXTRA_FLAGS=-Werror`.

## UART naming at the firmware interface

Hardware UART names exposed by CLI and MSP start at **UART1** for both zero-based and one-based MCU families. An unmodified Configurator therefore displays UART1 for the first hardware UART. PIO UART, LPUART, soft-serial and USB namespaces are unchanged.

GD32 hardware definitions remain zero-based internally. Do not shift `UART0_TX_PIN`, `UART0_RX_PIN`, driver identifiers, IRQs or DMA-table indexes to change the displayed name. The firmware translates only external UART names and protocol identifiers:

| GD32 internal peripheral / config macro | Internal saved identifier | CLI name | MSP identifier |
|---|---|---|---|
| USART0 / UART0_* | 50 | UART1 | 51 |
| USART1 / UART1_* | 51 | UART2 | 52 |
| USART2 / UART2_* | 52 | UART3 | 53 |
| UART3 / UART3_* | 53 | UART4 | 54 |
| UART4 / UART4_* | 54 | UART5 | 55 |
| USART5 / UART5_* | 55 | UART6 | 56 |
| UART6 / UART6_* | 56 | UART7 | 57 |
| UART7 / UART7_* | 57 | UART8 | 58 |

Only ports with available resources are reported; gaps are preserved. For example GD32H757V1STARTV1 has no UART5 hardware pin assignment, so external UART6 can be absent while UART7/UART8 remain correctly numbered.

Both MSP serial-configuration versions translate reads and writes, as does MSP serial-ID passthrough. CLI named/numeric serial assignments and numeric passthrough use the same external namespace. Modern identifier 50 is rejected on a zero-based target; it is not an alias for UART1. CLI `serial` retains its legacy numeric syntax (0 corresponds to external UART1) where that syntax was already supported.

### Existing configurations and scripts

- Internal saved serial identifiers and resource arrays are unchanged. Upgrading from the corrected zero-based GD32 profiles preserves the selected physical ports without a reset.
- Reconnect Configurator after upgrading; do not reuse cached pre-upgrade MSP identifiers.
- **Old text exports/scripts use the old names.** Translate old GD32 `UART0` to `UART1`, old `UART1` to `UART2`, etc., when importing their serial commands. Old UART0 is rejected, but other old names overlap valid new names and cannot be recognized automatically.
- Do not shift `resource SERIAL_TX`, `resource SERIAL_RX`, inverter resource indexes, GPIO names or DMA values. Their numbering is unchanged.
- This does not repair configurations saved with the original incorrect UART pin definitions from the September 3 HAKRC build. Those require corrected pin defaults or an explicit pin-map migration first.
- CLI commands on already one-based targets such as STM32 keep their existing names and identifiers.

Board-specific README files describe physical layouts and hardware-validation limits.
