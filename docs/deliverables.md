# Software Architecture

## High-Level System Design
* **Frontend/UI (Unreal Engine UMG):** Handles user input for drawing room dimensions, slider interactions for density (None to High), and style selection dropdowns.
* **Procedural Generation System (C++ / Blueprints):** The core AI system that receives spatial bounds and user constraints to calculate valid furniture spawn locations and orientations.
* **Asset Management:** Categorized mesh libraries separated by style tags (e.g., Japanese, Scottish) and footprints.
* **Rendering Pipeline:** Unreal Engine 5's Lumen and Nanite systems for real-time visualization of the generated interior designs.