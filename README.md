# 🎮 10 Seconds Ahead

### 🧩 Turn-Based Path Planning Puzzle Game

**10 Seconds Ahead** is a strategy-based puzzle game where you plan your moves 10 seconds ahead — then your actions execute automatically. Think fast, plan smart, and collect all treasure chests while navigating obstacles and using temporary blocks to alter the map.

---

## 🧑‍💻 Developer Info
**Developer:** Devashish Mishra  
**Role:** Lead Programmer & Game Designer  
**Language:** C++  
**Framework:** SFML (Simple and Fast Multimedia Library)

---

## ⚙️ Tech Stack
- **Language:** C++  
- **Framework:** SFML (Graphics, Window, System modules)  
- **IDE:** Visual Studio Code  
- **Compiler:** MinGW (g++)  
- **Assets:** Custom pixel art (Grass, Tree, Water, Chest, Block)

---

## 🎯 Gameplay Overview

### ⏳ Planning Phase (10 seconds)
- Use **W, A, S, D** to queue movement commands.
- A **yellow ghost trail** shows your future path.
- Press **K** to undo last move or undo last placed block.
- Press **B** to place a **temporary block** (limited count).
- Blocks vanish automatically at the start of the next turn.
- Ghost preview updates live to visualize your route.

### 🚀 Execution Phase
- Planned moves automatically execute.
- Chests are collected instantly when stepped on.
- Trees 🌳 and Water 🌊 tiles are unwalkable.
- Player-placed blocks temporarily alter the map.
- After execution finishes, you enter planning mode again.

---

## 🗺️ Map System
The grid is **20×20**, supporting:

| Symbol | Meaning |
|--------|---------|
| `T` | Tree (unwalkable) |
| `~` | Water (unwalkable) |
| `I` | Chest (collectible) |
| `B` | Temporary block |
| `P` | Player start |
| `.` | Grass tile |

Future hazard support (coming soon):
- `C>` / `C<` → Cannons (horizontal shooters)
- `Lv` / `L^` → Vertical lasers

---

## 🧱 Current Features
- Smooth grid rendering  
- 10-second planning phase with live countdown  
- Undo system for moves **and** blocks  
- Dynamic block placement + clear on turn reset  
- Chest collection system  
- Real-time ghost path preview (yellow/red for blocked)  
- Proper aspect-ratio letterboxing  
- HUD for timer, blocks left, controls tooltip  

---

## 🚧 Upcoming Features
- Full cannon & laser hazard mechanics  
- Laser beam cast + player hit detection  
- “Level Complete” screen after all chests collected  
- Start Menu + Settings  
- Level selection  
- 2-level campaign  

---

## 🧠 How to Build & Run

### Requirements
- SFML 2.6+
- Configure include and lib paths:
- I C:/SFML/include
- L C:/SFML/lib


### Build Command
```bash
g++ -g src/main.cpp src/Game.cpp src/Grid.cpp src/Player.cpp -o 10SecondsAhead.exe ^
-I C:/SFML/include -L C:/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -mwindows
```

## 🖼️ Assets

| File        | Description         |
|-------------|----------------------|
| **Grass.png** | Base tile           |
| **Tree.png**  | Tree obstacle       |
| **Water.png** | Water obstacle      |
| **Chest.png** | Collectible         |
| **Block.png** | Temporary barrier   |

---

## 📅 Current Progress

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
