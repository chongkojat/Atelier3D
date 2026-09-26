# Atelier3D

**A procedural 3D interior design desktop application that furnishes a room for you.**
Built by team **SayReal** (GAM300).

The user draws the outline of a real room, marks its doors and windows, chooses a design style and how heavily furnished they want it, and Atelier3D generates several complete, navigable 3D interiors. Each one is laid out according to real interior-design rules, scored and explained, and editable. The result is not a picture: it is a lit 3D scene.

## The problem

Someone furnishing a room has to answer two questions: *what should go in here?* and *where should it go?* The second needs design expertise (clearances, circulation, focal points, grouping, visual balance) that most people have never been taught.

Floor-planning and configurator tools (Planner 5D, IKEA Kreativ, Coohom, SketchUp, Sweet Home 3D) give you an editable 3D scene, but you still place every object by hand. Nothing in common use generates a layout that is **automatic, spatially valid and editable** at the same time. Atelier3D fills that gap: the system supplies the expertise, and the output stays a real scene.

## Who it is for

| Users | What they get |
|---|---|
| **Non-designers furnishing a room** (primary) | Credible layout options for *their* room, judged by looking at them |
| **Design students and junior designers** | Many variants early in a project, with an explicit score that says why each one works |
| **Environment artists and level designers** | A content tool that populates interior spaces quickly |

## How it works

1. **Draw the room:** trace the room outline to scale and place doors and windows on the walls.
2. **Set intent:** choose a style (e.g. Japanese, Scottish) and a furnishing density (None / Low / Medium / High).
3. **Generate:** the system selects the furniture that belongs in a room of that size, type and density, places it and scores the result, in seconds.
4. **Compare:** several distinct variants are shown side by side and the user picks one.
5. **Understand:** the chosen layout shows its score broken down by criterion, so the user can see why it is arranged that way.

### Propose, then evaluate

The generator separates **proposing** a layout from **evaluating** one. Any proposer emits candidate layouts, and a single evaluator scores them. The design rules live in one place, and proposers can be added or replaced without touching anything else.

| Layer | Responsibility |
|---|---|
| Room model | Polygon, walls, and doors and windows as openings with clearance volumes |
| Furniture catalogue | Per-item metadata: footprint, height, category, anchor type, front vector, clearance volume, style tags, group role |
| Selection | Room type, area and density → which furniture groups and instances belong in the room |
| Placement | Candidate poses from anchor rules; invalid candidates rejected, best-scoring candidate kept |
| Evaluation | Hard constraints plus weighted soft criteria; returns a score and a per-criterion breakdown |

**Furniture is placed in groups, not as single objects.** A seating group is a sofa, a coffee table at the correct distance, flanking chairs and a rug sized to sit under them. Designers author each group's internal composition so it is always visually correct. The system decides which groups a room needs, at what density and where they anchor. This is how generated rooms look deliberately designed rather than randomly scattered.

**Evaluation criteria:**

- **Clearance:** no overlapping clearance volumes; at least 900 mm of circulation on primary paths
- **Circulation:** every functional zone reachable from every door
- **Alignment:** large items flush and parallel to walls where the style expects it
- **Grouping:** correct pairing and spacing within a group
- **Focal relationships:** seating oriented toward each other or toward a focal point
- **Balance:** visual mass distributed across the room
- **Light and view:** windows not blocked by tall items
- **Style coherence:** items consistent with the selected style

Hard constraints (overlaps, blocked doors, unreachable zones) reject a candidate outright. The other criteria are combined into a weighted score.

**The evaluator learns from users.** Each time a user chooses between variants, the system records every variant's criterion scores and which one was picked. These preference pairs are used offline to fit the criterion weights, so the system's idea of a good layout comes from what users actually prefer. This model is convex, trains in milliseconds on a small number of examples, needs no GPU, external dataset or inference runtime, and keeps the per-criterion breakdown shown to the user.

## Technology

| Concern | Choice |
|---|---|
| Language | C++20 |
| Engine | Custom engine (`Source/projects/Engine`): windowing, OpenGL rendering, scenes, input, assets |
| Editor UI | Dear ImGui |
| Build | CMake (Visual Studio on Windows, GCC/Ninja on Linux) |
| Tests and checks | GoogleTest, clang-format, clang-tidy, cppcheck |
| CI | Jenkins ([Jenkinsfile](Jenkinsfile)) |

## Supported platforms

| Platform | Status |
|---|---|
| Windows 10/11 (Visual Studio 2022 or 2026) | Supported (primary target) |
| Ubuntu 24.04+ (including WSL2) | Supported, used by CI |
| macOS | Not supported yet |

## Quick start

Full instructions, including prerequisites and troubleshooting: **[docs/setup.md](docs/setup.md)**

**Windows** (Visual Studio with the "Desktop development with C++" workload, run in Developer PowerShell for VS):
```powershell
git clone https://github.com/L1nceLin/Atelier3D.git
cd Atelier3D
tools\ci\build.bat Debug
build\ci-Debug\projects\Editor\Debug\GAM300Editor.exe
```

**Ubuntu:**
```bash
sudo apt install -y git build-essential cmake ninja-build libgl1-mesa-dev xorg-dev
git clone https://github.com/L1nceLin/Atelier3D.git
cd Atelier3D
bash tools/ci/build.sh Debug linux
./build/ci-Debug/projects/Editor/GAM300Editor
```

## Continuous integration

Jenkins builds every commit on `main`: Linux and Windows builds, unit tests (GoogleTest), linting (clang-format, clang-tidy) and static analysis (cppcheck). See [docs/setup.md](docs/setup.md#5-before-you-push) to run the same checks locally.

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

## Milestones

| Milestone | Week | Weight | Focus |
|---|---|---|---|
| M1 | Week 5 | 25% | Tool / pipeline MVP: architecture, build pipeline, data pipeline, usability |
| M2 | Week 10 | 25% | Prototype-capable tool: integration quality, documentation, testing |
| M3 | Week 13–14 | 30% | First app / product MVP: core experience, Semester 2 feasibility |

## Documentation

- [Development setup](docs/setup.md)
- [Proposal](docs/proposal.md)
- [Architecture](docs/architecture.md)
- [Deliverables](docs/deliverables.md)
