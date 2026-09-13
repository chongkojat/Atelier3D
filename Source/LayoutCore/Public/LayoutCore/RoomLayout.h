#pragma once

// Standard C++ only, deliberately: no Unreal *types* here.
// This is the "propose, then evaluate" core described in docs/architecture.md.
// It must stay engine-independent so it can be unit-tested and run headless in CI.
//
// The one concession is LAYOUTCORE_API below: when built as a UE module, UBT
// force-defines LAYOUTCORE_API as DLLEXPORT/DLLIMPORT so Atelier3D can call across
// the module (DLL) boundary on Windows -- but those two tokens are themselves
// defined in HAL/Platform.h, which we only pull in when __UNREAL__ (also
// force-defined by UBT) says we're actually building inside Unreal.
#if defined(__UNREAL__)
	#include "HAL/Platform.h"
#elif !defined(LAYOUTCORE_API)
	#define LAYOUTCORE_API
#endif

#include <string>
#include <vector>

namespace LayoutCore
{
	struct Vec2
	{
		double X = 0.0;
		double Y = 0.0;
	};

	struct Opening
	{
		Vec2 Position;
		double Width = 0.0;
		bool bIsDoor = true;
	};

	struct RoomModel
	{
		std::vector<Vec2> Polygon; // room outline, in order
		std::vector<Opening> Openings;
	};

	enum class EDensity
	{
		None,
		Low,
		Medium,
		High
	};

	enum class EStyle
	{
		Japanese,
		Scottish
	};

	struct PlacedItem
	{
		std::string ItemId;
		std::string GroupRole;
		Vec2 Position;
		double RotationDegrees = 0.0;
	};

	struct CriterionScore
	{
		std::string Name;
		double Value = 0.0;
	};

	struct LayoutResult
	{
		std::vector<PlacedItem> Items;
		double TotalScore = 0.0;
		std::vector<CriterionScore> Breakdown;
	};

	// Selection + Placement + Evaluation, single pass. Currently a placeholder:
	// it anchors one item at the room centroid and scores by floor area so the
	// pipeline is exercised end to end. Real Selection/Placement/Evaluation land here.
	// LAYOUTCORE_API is a plain preprocessor define UBT supplies on the command
	// line (dllexport/dllimport) -- it doesn't pull in any Unreal headers.
	LAYOUTCORE_API LayoutResult GenerateLayout(const RoomModel& Room, EDensity Density, EStyle Style);
}
