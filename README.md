# Analog Voltage Measurement using SPI ADC (MCP3204) with LPC2129

Multi-channel analog voltage monitoring system using LPC2129 and SPI-based ADC (MCP3204).  
Voltage values are read from the MCP3204, processed by the LPC2129 microcontroller, displayed on an LCD, and channels are selected using UART commands.

---

## Features
- 4-channel, 12-bit analog input (MCP3204)
- SPI communication between LPC2129 and MCP3204
- Channel selection and commands via UART (serial)
- Voltage displayed on an LCD and optionally streamed over UART
- Simple calibration and scaling for real-world volt readings

---

## Hardware components
- NXP LPC2129 microcontroller (ARM7TDMI-S)
- Microchip MCP3204 (12-bit, 4-channel SPI ADC)
- Character LCD (e.g., 16x2) or any compatible display
- Serial interface (USB-UART or TTL UART) for user commands and logging
- Power supplies and decoupling (observe ADC reference voltage requirements)
- Wires / breadboard / PCB

---

## MCP3204 quick facts
- 12-bit resolution (0..4095)
- 4 single-ended channels (or differential)
- SPI interface
- Requires a stable reference voltage (VREF) — the conversion result is relative to VREF
- Datasheet: [MCP3204 Datasheet — Microchip](https://ww1.microchip.com/downloads/en/DeviceDoc/21291b.pdf) (consult for timing and max SPI clock)

---

## Example wiring (adjust pins to match your board)
Note: These are example LPC2129 pin labels — change them to the actual pins used on your board.

- MCP3204 VDD -> 5V or 3.3V (match MCU and system design)
- MCP3204 VREF -> VDD (or a separate precision reference)
- MCP3204 GND -> GND

SPI connections:
- MCP3204 CS (Chip Select / /CS) -> LPC2129 GPIO (example P0.16)
- MCP3204 SCLK -> LPC2129 SPI SCK (example P0.15)
- MCP3204 DIN (MOSI) -> LPC2129 SPI MOSI (example P0.18)
- MCP3204 DOUT (MISO) -> LPC2129 SPI MISO (example P0.17)

UART connections:
- LPC2129 UART TX -> USB-UART RX (for serial console)
- LPC2129 UART RX -> USB-UART TX

LCD:
- Connect data/command pins to chosen GPIOs or an I2C/SPI backpack if used.

Important: Tie CS high when idle. Use common ground between MCU and ADC.

---

## SPI settings (recommended)
- Mode: SPI Mode 0 (CPOL = 0, CPHA = 0) — verify with datasheet
- Bit order: MSB first
- Clock: start with a conservative clock (e.g., ≤ 1 MHz) then increase while respecting MCP3204 and LPC2129 specs and signal integrity
- CS: active low

---

## How MCP3204 is read (concept)
1. Pull CS low.
2. Send the control bits to select the conversion (start bit + single/diff + channel bits).
3. Clock out enough bits to receive the conversion result.
4. Combine received bytes into a 12-bit value.
5. Pull CS high.

Example (pseudocode outline):
- Send control byte(s) to request channel N conversion
- Read 2–3 bytes back
- Extract 12-bit ADC value (0..4095)
- Convert to voltage: V = ADC_value * Vref / 4095

(Exact control bit sequence and byte alignment depend on how you implement the SPI transfer — consult the MCP3204 datasheet for the exact timing/control sequence and sample code.)

---

## Voltage conversion
For a 12-bit ADC (0..4095), convert to volts with:

V = (ADC_value / 4095.0) * VREF

Example (VREF = 5.0 V):
- If ADC_value = 2048 → V ≈ (2048/4095) * 5.0 ≈ 2.50 V

If you use a lower VREF (e.g., 3.3 V), use that value in the formula.

---

## UART commands (example protocol)
The firmware uses a simple UART command set to select channels and control output. Example command set (you can adapt to your preference):

- `0` — Select channel 0 and display/send its voltage
- `1` — Select channel 1
- `2` — Select channel 2
- `3` — Select channel 3

Serial config (example):
- Baud: 9600 (common default)
- Data bits: 8
- Parity: None
- Stop bits: 1
- Flow control: None

You can print human-readable lines like:
CH1: 2.498 V (ADC: 2044)

---

## Firmware structure (suggested)
- main.c
  - Setup: clock, UART, SPI, GPIO, LCD
  - Main loop: handle UART input, perform ADC reads, update LCD
- spi_adc.c / spi_adc.h
  - Initialization and read function for MCP3204: uint16_t read_adc(uint8_t channel);
- uart.c / uart.h
  - Serial receive and transmit helpers
- lcd.c / lcd.h
  - LCD helper functions (init, print, clear)
- config.h
  - Pin definitions, VREF value, UART baud rate, polling interval

---

## Build & flash (examples)
You may be using Keil MDK-ARM or GCC + OpenOCD + a programmer. Example steps (adjust to your toolchain):

With Keil:
1. Open the provided uVision project.
2. Build (Project -> Build target).
3. Flash using your programmer (Flash -> Download).

Make sure your LPC2129 startup code, linker script, and vector table are configured for your board.

---

## Calibration & accuracy tips
- Use a stable, accurate VREF for correct voltage readings.
- If measuring voltages above VREF or different ranges, use appropriate resistor dividers; account for the divider in software scaling.
- Average multiple samples to reduce noise.
- Add low-pass filtering (hardware RC or software averaging) if readings are noisy.

---

## Troubleshooting
- No readings / constant zero:
  - Check CS, SCLK, MOSI, MISO wiring and common ground.
  - Verify MCU SPI pins are configured correctly.
  - Check VREF and power to MCP3204.
- Strange / fluctuating values:
  - Add decoupling capacitors to MCP3204 power pins.
  - Reduce SPI clock or check signal integrity.
  - Average samples.
- UART commands not recognized:
  - Verify baud rate, TTL levels, and wiring.
  - Use a USB-UART adapter and a terminal (e.g., PuTTY, minicom) to test.

---

## Notes & customization
- Adjust example pin assignments and SPI parameters to match your hardware.
- If your MCP3204 uses differential mode, adjust the read sequence and scaling accordingly.
- Use proper protection (input clamping, resistor dividers) if measuring signals outside MCU/ADC range.

---

## References
- MCP3204 Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/21291b.pdf
- LPC2129 User Manual / Datasheet: consult NXP documentation for pins and peripheral setup
