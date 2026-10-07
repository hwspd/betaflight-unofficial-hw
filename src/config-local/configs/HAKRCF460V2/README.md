# HAKRCF460V2 UART mapping

The CLI and an unmodified Configurator now use one-based UART names. Internal GD32 pin definitions stay zero-based so the peripheral/IRQ/DMA selection does not change.

| Board / CLI / Configurator | Internal macro | TX | RX |
|---|---|---|---|
| UART1 | UART0_* | PB6 | PB7 |
| UART2 | UART1_* | PA2 | PA3 |
| UART3 | UART2_* | PB10 | PB11 |
| UART4 | UART3_* | PA0 | PA1 |
| UART5 | UART4_* | PC12 | PD2 |
| UART6 | UART5_* | PC6 | PC7 |

External UART2 has inverter control PC0 (`INVERTER_PIN_UART1` internally). External UART1 has no configured external inverter. CRSF on PB6/PB7 previously passed at 251 Hz when the CLI called this port UART0; the new external name is UART1 and the underlying connection is unchanged. That earlier result is not a substitute for validating a new firmware artifact. Physical UART2 receiver operation remains unresolved.

Existing binary settings from the corrected zero-based profiles keep the same physical UART assignments. Old CLI exports require UART-name translation; see [the shared migration notes](../../README.md). Firmware with the original September 3 incorrect pin mapping needs a pin-default correction as well. ADC2 DMA option 0 and existing motor/timer settings are unchanged by this naming fix.
