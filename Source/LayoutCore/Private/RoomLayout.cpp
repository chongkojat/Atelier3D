#include "LayoutCore/RoomLayout.h"

#include <cmath>

namespace LayoutCore
{
	namespace
	{
		Vec2 ComputeCentroid(const std::vector<Vec2>& Polygon)
		{
			Vec2 Sum;
			if (Polygon.empty())
			{
				return Sum;
			}

			for (const Vec2& Point : Polygon)
			{
				Sum.X += Point.X;
				Sum.Y += Point.Y;
			}

			const double Count = static_cast<double>(Polygon.size());
			return Vec2{ Sum.X / Count, Sum.Y / Count };
		}

		// Shoelace formula.
		double ComputeArea(const std::vector<Vec2>& Polygon)
		{
			if (Polygon.size() < 3)
			{
				return 0.0;
			}

			double Area = 0.0;
			for (size_t Index = 0; Index < Polygon.size(); ++Index)
			{
				const Vec2& Current = Polygon[Index];
				const Vec2& Next = Polygon[(Index + 1) % Polygon.size()];
				Area += (Current.X * Next.Y) - (Next.X * Current.Y);
			}

			return std::fabs(Area) * 0.5;
		}

		const char* DensityLabel(EDensity Density)
		{
			switch (Density)
			{
			case EDensity::None: return "None";
			case EDensity::Low: return "Low";
			case EDensity::Medium: return "Medium";
			case EDensity::High: return "High";
			}
			return "Unknown";
		}

		const char* StyleLabel(EStyle Style)
		{
			switch (Style)
			{
			case EStyle::Japanese: return "Japanese";
			case EStyle::Scottish: return "Scottish";
			}
			return "Unknown";
		}
	}

	LayoutResult GenerateLayout(const RoomModel& Room, EDensity Density, EStyle Style)
	{
		LayoutResult Result;

		if (Density == EDensity::None || Room.Polygon.size() < 3)
		{
			Result.Breakdown.push_back(CriterionScore{ "FloorArea", 0.0 });
			return Result;
		}

		const Vec2 Centroid = ComputeCentroid(Room.Polygon);
		const double AreaSqCm = ComputeArea(Room.Polygon);
		const double AreaSqM = AreaSqCm / 10000.0;

		PlacedItem Anchor;
		Anchor.ItemId = std::string(StyleLabel(Style)) + "_SeatingGroup_" + DensityLabel(Density);
		Anchor.GroupRole = "Seating";
		Anchor.Position = Centroid;
		Anchor.RotationDegrees = 0.0;
		Result.Items.push_back(Anchor);

		// Placeholder scoring until Selection/Placement/Evaluation are real:
		// reward rooms with sane floor area, clamp to [0, 100].
		const double AreaScore = std::fmin(100.0, AreaSqM * 10.0);
		Result.Breakdown.push_back(CriterionScore{ "FloorArea", AreaScore });
		Result.TotalScore = AreaScore;

		return Result;
	}
}
