# Software Architecture

## Guiding principle: propose, then evaluate

The system separates **proposing** a layout from **evaluating** one. Any proposer emits candidate layouts; a single evaluator scores them against a fixed set of design rules. This keeps the rules in one place, lets proposers be added or replaced without touching evaluation, and means a more ambitious proposer can be introduced later without risking the one already working.

A single generation request runs this pass repeatedly under different random seeds, keeping the three most distinct scored results as the variants shown to the user. The same seed with the same inputs always reproduces the same layout, so a run is fully repeatable across machines.

## System layers

| Layer | Responsibility |
|-------|----------------|
| Room model | Polygon, walls, doors and windows as openings, each with a clearance volume |
| Furniture catalogue | Per-item metadata: footprint, height, category, anchor type, front vector, clearance volume, style tags, group role |
| Selection | Given room type, area and density, decides which furniture groups and instances belong in the room |
| Placement | Generates candidate poses from anchor rules, rejects invalid candidates, keeps the best-scoring one |
| Evaluation | Applies hard constraints plus weighted soft criteria; returns a score and a per-criterion breakdown |

Data flows one way through these layers per generation pass: room model + selection output feed placement, placement output feeds evaluation, and evaluation results are what the UI compares across variants.

The five layers above are the conceptual model; the Product Design Document's component-ownership table is the authoritative breakdown of concrete pieces (Solver Bridge, Wall Builder, Actor Spawner, Occupancy Grid, Preference Logger, Weight Fitter, Catalogue Authoring, Headless Batch Runner, and so on) and who owns each.

## Module boundaries

Per the Product Design Document, `LayoutCore` ships as a plugin, not a module of the game project directly, split into three:

| Module | Contents |
|--------|----------|
| `Plugins/LayoutTool/Source/LayoutCore/` | No Unreal dependencies. Compiles standalone; unit-tested in CI |
| `Plugins/LayoutTool/Source/LayoutRuntime/` | Unreal integration — data assets, spawning, UI binding |
| `Plugins/LayoutTool/Source/LayoutEditor/` | Authoring and debug tooling (editor-only) |

This is pending confirmation with the instructor (open question Q4 in the Product Design Document) and does not yet match the current source layout, which lives directly under the game project's `Source/` rather than as a plugin.

## High-level system design

* **Frontend/UI (Unreal Engine UMG):** room-drawing input (outline, doors, windows), the density slider (None/Low/Medium/High), style selection, and the side-by-side variant comparison + score breakdown view. Density is a discrete label in the UI, but internally it means a target furnishing ratio — total furniture plan area divided by room area — not an item or group count.
* **`<LayoutCore>` (C++, no Unreal dependencies):** implements the Room model, Selection, Placement and Evaluation layers above. Compiles standalone and is unit-tested and exercised by a headless batch runner in CI without an engine build, while still living inside the Unreal project from the start so integration stays continuous rather than deferred.
* **Procedural Generation System (C++ / Blueprints):** the Unreal-side integration layer — receives spatial bounds and user constraints from the UI, drives `<LayoutCore>`, and translates its output (poses per item) into actors in the level.
* **Asset Management:** categorized mesh libraries separated by style tag (e.g. Japanese, Scottish) and footprint, imported via the Interchange Framework and indexed by the furniture catalogue.
* **Room geometry:** built procedurally from the drawn outline using Geometry Script — walls and openings are generated, not hand-placed.
* **Decorative scatter (PCG):** surface clutter placed after layout is finalized; PCG is not used for layout decisions, only dressing.
* **Rendering Pipeline:** Unreal Engine 5's Lumen and Nanite for real-time visualization, with Movie Render Queue used for presentation capture.

## Composition by groups

Furniture is selected and placed in **groups**, not as individual objects — a seating group is a sofa, a coffee table at a correct distance, flanking chairs, and a rug sized to sit under them. The internal composition of each group is authored by a designer, so it is always visually correct; the Selection and Placement layers only decide which groups a room needs, at what density, and where each group anchors. This is what lets generated output look deliberately designed rather than randomly scattered while remaining genuinely generated.

## Evaluation criteria

Hard constraints (overlapping clearance volumes, blocked doors, unreachable zones) reject a candidate outright. Remaining candidates are scored on weighted soft criteria:

* **Clearance** — no overlapping clearance volumes; circulation gaps at or above 900 mm on primary paths, 600 mm on secondary paths
* **Circulation** — path directness and absence of pinch points between doors and functional zones
* **Alignment** — large items flush and parallel to walls where the style expects it
* **Grouping** — correct pairing and spacing within a group
* **Focal relationships** — seating oriented toward each other or toward a focal point
* **Balance** — visual mass distributed across the room
* **Light and view** — windows not blocked by tall items
* **Style coherence** — items consistent with the selected style

Criteria are drawn from published work on automated furniture layout (Merrell et al. 2011; Yu et al. 2011; Kán & Kaufmann 2018) and from standard composition principles.

## Data model

All dimensions are integer millimetres; the room origin is the outline's first vertex, Z up. The furniture catalogue and group templates are authored as JSON under version control and imported to Unreal data assets at build time — a catalogue entry is per-item metadata (id, category, footprint, height, anchor type, front vector, per-side clearance, style tags, which group roles it can fill); a group template is a named set of role "slots" (required or optional, with a relative pose and tolerance) that the Selector fills with catalogue items rather than a fixed, pre-baked composition. See the Product Design Document's data model section for the full field-level schema.

## Learning component

Evaluator weights are not fixed indefinitely by hand-tuning. Each time a user picks a preferred variant, the feature vector of every variant shown and which one was chosen is recorded. These preference pairs are used to fit the criterion weights offline. This problem is convex, trains in milliseconds on a small number of examples, needs no GPU, no external dataset, and no third-party inference runtime, and it preserves the per-criterion breakdown shown to the user — properties a learned *generator* would not have, which is why layout generation itself stays rule-based and only the evaluator's weights are learned.

## Stack

| Concern | Choice |
|---------|--------|
| Engine | Unreal Engine 5.8.2 |
| Language | C++ (generator and integration), Blueprints (UI wiring) |
| Room geometry | Geometry Script — procedural walls and openings from the drawn outline |
| Decorative scatter | PCG — surface clutter only, not layout |
| Asset import | Interchange Framework |
| Rendering | Lumen, Nanite; Movie Render Queue for presentation capture |
| UI | UMG |
| Build | Unreal Build Tool; CMake for the standalone core, so it builds and tests in CI without an engine build |
| Target platform | Windows desktop |
