# Atelier3D (GAM300)

## Overview
A procedural 3D interior design desktop application built with Unreal Engine. Users can draw custom room dimensions, select furniture density via a slider (None, Low, Med, High), and choose stylistic themes (e.g., Japanese, Scottish). An AI system then procedurally generates custom room layouts and designs based on these parameters.

## Prerequisites
* Unreal Engine 5.x
* Visual Studio 2022 (with "Game development with C++" workload)
* Windows 11 / macOS Tahoe / Ubuntu 24.04 (depending on UE5 platform support)

## Setup Instructions
1. **Clone:** `git clone https://github.com/chongkojat/Atelier3D`
2. **Generate Project:** Right-click the `Atelier3D.uproject` file and select **Generate Visual Studio project files**.
3. **Build:** Open `Atelier3D.sln` in Visual Studio, set your configuration to *Development Editor*, and build the solution.
4. **Run:** Launch the project directly from the Epic Games Launcher or by pressing F5 in Visual Studio.
# Atelier3D

**A Scratch-inspired 3D programming and game development environment.**

Built by team **SayReal** (GAM300).

Atelier3D is a custom 3D engine and visual programming environment designed to help beginners learn how games and programming work by making code execution visible.

Inspired by the simplicity of Scratch, Atelier3D brings block-based programming into a real-time 3D environment. Users can create 3D scenes, attach logic to objects, run their programs and directly see how their blocks affect the world.

The core idea is:

**Code → Logic → Object → Execution → Result**

Rather than hiding program execution behind the engine, Atelier3D makes it visible.

---

## The problem

Learning programming can be difficult because program execution is mostly invisible.

A beginner may understand what a block says, but not necessarily understand:

- Which instruction is currently running
- Why a condition did or did not execute
- Which object is controlled by a piece of logic
- What caused a variable to change
- Why an object behaved differently from what they expected

Traditional block programming tools make syntax easier to learn, but the transition from blocks to traditional programming can still feel disconnected.

Professional game engines provide powerful 3D tools, but their complexity can make them difficult for beginners to understand.

Atelier3D sits between these two approaches.

It combines:

- A lightweight 3D editor
- Scratch-inspired visual programming
- A custom runtime
- Live execution visualisation
- Beginner-oriented debugging
- A gradual path towards C++

The goal is not to compete with professional game engines in rendering quality or production features. Atelier3D is designed as a learning environment that happens to produce small playable 3D games.

---

## Who it is for

| Users | What they get |
|---|---|
| **Beginners learning programming** | Learn programming concepts through visual blocks and immediate 3D feedback |
| **Students learning game development** | Build small 3D games while understanding how their logic executes |
| **Programming learners transitioning to C++** | Understand how visual blocks map to traditional programming concepts |
| **Educators** | A controlled 3D environment for teaching programming concepts |
| **Game development beginners** | Learn scenes, objects, events, variables, conditions and loops through practical projects |

---

## How it works

1. **Create the scene:** Build a 3D environment by creating objects, positioning them and editing their properties.

2. **Add logic:** Attach visual blocks to objects and build behaviour using events, variables, conditions and loops.

3. **Run:** Start the game directly inside the editor.

4. **See execution:** The currently executing block is highlighted while the corresponding object changes in the 3D world.

5. **Debug:** Pause execution, inspect variables and step through the program to understand what is happening.

6. **Learn:** Use structured lessons in Teaching Mode or freely experiment in Sandbox Mode.

The learner should always be able to answer three questions:

- **What is controlling this object?**
- **What is happening right now?**
- **What changed?**

---

## See the code. See the logic. See the result.

The central feature of Atelier3D is the connection between visual programming and the 3D world.

For example:

```text
[ When W is held ]
        |
        v
[ If Player is on ground ]
        |
        v
[ Move Player forward ]
        |
        v
[ Play footstep sound ]
```

When the scene is running, Atelier3D can show the execution state:

```text
[ When W is held ]          Waiting
        |
[ If Player is on ground ]  Completed
        |
[ Move Player forward ]     EXECUTING
        |
[ Play footstep sound ]     Waiting
```

At the same time, the Player object visibly moves in the 3D viewport.

This creates a direct relationship between:

```text
Input
  ↓
Event
  ↓
Block
  ↓
Object State
  ↓
3D Result
```

---

## Core Features

### 3D Scene Editor

Create and edit interactive 3D scenes directly inside Atelier3D.

- Create and delete objects
- Select objects
- Translate, rotate and scale objects
- Interactive transform gizmos
- Camera controls
- Lighting
- Materials
- Collision
- Basic physics
- Object hierarchy
- Object inspector
- Real-time 3D viewport

---

### Visual Block Programming

Build behaviour using drag-and-drop blocks instead of writing code.

The block system covers core programming concepts including:

- Events
- Movement
- Variables
- Conditions
- Loops
- Functions
- Object lifecycle
- Audio
- Visuals
- Game systems

Example:

```text
WHEN KEY W IS PRESSED
        |
        v
IF PLAYER IS ON GROUND
        |
        v
MOVE PLAYER FORWARD
```

Blocks are associated with objects so that users can understand which logic controls an object.

---

### Live Execution Visualisation

While a project is running, Atelier3D can communicate runtime information back to the editor.

This allows the editor to show:

- Currently executing block
- Waiting blocks
- Runtime events
- Variable values
- Object state
- Runtime errors
- Execution progress

The purpose is to make program execution visible rather than treating the program as a black box.

---

### In-World Debugger

Atelier3D provides debugging information directly alongside the 3D world.

A watched object can expose information such as:

```text
Player
--------------------
Position
X: 10.0
Y: 0.0
Z: 5.0

Health: 80
Speed: 5.0

State: Running
Event: W Held
Current Block:
Move Player Forward
```

This allows learners to connect a variable or execution state with the object they can see in the scene.

---

### Step Debugging

Users can pause a running project and inspect execution one block at a time.

Planned debugging features include:

- Pause
- Resume
- Step execution
- Breakpoints
- Variable inspection
- Runtime event inspection
- Beginner-friendly error messages

Instead of debugging low-level engine instructions, Atelier3D focuses on meaningful block-level execution.

---

## Teaching Mode

Teaching Mode provides structured programming challenges.

A lesson can contain:

1. A prepared 3D scene
2. A programming objective
3. A limited block palette
4. A programming workspace
5. Automatic validation
6. Optional hints

Example:

```text
Challenge:

Make the player move forward
when the W key is held.
```

The learner builds the behaviour using blocks and runs the scene to test their solution.

Teaching Mode is intended to progressively introduce:

```text
Events
   ↓
Sequence
   ↓
Variables
   ↓
Conditions
   ↓
Loops
   ↓
Functions
   ↓
Game Systems
```

---

## Sandbox Mode

Sandbox Mode allows users to experiment freely without lesson restrictions.

Users can:

- Create their own scenes
- Add objects
- Build custom block logic
- Import assets
- Experiment with game mechanics
- Debug their projects
- Create small playable games
- Export their projects

The same engine and editor are used for both Teaching Mode and Sandbox Mode.

---

## Block Execution Model

Blocks do not execute directly inside the editor.

Instead, the block graph is compiled into an intermediate representation which is executed by the runtime.

```text
Block Graph
     |
     v
Block Compiler
     |
     v
Intermediate Representation
     |
     v
Block Virtual Machine
     |
     +------> Runtime State
     |
     +------> Current Block
     |
     +------> Variables
     |
     +------> Events
     |
     +------> Errors
             |
             v
        Editor Debugger
```

Each instruction retains information about the block it originated from.

This allows the runtime to report the currently executing block back to the editor and enables:

- Execution highlighting
- Breakpoints
- Step debugging
- Runtime inspection

Long-running block handlers yield instead of freezing the editor, allowing the editor and runtime to remain responsive during execution.

---

## Block-to-C++ Learning Path

Atelier3D is designed to provide a gradual transition from visual programming to traditional programming.

The intended progression is:

```text
Beginner
   |
   v
Blocks
   |
   v
Blocks + Generated C++
   |
   v
C++ Extension Points
   |
   v
Engine / Graphics Programming
```

The C++ system is intended as a **learning bridge**, not an arbitrary C++-to-block converter.

The architecture uses a shared intermediate representation:

```text
                    +------------------+
                    |                  |
Block Graph ------> | Intermediate     | ------> Runtime VM
                    | Representation   |
                    |                  |
                    +--------+---------+
                             |
                             v
                    C++ Code Generator
                             |
                             v
                       C++ View
```

The initial MVP focuses on the block programming system and runtime. The generated C++ view is planned as a later feature.

---

## Propose, Execute, Debug

The system is designed around a clear separation between creating logic and executing logic.

| Layer | Responsibility |
|---|---|
| **Block Editor** | Create and edit visual programs |
| **Compiler** | Convert block graphs into an intermediate representation |
| **Runtime / VM** | Execute the intermediate representation |
| **Object System** | Store objects, components, variables and attached logic |
| **Event System** | Dispatch input, update, collision and other events |
| **Debug Bridge** | Send runtime state back to the editor |
| **Debugger** | Display execution, variables, events and errors |

This separation allows the editor to inspect the runtime without being responsible for executing the game logic itself.

---

## Custom 3D Engine

Atelier3D is built using a **custom 3D engine** rather than Unity, Unreal Engine, Godot or another complete game engine.

The engine is custom in the areas that are central to the project:

- Object system
- Scene system
- Runtime architecture
- Block execution model
- Editor
- Debugging system
- Project system
- Engine/editor communication

Established libraries are used for supporting functionality instead of reimplementing every low-level system.

---

## Editor

The Atelier3D editor is built around a custom 3D viewport and immediate-mode editor interface.

### Editor UI

The editor uses:

- **Dear ImGui**
- **ImGuizmo**
- Custom editor panels
- Custom 3D viewport
- Scene hierarchy
- Inspector
- Asset browser
- Block editor
- Debugging interface

Example editor layout:

```text
+----------------------------------------------------------+
| Atelier3D                                                |
+----------------+-------------------------+---------------+
| Scene          |                         | Inspector     |
|                |                         |               |
| Player         |       3D VIEWPORT       | Transform     |
| Floor          |                         | Material      |
| Enemy          |       Gizmos            | Physics       |
| Camera         |       Objects           | Variables     |
|                |       Debug Info        | Logic         |
+----------------+-------------------------+---------------+
| Asset Browser  | BLOCK EDITOR / DEBUGGER                 |
+----------------+-----------------------------------------+
```

---

## Architecture

Atelier3D is divided into three major layers.

```text
+------------------------------------------------------+
|                    EDITOR LAYER                     |
|------------------------------------------------------|
| Scene Editor | Block Editor | Inspector | Debugger  |
| Teaching     | Asset Browser| Gizmos    | C++ View* |
+---------------------------+--------------------------+
                            |
                     Commands / State
                            |
+---------------------------v--------------------------+
|                    RUNTIME LAYER                    |
|------------------------------------------------------|
| Object System | Event System | Block VM             |
| Renderer      | Physics      | Audio                |
| Asset Manager | Debug Bridge                       |
+---------------------------+--------------------------+
                            |
                       Load / Save
                            |
+---------------------------v--------------------------+
|                    PROJECT LAYER                    |
|------------------------------------------------------|
| Scenes | Block Graphs | Models | Textures | Audio   |
| Settings | Project Metadata                          |
+------------------------------------------------------+

* Planned feature
```

The editor and runtime operate together so that runtime information can be reported directly back to the editor.

This architecture is what enables live execution highlighting and in-world debugging.

---

## Technology

| Concern | Choice |
|---|---|
| Language | C++20 |
| Engine | Custom 3D Engine |
| Graphics | OpenGL |
| Editor UI | Dear ImGui |
| Gizmos | ImGuizmo |
| Window / Input | GLFW |
| Mathematics | GLM |
| Model Loading | tinyobjloader / glTF |
| Image Loading | stb_image |
| Audio | miniaudio |
| Project Format | JSON |
| Build | CMake |
| Tests | GoogleTest |
| Formatting | clang-format |
| Static Analysis | clang-tidy, cppcheck |
| CI | Jenkins |

---

## Supported Platforms

| Platform | Status |
|---|---|
| Windows 10/11 | Supported (primary target) |
| Ubuntu 24.04+ / WSL2 | Supported, used by CI |
| macOS | Not supported yet |

---

## Project Structure

```text
Atelier3D/
├── Source/
│   ├── Core/
│   ├── Engine/
│   ├── Renderer/
│   ├── Editor/
│   ├── Runtime/
│   ├── Scripting/
│   ├── Physics/
│   ├── Audio/
│   ├── Assets/
│   └── UI/
│
├── Assets/
│   ├── Models/
│   ├── Textures/
│   ├── Audio/
│   └── Shaders/
│
├── Tests/
│
├── tools/
│   └── ci/
│
├── docs/
│
├── CMakeLists.txt
└── README.md
```

---

## Quick Start

Full setup instructions, prerequisites and troubleshooting:

**[docs/setup.md](docs/setup.md)**

### Windows

Requirements:

- Visual Studio
- Desktop development with C++ workload
- CMake
- Git

Run from Developer PowerShell for Visual Studio:

```powershell
git clone https://github.com/L1nceLin/Atelier3D.git

cd Atelier3D

tools\ci\build.bat Debug

build\ci-Debug\projects\Editor\Debug\GAM300Editor.exe
```

### Ubuntu

Install the required dependencies:

```bash
sudo apt install -y git build-essential cmake ninja-build libgl1-mesa-dev xorg-dev
```

Clone and build:

```bash
git clone https://github.com/L1nceLin/Atelier3D.git

cd Atelier3D

bash tools/ci/build.sh Debug linux

./build/ci-Debug/projects/Editor/GAM300Editor
```

---

## Continuous Integration

Jenkins automatically checks commits on `main`.

The CI pipeline currently covers:

- Linux build
- Windows build
- Unit tests
- clang-format
- clang-tidy
- cppcheck

The Jenkins pipeline is defined in:

**[Jenkinsfile](Jenkinsfile)**

The same checks can be run locally using the scripts under:

```text
tools/ci/
```

For more information, see:

**[docs/setup.md](docs/setup.md)**

---

## MVP

The initial MVP focuses on the core learning experience.

### Engine

- [x] C++ engine foundation
- [x] CMake build system
- [ ] 3D renderer
- [ ] Scene management
- [ ] Object system
- [ ] Transform system
- [ ] Camera system
- [ ] Lighting
- [ ] Collision
- [ ] Basic physics
- [ ] Asset loading

### Editor

- [x] Dear ImGui foundation
- [ ] 3D viewport
- [ ] Transform gizmos
- [ ] Scene hierarchy
- [ ] Inspector
- [ ] Asset browser
- [ ] Project save/load

### Visual Programming

- [ ] Block palette
- [ ] Block canvas
- [ ] Connection validation
- [ ] Events
- [ ] Variables
- [ ] Conditions
- [ ] Loops
- [ ] Functions
- [ ] Object interaction
- [ ] Block compiler
- [ ] Runtime virtual machine

### Debugging

- [ ] Execution highlighting
- [ ] Runtime state tracking
- [ ] In-world debugger
- [ ] Pause / resume
- [ ] Block stepping
- [ ] Breakpoints
- [ ] Beginner-friendly errors

### Learning

- [ ] Teaching Mode
- [ ] Validated lessons
- [ ] Sandbox Mode
- [ ] Windows export

### Future

- [ ] Generated C++ view
- [ ] C++ extension points
- [ ] Reusable logic components
- [ ] Logic dependency visualisation
- [ ] Execution history
- [ ] Progressive hints
- [ ] Advanced materials and shaders
- [ ] Additional platforms

---

## Development Roadmap

| Phase | Activity | Deliverable |
|---|---|---|
| 1 | Requirements & system design | Architecture, block language and project format |
| 2 | Core engine & scene editor | 3D scene with objects, camera, lighting and gizmos |
| 3 | Object & asset pipeline | Asset import, inspector and project save/load |
| 4 | Block programming system | Block palette, canvas, rules and compilation |
| 5 | Runtime & execution visualisation | Running games with block execution highlighting |
| 6 | Debugging & inspection | Step execution, breakpoints and readable errors |
| 7 | Teaching Mode | Structured validated lessons |
| 8 | Sandbox Mode & export | Free project creation and Windows export |
| 9 | C++ View | Generated C++ representation |
| 10 | Testing & optimisation | Final testing, usability trials and documentation |

The C++ view is treated as a conditional later-stage feature so that the core engine, block system and runtime remain the priority.

---

## Team SayReal

| Member | Role |
|---|---|
| Ko Jat Chong | Tech Lead |
| Sheng Lin | Product Manager |
| Kye Le Woh | Design Lead |
| Hong Chieh Jethro Sung | Design Champion |
| Julius Junshen Cai | Engine Lead |
| Shi Wei Beh | Engine Champion |
| Thant Win Kaung | AI Lead |
| San Ni Go | AI Champion |
| Henrey Hu | Programmer |

---

## Milestones

| Milestone | Week | Weight | Focus |
|---|---:|---:|---|
| M1 | Week 5 | 25% | Tool / pipeline MVP: architecture, build pipeline, data pipeline and usability |
| M2 | Week 10 | 25% | Prototype-capable tool: integration quality, documentation and testing |
| M3 | Week 13–14 | 30% | First app / product MVP: core experience and Semester 2 feasibility |

---

## Project Scope

Atelier3D is intentionally focused on the core learning experience.

The initial project does **not** focus on:

- Online services
- User accounts
- Monetisation
- Asset marketplaces
- Multiplayer
- Classroom management
- Learning analytics
- AI tutoring
- Large-scale commercial game development
- High-end production rendering

These may be considered future extensions but are outside the core MVP.

---

## Design Philosophy

### Make execution visible

A program should not feel like a black box.

### Connect logic to objects

Users should be able to understand which logic controls an object in the scene.

### Learn progressively

The system should introduce programming concepts gradually.

### Debug by understanding

Errors should help learners understand what went wrong rather than simply exposing technical engine errors.

### Build real things

Programming concepts should be learned through creating interactive 3D projects.

### Keep the engine understandable

The engine should remain small enough that its architecture can be understood by the people learning from it.

---

## What Atelier3D Is Not

Atelier3D is **not intended to replace Unity, Unreal Engine or other professional game engines.**

It is not designed for:

- AAA game development
- Large open-world games
- Advanced multiplayer infrastructure
- Console development
- Production-scale rendering
- Large studio pipelines

Instead, Atelier3D focuses on providing a small and understandable 3D environment where beginners can learn programming and game development.

---

## Vision

Atelier3D aims to make programming concepts visible.

Instead of:

```text
Code
  ↓
???
  ↓
Result
```

the learner should see:

```text
Code
  ↓
Logic
  ↓
Execution
  ↓
Object State
  ↓
3D Result
```

A learner should be able to start with simple blocks, understand how programs execute, transition toward C++, and eventually understand the engine and graphics systems underneath the tools they are using.

> **See the code. See the logic. See the result.**

---

## Documentation

- [Development Setup](docs/setup.md)
- [Proposal](docs/proposal.md)
- [Architecture](docs/architecture.md)
- [Deliverables](docs/deliverables.md)