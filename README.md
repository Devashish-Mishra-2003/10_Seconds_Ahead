## 10 Seconds Ahead: A Tactical C++ Puzzle Game
10 Seconds Ahead is a grid-based tactical puzzle game developed in C++ using the SFML (Simple and Fast Multimedia Library) framework. The game challenges players with a unique time-delayed mechanics system where movement must be planned 10 seconds in advance before being executed in a dangerous, hazard-filled environment.

## Overview
The core gameplay loop is divided into two distinct phases: Planning and Executing. During the Planning phase, the player has a 10-second window to input a sequence of moves and place defensive blocks. Once the timer expires, the Execution phase begins, and the player character follows the pre-programmed path. Success depends on the player's ability to predict hazard patterns—such as laser beams and cannon projectiles—and navigate safely to collect all items.

## Screnshoot
<img width="1919" height="989" alt="image" src="https://github.com/user-attachments/assets/e30c982a-e730-42ba-a909-7890f303cec2" />
<img width="1919" height="987" alt="image" src="https://github.com/user-attachments/assets/f31fe60a-85aa-4ea7-80d8-6c890ce2cac3" />
<img width="1919" height="987" alt="image" src="https://github.com/user-attachments/assets/de801408-da61-4bc2-8824-2c37bfd2fd35" />
<img width="1919" height="986" alt="image" src="https://github.com/user-attachments/assets/60162bc1-f93a-4d9d-9ee9-cedb73ce5b5c" />
<img width="1919" height="987" alt="image" src="https://github.com/user-attachments/assets/5a4a6338-d128-4855-a2f9-608a38ba59ea" />
<img width="1919" height="990" alt="image" src="https://github.com/user-attachments/assets/c86fc7e2-1c9e-4ec4-baf8-22606d0814a3" />

## Key Features
- **Predictive Movement System**: A command-queue-based movement system that separates input from execution.
- **Dynamic Hazard System**: Includes real-time projectiles (Cannons) and expanding/collapsing environmental hazards (Lasers).
- **Strategic Block Placement**: Players can place a limited number of blocks to obstruct hazards or modify the pathing environment.
- **Undo/Redo Logic**: A robust LIFO (Last-In, First-Out) action history system allowing players to undo moves or block placements during the planning phase.
- **Difficulty Scaling**: Adjustable difficulty settings (Easy, Normal, Hard) that modify block availability and turn limits.
- **Custom UI Framework**: A custom-built ElevatedButton system featuring smooth scale animations, shadow offsets, and state-based color shifts.
- **Letterbox Rendering**: Resolution-independent rendering using a calculated viewport to maintain aspect ratio across different window sizes.

## Technical Architecture
The project is designed using an Object-Oriented approach with a clear separation of concerns:
- **Game Engine (Game.h/cpp)**: Manages the high-level state machine, UI transitions, and the main game loop.
- **Grid System (Grid.h/cpp)**: Handles tilemap parsing, collision detection, and hazard simulation (Laser beam computation and Projectile physics).
- **Player Controller (Player.h/cpp)**: Manages the move queue and sprite animations.
- **UI Components (UI.h/cpp)**: Implements an interactive button class with smooth interpolation for visual feedback.
- **Configuration (Config.h)**: Centralized constants for grid size, cell dimensions, and window parameters.

## Prerequisites
- **C++ Compiler**: Support for C++17 or higher.
- **SFML 3.0**: The project utilizes the latest SFML 3 features (e.g., sf::degrees, is<sf::Event::Closed>, and revised FloatRect structures).
- **Assets**: Ensure the assets/ directory containing required .png and .ttf files is in the executable path.

## Installation and Build
Clone the Repository:
```bash
git clone https://github.com/yourusername/10-seconds-ahead.git
cd 10-seconds-ahead
```
Configure SFML: Ensure **SFML 3.0** is correctly installed and linked in your development environment (VS Code, Visual Studio, or CMake).
Compile:
- If using VS Code, use the integrated task runner configured for SFML.
- Ensure Config.h is included in the build path.
- Run: Execute the generated binary.

## Controls
- **WASD**: Enqueue movement directions.
- **B**: Place a defensive block at the projected end-of-path position.
- **K**: Undo the last action (Move or Block).
- **ESC**: Pause/Resume the game.

## Credits

Developed by **Devashish Mishra**
