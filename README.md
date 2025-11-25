# 10 Seconds Ahead

## Turn-Based Path Planning Puzzle Game

**10 Seconds Ahead** is a strategy puzzle game where players plan their movement sequence during a 10-second planning phase. Once the timer ends, all moves automatically execute. Players must collect all treasure chests while navigating obstacles and using temporary blocks to alter the environment.

---

## Developer Info
**Developer:** Devashish Mishra  
**Role:** Lead Programmer & Game Designer  
**Language:** C++  
**Framework:** SFML (Simple and Fast Multimedia Library)

---

## Tech Stack
- C++  
- SFML (Graphics, Window, System modules)  
- Visual Studio Code  
- MinGW (g++)  
- Custom pixel-art assets (Grass, Tree, Water, Chest, Block)

---

## Gameplay Overview

### Planning Phase (10 seconds)
- Use **W, A, S, D** to queue movement commands.
- A yellow ghost trail shows the predicted movement path.
- Press **K** to undo the last planned move or remove the last placed block.
- Press **B** to place a **temporary block** (limited count).
- Blocks disappear automatically at the start of the next turn.
- The ghost preview updates after every input.

### Execution Phase
- All planned moves are executed automatically.
- Chests are collected instantly upon stepping onto them.
- Trees and water tiles are unwalkable.
- Blocks temporarily modify the map layout.
- After all moves finish, the next planning phase begins.

---

## Map System

The game uses a 20×20 grid. Supported map symbols:

| Symbol | Meaning |
|--------|---------|
| `T` | Tree (unwalkable) |
| `~` | Water (unwalkable) |
| `I` | Chest (collectible) |
| `B` | Temporary block |
| `P` | Player start |
| `.` | Grass tile |

Upcoming hazard features (in development):
- `C>` / `C<` – Horizontal cannons
- `Lv` / `L^` – Vertical laser emitters

---

## Current Features
- Smooth grid rendering  
- 10-second planning and automatic execution cycles  
- Undo system for both moves and blocks  
- Temporary block placement system  
- Chest collection and tracking  
- Ghost path preview (yellow for safe, red for blocked)  
- Letterboxed rendering for proper aspect ratio  
- HUD showing timer, blocks remaining, and control tips  

---

## Upcoming Features
- Complete cannon and laser hazard system  
- Laser beam collision detection (insta-fail or restart)  
- Level-complete screen after all items are collected  
- Start menu and settings menu  
- Multiple levels (target: 2-level mini campaign)

---

## How to Build & Run

### Requirements
- SFML 2.6+
- Include and library paths configured in compiler:
  - `-I C:/SFML/include`
  - `-L C:/SFML/lib`

### Build Command

```bash
g++ -g src/main.cpp src/Game.cpp src/Grid.cpp src/Player.cpp -o 10SecondsAhead.exe ^
-I C:/SFML/include -L C:/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -mwindows
```

## Assets

| File        | Description         |
|-------------|----------------------|
| **Grass.png** | Base tile           |
| **Tree.png**  | Tree obstacle       |
| **Water.png** | Water obstacle      |
| **Chest.png** | Collectible         |
| **Block.png** | Temporary barrier   |

---

## Current Progress

| Feature                     | Status |
|-----------------------------|:------:|
| Movement + ghost preview    | ✅     |
| Item collection             | ✅     |
| Temporary block system      | ✅     |
| Undo system                 | ✅     |
| Planning/execution cycle    | ✅     |
| Cannons & lasers            | 🚧 In progress |
| Level completion logic      | 🚧 Next |

---

## 🏁 Credits

Developed by **Devashish Mishra**
