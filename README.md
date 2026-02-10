# chip8

A simple CHIP-8 interpreter made using C and SDL2.

## dependencies

- SDL2

## build instructions

1. Make sure SDL2 is installed in your system. If not, install it on your system:

2. Clone this repository & navigate to it
```sh
git clone git@github.com:codexagon/chip8-interpreter.git
cd chip8-interpreter
```

3. Compile the binary
```sh
gcc -o chip8 *.c -lSDL2
```

4. Download a CHIP-8 ROM file

5. Run the compiled binary
```sh
./chip8 <downloaded-rom-file>
```

## how to use

Microcomputers running CHIP-8 in the mid-1970s had a hex keypad of 16 values ranging from 0 to F. Its layout was:

|  1  |  2  |  3  |  C  |
| :-: | :-: | :-: | :-: |
|  4  |  5  |  6  |  D  |
|  7  |  8  |  9  |  E  |
|  A  |  0  |  B  |  F  |

These keys are mapped to the left side of a standard QWERTY keyboard, like

|  1  |  2  |  3  |  4  |
| :-: | :-: | :-: | :-: |
|  Q  |  W  |  E  |  R  |
|  A  |  S  |  D  |  F  |
|  Z  |  X  |  C  |  V  |

To exit the program, press `Enter`.

## gallery
Pong:
<img width="1300" height="657" alt="image" src="https://github.com/user-attachments/assets/eab4883c-d7a9-442f-b5c0-2177d15b2386" />

Tetris:
<img width="1291" height="657" alt="image" src="https://github.com/user-attachments/assets/2b9aee8e-79bd-4eb3-871a-07959f6eb4c2" />
