# SayReal: Project Proposal

**Team:** SayReal
**Product:** Atelier3D: a procedural 3D interior design desktop application

## Concept

Atelier3D is a desktop application that furnishes a room for you.

The user draws the outline of a real room, marks its doors and windows, chooses a design style and how heavily furnished they want it, and the application generates several complete, navigable 3D interiors, each one laid out according to real interior-design rules, each one scored and explained, each one editable.

The generated result is not a picture. It is a lit 3D scene.

## The problem

Someone about to furnish a room has to answer two different questions: "what should go in here?", and "where should it go?". The second question is the one they can't answer, because it is domain expertise; clearances, circulation, focal points, grouping, visual balance, that interior designers train for and most people have never been taught.

The tools available today:

**Floor-planning and configurator tools** (Planner 5D, IKEA Kreativ, Coohom, SketchUp, Sweet Home 3D) give an editable 3D scene, but the user still places every object by hand. The tool provides the canvas; the expertise is still missing.

Nothing in common use generates a **layout that is simultaneously automatic, spatially valid, and editable**.

That gap is what Atelier3D targets: the expertise is supplied by the system, and the output stays a real scene.

## Target users

**Primary: The Non-Designer furnishing a room.** Renters and homeowners with a room to fill, a rough budget, no design training, and no confidence about arrangement. They want to see credible options for *their* room rather than inspiration for someone else's, and they want to judge those options by looking at them.

**Secondary: Design students and junior designers.** People who can evaluate a layout but want option volume early in a project. For them the value is the speed of generating variants to react to, and the explicit scoring that says "why" a layout works.

**Tertiary: Environment artists and level designers.** The same generator, used as a content tool to populate interior spaces quickly.

## Core user journey

1. **Draw the room.** The user traces the outline of their room to scale and places doors and windows on the walls.
2. **Set intent.** They choose a style (e.g. Japanese, Scottish) and a furnishing density (None / Low / Medium / High).
3. **Generate.** The system selects what furniture belongs in a room of that size, type and density, places it, and scores the result. This runs in seconds.
4. **Compare.** Several distinct variants are presented side by side. The user picks the one they prefer.
5. **Understand.** The chosen layout shows its score broken down by criterion: clearance, circulation, alignment, grouping, balance; so the user can see why it is arranged that way.

## Technical approach

### Architecture: propose, then evaluate

The system separates **proposing** a layout from **evaluating** one. Any proposer emits candidate layouts; a single evaluator scores them. This keeps the design rules in one place, allows proposers to be added or replaced without touching anything else, and means a more ambitious proposer can be introduced later without risking the working one.

Five layers:

| Layer | Responsibility |
|-------|----------------|
| Room model | Polygon, walls, doors and windows as openings with clearance volumes |
| Furniture catalogue | Per-item metadata: footprint, height, category, anchor type, front vector, clearance volume, style tags, group role |
| Selection | Room type, area and density -> which furniture groups and instances belong in this room |
| Placement | Candidate poses from anchor rules, invalid candidates rejected, best-scoring candidate taken |
| Evaluation | Hard constraints plus weighted soft criteria; returns a score and a per-criterion breakdown |

### Composition by groups

Furniture is selected and placed in **groups**, not as individual objects. A seating group is a sofa, a coffee table at a correct distance, flanking chairs and a rug sized to sit under them.

The internal composition of each group is authored by a designer, so it is always visually correct. The system chooses which groups a room needs, at what density, and where they anchor.

This is the central design decision of the project: it is how generated output can look deliberately designed rather than randomly scattered, while remaining genuinely generated.

### Evaluation criteria

Layouts are scored against interior-design guidelines drawn from published work on automated furniture layout (Merrell et al. 2011; Yu et al. 2011; Kán & Kaufmann 2018) and from composition principles:

- **Clearance** : no overlapping clearance volumes; circulation gaps at or above 900 mm on primary paths
- **Circulation** : every functional zone reachable from every door without obstruction
- **Alignment** : large items flush and parallel to walls where the style expects it
- **Grouping** : correct pairing and spacing within a group
- **Focal relationships** : seating oriented toward each other or toward a focal point
- **Balance** : visual mass distributed across the room
- **Light and view** : windows not blocked by tall items
- **Style coherence** : items consistent with the selected style

Hard constraints (overlap, blocked doors, unreachable zones) reject a candidate outright. The remaining criteria are combined into a weighted score.

### The learning component

The evaluator's weights are not hand-tuned indefinitely. Each time a user chooses among variants, the system records the feature vector of every variant shown and which one was chosen. These preference pairs are used to fit the criterion weights offline, so the system's notion of a good layout is learned from what users actually prefer rather than asserted by the team.

This approach -> learning the evaluator rather than training a layout generator was selected deliberately. It is convex, trains in milliseconds on a small number of examples, needs no GPU, no external dataset, and no third-party inference runtime, and it preserves the per-criterion breakdown that shows the user. A learned *generator* would forfeit all of those properties and is out of scope.

<LayoutCore> has no Unreal dependencies and compiles standalone, so it can be unit-tested and exercised by a headless batch runner in CI without an engine build. It lives inside the Unreal project from the start, so integration is continuous rather than deferred.

### Why Unreal Engine

The product's credibility depends on the generated interiors looking realistic. A correct layout rendered poorly does not demonstrate the value of the system. Unreal Engine 5 provides Lumen global illumination and Nanite geometry out of the box, along with an established asset ecosystem, which a custom engine could not match within the project timeline.

**Stack:**

| Concern | Choice |
|---------|--------|
| Engine | Unreal Engine 5.8.2 |
| Language | C++ (generator and integration), Blueprints (UI wiring) |
| Room geometry | Geometry Script - procedural walls and openings from the drawn outline |
| Decorative scatter | PCG - surface clutter only, not layout |
| Asset import | Interchange Framework |
| Rendering | Lumen, Nanite; Movie Render Queue for presentation capture |
| UI | UMG |
| Target platform | Windows desktop |

## Risks

| Risk | Impact | Mitigation |
|------|--------|------------|
| Catalogue metadata authoring is underestimated | Blocks all generation work | Schema frozen in Week 3; placeholder catalogue so generator work never waits on assets |
| Nine people blocked on one interface | Lost weeks | Catalogue schema published early as the contract between the generator and the asset pipeline |
| Mixed asset sources look amateur | Inconsistent scale and material response | Build the first catalogue from one coherent set |

## Team

| Member | Role |
|--------|------|
| Ko Jat Chong | Tech Lead |
| Sheng Lin | Product Manager |
| Kye Le Woh | Design Lead |
| Hong Chieh Jethro Sung | Design Champion |
| Julius Junshen Cai | Engine Lead |
| Shi Wei Beh | Engine Champion |
| Thant Win Kaung | AI Lead |
| San Ni Go | AI Champion |
| Henrey Hu | Programmer |

## Schedule

| Milestone | Week | Weight | Focus |
|-----------|------|--------|-------|
| M1 | Week 5 | 25% | Tool / pipeline MVP - architecture, build pipeline, data pipeline, usability |
| M2 | Week 10 | 25% | Prototype-capable tool - integration quality, documentation, testing |
| M3 | Week 13-14 | 30% | First app / product MVP - core experience, Semester 2 feasibility |