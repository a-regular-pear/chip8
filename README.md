# CHIP-8 Emulator

A straightforward CHIP-8 emulator written in C++ using SDL2 for graphics, input, and audio.

## Prerequisites

You need a C++ compiler (`g++`), `make`, and the SDL2 development library.
On Ubuntu/Debian, you can install SDL2 with:

```bash
sudo apt install libsdl2-dev
```

## How to Build

Open your terminal in the project folder and run:

```bash
make
```

To clear the generated build files, run `make clean`.

## How to Run

Execute the compiled program by providing the path to your CHIP-8 ROM file as a command-line argument:

```bash
./chip8 path/to/your/rom.ch8
```

## Controls

The emulator maps the original 16-key CHIP-8 hex pad to the left side of your keyboard:

| CHIP-8 Keypad | Your Keyboard |
| :---: | :---: |
| 1 2 3 C | 1 2 3 4 |
| 4 5 6 D | Q W E R |
| 7 8 9 E | A S D F |
| A 0 B F | Z X C V |