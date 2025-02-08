# CHIP-8 Emulator

## Overview
This is a CHIP-8 emulator written in C++ using SDL2 for rendering and input handling. The emulator allows you to run CHIP-8 programs (ROMs) on your system.

## Features
- **CPU Emulation**: Implements the CHIP-8 instruction set.
- **Memory Management**: Loads and executes CHIP-8 programs.
- **Input Handling**: Maps keyboard input to CHIP-8 keys.
- **Graphics**: Uses SDL2 to render the CHIP-8 display.
- **Audio Support**: Implements CHIP-8 sound using SDL2.
- **Debug Mode**: Includes debugging output and logging.

## Requirements
- C++17 or later
- SDL2 library

## Installation
### 1. Clone the repository
```sh
git clone https://github.com/Adnan-Adel/chip8-emulator.git 

cd chip8-emulator
```

### 2. Install dependencies (Ubuntu/Debian)
```sh
sudo apt update
sudo apt install libsdl2-dev
```

### 3. Build the emulator
```sh
make
```

### 4. Build in debug mode (optional)
```sh
make debug
```

### 5. Clean build files
```sh
make clean
```

## Usage
Run the emulator with a CHIP-8 ROM:
```sh
./chip8-emulator path/to/rom.ch8
```
Or in debug mode:
```sh
./chip8-emulator-debug path/to/rom.ch8
```

## Controls
The CHIP-8 keypad is mapped to your keyboard as follows:
```
 CHIP-8 Keypad      Keyboard Mapping
 -----------------  ----------------
 | 1 | 2 | 3 | C |    | 1 | 2 | 3 | 4 |
 | 4 | 5 | 6 | D | -> | Q | W | E | R |
 | 7 | 8 | 9 | E |    | A | S | D | F |
 | A | 0 | B | F |    | Z | X | C | V |
```

## License
This project is licensed under the MIT License.

## Contributing
Pull requests are welcome! If you find any issues or have suggestions, please open an issue on GitHub.

## Author
[Your Name](https://github.com/your-username)


