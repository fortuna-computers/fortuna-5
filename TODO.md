# Emulator

- [x] Setup f-debugger
- [x] Write base emulator

# Computer

- [x] Create schematic
- [x] Create computer circuit
- [x] Ack
- [x] RAM
- [x] CPU
  - [x] Full step

# I/O logic

- [x] Write char to terminal
  - [x] Emulator
  - [x] Real computer (debugger)
    - [x] Add I/O chip (test)
- [x] Read char from terminal
  - [x] Emulator
  - [x] Real computer (debugger)
- [x] Real computer
  - [x] Add I/O chip (real)
  - [x] Add clock chip
  - [x] Debugger logic
- [x] ROM upload
  - [x] Compile BIOS on build
  - [x] Generate binary on source file (current data: 1614)
  - [x] Record BIOS on initialization

# Improvements

- [x] Move ROM to AVR ROM instead of RAM
- [x] Recreate build system, resolve warnings
- [x] Interrupts
- [x] Read keyboard
  - [x] Read/write UART cycle in...
    - [x] Emulator
    - [x] Debugger
    - [x] Runner
- [ ] Move to debug/run at will

# SDCard

- [ ] Load data in Memory 1
  - [ ] Implement SPI/SD in Atmega
  - [ ] Emulator
- [ ] Control SD from debugger
  - [ ] Protobuf commands
  - [ ] UI
  - [ ] Computer (FsFat)
    - [ ] Emulator
    - [ ] ATmega
- [ ] Define SD commands
  - [ ] Implement SD commands
- [ ] Write boot code (assembly)

# I/O board

- [ ] Create schematic
- [ ] Define I/O commands
- [ ] Write VGA code

# Fortuna code

- [ ] Monitor
- [ ] Base operating system
  - [ ] BASIC
- [ ] CP/M mode

# Page
- [ ] Create page
- [ ] Create videos