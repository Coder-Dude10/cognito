# Cognito
![Cognito Logo](https://github.com/Coder-Dude10/cognito/blob/main/Images/Cognito%20Logo.png?raw=true)
A game engine for Nintendo Switches/Switch Lites/Switch OLEDs running Atmosphère OS. Brainfk++ is the only programming language that Cognito supports.

[Download the Latest Release](https://github.com/Coder-Dude10/cognito/releases)
# Brainfk++
![Brainfk++ Logo](https://github.com/Coder-Dude10/cognito/blob/main/Images/Brainfk%2B%2B%20Logo.png?raw=true)
Brainfk++ is a programming language comprised of 22 instructions, with each instruction represented by an ASCII character. It expands on the work of Urban Müller by introducing more instructions, thereby simplifying the development process and enabling the creation of more complex programs.
# Brainfk++ Instruction Set
| Instruction | Description | Parameters |
| --- | --- | --- |
| + | Increment current cell value by 1. | - |
| - | Decrement current cell value by 1. | - |
| < | Move cell pointer left by 1 cell. | - |
| > | Move cell pointer right by 1 cell. | - |
| [ | If **currentCellValue** == 0, goto cell after corresponding ']' instruction. | currentCellValue |
| ] | If **currentCellValue** != 0, goto cell after corresponding '[' instruction. | currentCellValue |
| , | Set the value of the current cell to the keycode of the key currently being pressed (none = 0, leftJoystickLeft = 1, leftJoystickRight = 2, leftJoystickUp = 3, leftJoystickDown = 4, A = 5, B = 6, X = 7, Y = 8) (**mode** == 0) or set the value of the current and next cell to leftJoystickX and leftJoystickY, respectively (**mode** == 1). | mode |
| . | Create an object that retreives its attributes (**objectIndex**, **objectType** (integer = 0, character = 1, sprite = 2), **objectColor**, **x**, **y**) from the current and following cells and updates itself when said cells are modified, or set the background to **objectIndex** (**objectType** == 3). **NOTE:** Sprites and audio files must be named "\<integer > -1\>.png" and backgrounds must be named "\<integer > 0\>.png". | objectIndex, objectType, x, y |
| ; | End of program branch. | - |
| : | Destroy all objects. | - |
| ? | Set the value of the current cell to a random integer from 0 to **maximumIntegerValue**. | maximumIntegerValue |
| ! | Play the audio file at **audioIndex** as a sound (doesn't loop and is played at 50% volume) if its length is less than 30 seconds. Otherwise, play said audio file as music (loops and is played at 100% volume). **NOTE:** Only WAV files are supported. | audioIndex |
| ~ | Pause for (**duration** / 10) seconds. | duration |
| ^ | Set the cell pointer to **cellIndex**. | cellIndex |
| = | Set the value of the current cell to the value of the cell at **cellIndex**. | cellIndex |
| # | Set the value of the current cell to either 0 (**currentCellValue** != 0) or 1 (**currentCellValue** == 0). | currentCellValue |
| \| | Set the value of the current cell to either 0 (**currentCellValue** < 0) or 1 (**currentCellValue** > -1). | currentCellValue |
| @ | Execute the program branch at **programTreeIndex**. | programTreeIndex |
| / | Set the value of the buffer cell to **currentCellValue**. | currentCellValue |
| \ | Set the value of the current cell to the value of the buffer cell. | - |
| & | Set the value of the current cell to (**currentCellValue** * 2) (a.k.a. a bit shift). | currentCellValue |
| _ | Skip the next instruction. | - |
# Brainfk++ Character Set
![Brainfk++ Character Set](https://github.com/Coder-Dude10/cognito/blob/main/Images/Brainfk%2B%2B%20Character%20Set.png?raw=true)
# Brainfk++ Error Codes
| Error | Description |
| --- | --- |
| 1 | Minimum of cell memory reached. |
| 2 | Maxiumum of cell memory reached. |
| 3 | Corresponding ']' instruction not found. |
| 4 | Corresponding '[' instruction not found. |
| 5 | Asset not found. |
| 6 | Invalid index. |
