# fortuna-5

A powerful Z80 computer. It contains:

- Z80 CPU running at 25 Mhz
- 512 kB RAM (bankswitched in blocks of 64 kB)
- DMA and Memory management
- SD Card storage
- VGA text and graphics
- Wi-Fi and internet access
- SPI interface for external boards
- Serial (UART) interface
- Runs both a custom operating system (Fortuna-OS) and CP/M

# I/O ports

| Port | Direction | Description |
|------|-----------|-------------|
| `03` | OUT       | Print char to serial output |
| `03` | IN        | Return last inputted char on serial |