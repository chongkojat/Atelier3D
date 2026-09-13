#pragma once

// Placeholder furniture catalogue: hand-authored, stand-in for the real
// catalogue data (the "Furniture catalogue" layer in docs/architecture.md)
// until the asset pipeline lands. Same engine-independence rules as
// RoomLayout.h -- see that file for why.

#include "LayoutCore/RoomLayout.h"

#include <string>
#include <vector>

namespace LayoutCore
{
	// A single piece of furniture, positioned relative to its group's anchor.
	// LocalOffset/LocalRotationDegrees are authored assuming the group's anchor
	// sits at the origin facing local +Y (into the room, away from the wall it's
	// placed against); Placement rotates/translates the whole group into world
	// space as one unit.
	struct FurnitureItem
	{
		std::string Id;
		Vec2 LocalOffset;
		double LocalRotationDegrees = 0.0;
		double FootprintWidth = 0.0;  // cm, along the item's own local X ("right")
		double FootprintDepth = 0.0;  // cm, along the item's own local Y ("forward")
		double ClearanceMargin = 0.0; // cm, added around the footprint for overlap checks
	};

	// A designer-authored composition of items -- see "Composition by groups" in
	// docs/architecture.md. Width/Depth describe the group's own footprint
	// envelope (along the wall / into the room) for Placement's fit checks;
	// they are not derived from the member items automatically.
	struct FurnitureGroup
	{
		std::string GroupId;
		std::string GroupRole;         // "Seating", "Dining", "Storage", ...
		std::vector<EStyle> StyleTags; // empty = fits any style
		double Width = 0.0;  // cm, footprint along the wall it anchors to
		double Depth = 0.0;  // cm, footprint from the wall into the room
		std::vector<FurnitureItem> Items;
	};

	// Small hand-authored placeholder set standing in for the real catalogue.
	const std::vector<FurnitureGroup>& GetPlaceholderCatalogue();
}
