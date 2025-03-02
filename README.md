# B31DG_Assignment1

## Overview
This repository contains the source code and project structure for an embedded software assignment focused on developing a bare-metal software application on an ESP32-based module. The project involves generating a pair of periodically repeating digital signals controlled via two push buttons.

The implementation is carried out in two stages:
1. **Arduino IDE Implementation** – The initial implementation is done using the Arduino framework.
2. **ESP-IDF Implementation** – The software is then reimplemented using the Espressif Internet of Things Development Framework (ESP-IDF).

## Repository Structure

```
assignment1_vsCode/
│── .devcontainer/         # Configuration files for development container (if used)
│── .vscode/               # VS Code-specific settings and configurations
│── main/                  # Contains the main ESP-IDF source files
│   │── CMakeLists.txt     # CMake configuration file for ESP-IDF build system
│   │── assignment1.c      # Main source file for the ESP-IDF implementation
│   │── assignment1.h      # Header file defining functions and constants
│── .gitignore             # .gitignore file to exclude build folder from git repository
│── CMakeLists.txt         # Top-level CMake configuration file
│── README.md              # Project documentation (this file)
│── sdkconfig              # ESP-IDF configuration file
│── sdkconfig.ci           # ESP-IDF configuration file for CI/CD
│
│── embeddedsoftware_ass1/  # Contains the Arduino IDE implementation
│   │── embeddedsoftware_ass1.ino  # Arduino-based implementation file
│── README.md              # Documentation specific to the Arduino implementation
```

## Project Implementation Details
- The **`main/` directory** contains the ESP-IDF implementation, where `assignment1.c` handles signal generation and button interactions.
- The **`main/` directory** contains the ESP-IDF implementation, where `assignment1.h` handles defining functions, constants and importing additional libraries  
  for the main application.
- The **`embeddedsoftware_ass1/` directory** contains the initial Arduino implementation in the `emmbeddedsoftware_ass1.ino` format.


## Development Environment
- The ESP-IDF implementation can be developed using **Visual Studio Code** with the **ESP-IDF extension**.
- The Arduino implementation requires the **Arduino IDE** with the appropriate ESP32 board support package installed.

