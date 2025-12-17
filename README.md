# 💎 Splendor Duel Digital

![C++](https://img.shields.io/badge/Language-C++17-blue.svg)
![Qt](https://img.shields.io/badge/Framework-Qt_6-green.svg)
![Build](https://img.shields.io/badge/Build-CMake-orange.svg)
![License](https://img.shields.io/badge/License-MIT-lightgrey.svg)

**Splendor Duel Digital** is a digital adaptation of the famous board game *Splendor Duel*, developed in C++ using the Qt framework. This project was created as part of the LO21 course (Object-Oriented Design and Programming) at the University of Technology of Compiègne (UTC).

The main objective is to provide a robust implementation that respects OOP principles, with a strict separation between business logic (Core) and the user interface (Qt).

---

## Features

* **Complete Rules:** Faithful implementation of the official Splendor Duel rules (taking tokens, buying cards, privileges, crowns, etc.).
* **Graphical User Interface (GUI):** Immersive interface built with Qt Widgets, featuring drag-and-drop functionality and visual animations.
* **Save & Load:** Data persistence system using JSON format (via the `nlohmann/json` library).
* **2-Player Mode:** Management of turns, victory conditions, and player interactions.
* **MVC Architecture:** Clear separation between data (`src/core`) and display (`src/Qt`).

---

## Technologies Used

* **Language:** C++ (Standard C++17)
* **Interface:** Qt 6 (Modules: Core, Widgets)
* **Build System:** CMake (Min. version 3.16)
* **External Dependencies:**
    * [nlohmann/json](https://github.com/nlohmann/json) for serialization (automatically downloaded via CMake FetchContent).

---

## Installation and Build

### Prerequisites
Ensure you have the following installed:
* A C++17 compatible compiler (GCC, Clang, or MSVC).
* **CMake** (version 3.16 or higher).
* **Qt 6** (development packages `qt6-base` or via the Qt Installer).

### Build Instructions

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/leopoldch/splendor-duel-digital.git
    cd splendor-duel-digital
    ```

2.  **Configure and Compile (Command Line):**
    It is recommended to perform an "out-of-source" build to keep the directory clean.

    ```bash
    mkdir build
    cd build

    cmake ..

    make
    ```

    Build with debug symbols: 

    ```
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    ```

3.  **Run the Game:**
    Once compilation is complete, the executable will be located in the `build` folder.
    ```bash
    ./lo21-splendor-duel
    ```

> **Note for macOS/Linux:** If you encounter errors related to `Qt6_DIR`, you may need to specify the path to Qt during CMake configuration:
> `cmake -DCMAKE_PREFIX_PATH=/path/to/qt6 ..`

---

## Authors

Project created by:
* Léopold Chappuis
* Aubin Vert
* Alexandru Ghitu
* Gaspard Petri
* Quentin Fouinat
