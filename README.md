# Terminix 
A tiling terminal emulator for Linux and high POSIX-compatible operating systems. Written in QtQuick/QML 6.7 and C++. 

## Features
- Open and tile multiple terminal instances in a single application window 
    - `Ctrl+Shift+E` to spawn a new terminal vertically 
    - `Ctrl+Shift+O` to spawn a new terminal horizontally
    - `Ctrl+M` to toggle maximizing window 
- Each terminal instance is resizable by dragging the splitter bar 
- Muted dark theme with high contrast fonts and basic output coloring 

## Developer setup 
### Requirements: 
- Qt 6.7.1
- CMake 3.29.3
- GCC 14.1.1
- Bash executable at `/usr/bin/bash`
- Bash history at `~/.bash_history`
    
### Recommended:
- QtCreator 13.0.1 based on Qt 6.7.1
- Visual Studio Code of any version for easy markdown editing and preview 

### Build instructions: 
1. Clone the repository
2. Open the project in QtCreator any IDE/text editor of choice 
3. Create a `build` directory in the project root
4. To build and run the project: 
    
    - `Ctrl+R` inside QtCreator, or 
    - Open a terminal in the `build` directory and run 
    ```
    cmake .. && cmake --build . && ./appterminix
    ```

## User installation 
This product is not ready to be shipped to end-users. Please refer to the developer setup section for building and running the application.

User installation guide will be updated in due time :) 