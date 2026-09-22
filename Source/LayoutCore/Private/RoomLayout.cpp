#include "LayoutCore/RoomLayout.h"
#include "LayoutCore/FurnitureCatalogue.h"

#include <algorithm>
#include <cmath>

namespace LayoutCore
{
	namespace
	{
		constexpr double kPi = 3.14159265358979323846;

		Vec2 Subtract(const Vec2& A, const Vec2& B) { return Vec2{ A.X - B.X, A.Y - B.Y }; }
		Vec2 Add(const Vec2& A, const Vec2& B) { return Vec2{ A.X + B.X, A.Y + B.Y }; }
		Vec2 Scale(const Vec2& A, double S) { return Vec2{ A.X * S, A.Y * S }; }
		double Dot(const Vec2& A, const Vec2& B) { return A.X * B.X + A.Y * B.Y; }
		double Length(const Vec2& A) { return std::sqrt(Dot(A, A)); }

		Vec2 Normalize(const Vec2& A)
		{
			const double Len = Length(A);
			return Len > 1e-6 ? Vec2{ A.X / Len, A.Y / Len } : Vec2{ 0.0, 0.0 };
		}

		double AngleOfVector(const Vec2& V)
		{
			return std::atan2(V.Y, V.X) * 180.0 / kPi;
		}

		// Forward = the object's own facing direction, at world angle Degrees.
		Vec2 ForwardAxisFromAngle(double Degrees)
		{
			const double Rad = Degrees * kPi / 180.0;
			return Vec2{ std::cos(Rad), std::sin(Rad) };
		}

		// Perpendicular ("right") axis for an object whose forward is at Degrees.
		Vec2 RightAxisFromAngle(double Degrees)
		{
			const Vec2 Forward = ForwardAxisFromAngle(Degrees);
			return Vec2{ Forward.Y, -Forward.X };
		}

		Vec2 ComputeCentroid(const std::vector<Vec2>& Polygon)
		{
			Vec2 Sum;
			for (const Vec2& Point : Polygon)
			{
				Sum.X += Point.X;
				Sum.Y += Point.Y;
			}
			const double Count = static_cast<double>(Polygon.size());
			return Count > 0.0 ? Vec2{ Sum.X / Count, Sum.Y / Count } : Sum;
		}

		// Signed shoelace area: positive for CCW winding, negative for CW.
		double ComputeSignedArea(const std::vector<Vec2>& Polygon)
		{
			double Area = 0.0;
			for (size_t Index = 0; Index < Polygon.size(); ++Index)
			{
				const Vec2& Current = Polygon[Index];
				const Vec2& Next = Polygon[(Index + 1) % Polygon.size()];
				Area += (Current.X * Next.Y) - (Next.X * Current.Y);
			}
			return Area * 0.5;
		}

		struct Wall
		{
			Vec2 Midpoint;
			Vec2 InwardNormal;
			double Length = 0.0;
		};

		std::vector<Wall> BuildWalls(const std::vector<Vec2>& Polygon, bool bCCW)
		{
			std::vector<Wall> Walls;
			Walls.reserve(Polygon.size());

			for (size_t Index = 0; Index < Polygon.size(); ++Index)
			{
				const Vec2& A = Polygon[Index];
				const Vec2& B = Polygon[(Index + 1) % Polygon.size()];
				const Vec2 Edge = Subtract(B, A);
				const double EdgeLength = Length(Edge);
				if (EdgeLength < 1e-3)
				{
					continue;
				}

				// Interior is to the left of travel for a CCW polygon, to the right for CW.
				const Vec2 Inward = bCCW ? Normalize(Vec2{ -Edge.Y, Edge.X }) : Normalize(Vec2{ Edge.Y, -Edge.X });

				Wall W;
				W.Midpoint = Scale(Add(A, B), 0.5);
				W.InwardNormal = Inward;
				W.Length = EdgeLength;
				Walls.push_back(W);
			}

			// Longest wall first: prefer anchoring the more prominent groups there.
			std::sort(Walls.begin(), Walls.end(), [](const Wall& A, const Wall& B) { return A.Length > B.Length; });
			return Walls;
		}

		// Rough interior extent perpendicular to a wall: distance from the wall's
		// midpoint to the room centroid, doubled. Exact for rectangles; an
		// approximation for anything else, good enough until Room model gets
		// real per-wall depth queries.
		double EstimateRoomDepthFromWall(const Wall& CandidateWall, const Vec2& Centroid)
		{
			return std::fabs(Dot(Subtract(Centroid, CandidateWall.Midpoint), CandidateWall.InwardNormal)) * 2.0;
		}

		bool GroupFitsOnWall(const FurnitureGroup& Group, const Wall& CandidateWall, double RoomInteriorDepth)
		{
			constexpr double kWallMargin = 20.0; // cm of breathing room at each wall end
			return CandidateWall.Length >= (Group.Width + kWallMargin) && RoomInteriorDepth >= Group.Depth;
		}

		// A furniture item's clearance volume, projected to 2D: an oriented box
		// (footprint + clearance margin, at the item's own world rotation).
		struct OrientedBox
		{
			Vec2 Center;
			double HalfWidth = 0.0;  // along the item's own "right" axis
			double HalfDepth = 0.0;  // along the item's own "forward" axis
			double RotationDegrees = 0.0;
			int GroupIndex = -1; // which selected group this item belongs to
		};

		double ProjectedExtent(const OrientedBox& Box, const Vec2& Axis)
		{
			const Vec2 Right = RightAxisFromAngle(Box.RotationDegrees);
			const Vec2 Forward = ForwardAxisFromAngle(Box.RotationDegrees);
			return std::fabs(Dot(Right, Axis)) * Box.HalfWidth + std::fabs(Dot(Forward, Axis)) * Box.HalfDepth;
		}

		bool SeparatingAxis(const OrientedBox& A, const OrientedBox& B, const Vec2& Axis)
		{
			const double CenterDistance = std::fabs(Dot(Subtract(B.Center, A.Center), Axis));
			return CenterDistance > (ProjectedExtent(A, Axis) + ProjectedExtent(B, Axis));
		}

		// 2D oriented-box overlap via the separating axis theorem: no overlap if
		// any of the four candidate axes (each box's own right/forward) separates
		// them; otherwise they intersect. This is the "no overlapping clearance
		// volumes" hard constraint from docs/architecture.md.
		bool BoxesOverlap(const OrientedBox& A, const OrientedBox& B)
		{
			const Vec2 CandidateAxes[4] = {
				RightAxisFromAngle(A.RotationDegrees), ForwardAxisFromAngle(A.RotationDegrees),
				RightAxisFromAngle(B.RotationDegrees), ForwardAxisFromAngle(B.RotationDegrees)
			};

			for (const Vec2& Axis : CandidateAxes)
			{
				if (SeparatingAxis(A, B, Axis))
				{
					return false;
				}
			}
			return true;
		}

		// Selection: which groups belong in this room, given density and style.
		// Priority order (Seating, then Dining, then Storage) is a placeholder for
		// real room-area/catalogue-driven selection -- see docs/architecture.md.
		std::vector<const FurnitureGroup*> SelectGroups(EDensity Density, EStyle Style)
		{
			int TargetCount = 0;
			switch (Density)
			{
			case EDensity::None: TargetCount = 0; break;
			case EDensity::Low: TargetCount = 1; break;
			case EDensity::Medium: TargetCount = 2; break;
			case EDensity::High: TargetCount = 3; break;
			}

			const char* PriorityOrder[] = { "Seating", "Dining", "Storage" };

			std::vector<const FurnitureGroup*> Selected;
			for (const char* Role : PriorityOrder)
			{
				if (static_cast<int>(Selected.size()) >= TargetCount)
				{
					break;
				}

				for (const FurnitureGroup& Group : GetPlaceholderCatalogue())
				{
					if (Group.GroupRole != Role)
					{
						continue;
					}

					const bool bStyleMatches = Group.StyleTags.empty()
						|| std::find(Group.StyleTags.begin(), Group.StyleTags.end(), Style) != Group.StyleTags.end();
					if (!bStyleMatches)
					{
						continue;
					}

					Selected.push_back(&Group);
					break; // one group per role, for now
				}
			}

			return Selected;
		}
	}

	LayoutResult GenerateLayout(const RoomModel& Room, EDensity Density, EStyle Style)
	{
		LayoutResult Result;

		if (Room.Polygon.size() < 3)
		{
			return Result;
		}

		const double SignedArea = ComputeSignedArea(Room.Polygon);
		const double AreaSqM = std::fabs(SignedArea) / 10000.0;
		const double AreaScore = std::fmin(100.0, AreaSqM * 10.0);

		const Vec2 Centroid = ComputeCentroid(Room.Polygon);
		const bool bCCW = SignedArea > 0.0;
		std::vector<Wall> Walls = BuildWalls(Room.Polygon, bCCW);
		std::vector<bool> WallUsed(Walls.size(), false);
		std::vector<OrientedBox> PlacedBoxes;

		const std::vector<const FurnitureGroup*> SelectedGroups = SelectGroups(Density, Style);
		int PlacedGroupCount = 0;

		for (size_t GroupSequenceId = 0; GroupSequenceId < SelectedGroups.size(); ++GroupSequenceId)
		{
			const FurnitureGroup& Group = *SelectedGroups[GroupSequenceId];
			bool bGroupPlaced = false;

			for (size_t WallIndex = 0; WallIndex < Walls.size() && !bGroupPlaced; ++WallIndex)
			{
				if (WallUsed[WallIndex])
				{
					continue;
				}

				const Wall& CandidateWall = Walls[WallIndex];
				const double RoomInteriorDepth = EstimateRoomDepthFromWall(CandidateWall, Centroid);
				if (!GroupFitsOnWall(Group, CandidateWall, RoomInteriorDepth))
				{
					continue;
				}

				const Vec2 Forward = CandidateWall.InwardNormal;
				const Vec2 Right = Vec2{ Forward.Y, -Forward.X };
				const double ForwardAngle = AngleOfVector(Forward);
				const Vec2 AnchorPosition = Add(CandidateWall.Midpoint, Scale(Forward, Group.Depth * 0.5 + 5.0));

				// Stage this group's placements before committing, so it can be
				// rejected as a whole (and the next wall tried) if it would
				// overlap something already placed elsewhere in the room.
				std::vector<PlacedItem> CandidateItems;
				std::vector<OrientedBox> CandidateBoxes;
				bool bOverlapsExisting = false;

				for (const FurnitureItem& Item : Group.Items)
				{
					const Vec2 WorldOffset = Add(Scale(Right, Item.LocalOffset.X), Scale(Forward, Item.LocalOffset.Y));
					const Vec2 WorldPosition = Add(AnchorPosition, WorldOffset);
					const double WorldRotation = ForwardAngle + Item.LocalRotationDegrees;

					OrientedBox Box;
					Box.Center = WorldPosition;
					Box.HalfWidth = Item.FootprintWidth * 0.5 + Item.ClearanceMargin;
					Box.HalfDepth = Item.FootprintDepth * 0.5 + Item.ClearanceMargin;
					Box.RotationDegrees = WorldRotation;
					Box.GroupIndex = static_cast<int>(GroupSequenceId);

					for (const OrientedBox& Existing : PlacedBoxes)
					{
						if (BoxesOverlap(Box, Existing))
						{
							bOverlapsExisting = true;
							break;
						}
					}
					if (bOverlapsExisting)
					{
						break;
					}

					PlacedItem NewItem;
					NewItem.ItemId = Group.GroupId + "_" + Item.Id;
					NewItem.GroupRole = Group.GroupRole;
					NewItem.Position = WorldPosition;
					NewItem.RotationDegrees = WorldRotation;

					CandidateItems.push_back(NewItem);
					CandidateBoxes.push_back(Box);
				}

				if (bOverlapsExisting)
				{
					continue; // try the next wall
				}

				Result.Items.insert(Result.Items.end(), CandidateItems.begin(), CandidateItems.end());
				PlacedBoxes.insert(PlacedBoxes.end(), CandidateBoxes.begin(), CandidateBoxes.end());
				WallUsed[WallIndex] = true;
				bGroupPlaced = true;
			}

			if (bGroupPlaced)
			{
				++PlacedGroupCount;
			}
		}

		// Clearance: re-check the final set independently of Placement's own
		// rejection above, so this stays a real evaluation, not just a mirror of
		// Placement's bookkeeping. Hard constraint per docs/architecture.md --
		// should always be 100 given Placement already rejects overlaps, but is
		// computed honestly rather than assumed. Only cross-group overlaps count:
		// a group's internal composition is authored and trusted (see "Composition
		// by groups" in docs/architecture.md), so items overlapping within the
		// same group is expected, not a clearance violation.
		int OverlapCount = 0;
		for (size_t A = 0; A < PlacedBoxes.size(); ++A)
		{
			for (size_t B = A + 1; B < PlacedBoxes.size(); ++B)
			{
				if (PlacedBoxes[A].GroupIndex != PlacedBoxes[B].GroupIndex && BoxesOverlap(PlacedBoxes[A], PlacedBoxes[B]))
				{
					++OverlapCount;
				}
			}
		}
		const double ClearanceScore = OverlapCount == 0 ? 100.0 : std::fmax(0.0, 100.0 - OverlapCount * 25.0);

		const double CompletenessScore = SelectedGroups.empty()
			? 100.0
			: 100.0 * static_cast<double>(PlacedGroupCount) / static_cast<double>(SelectedGroups.size());

		Result.Breakdown.push_back(CriterionScore{ "FloorArea", AreaScore });
		Result.Breakdown.push_back(CriterionScore{ "Clearance", ClearanceScore });
		Result.Breakdown.push_back(CriterionScore{ "Completeness", CompletenessScore });
		Result.TotalScore = (AreaScore + ClearanceScore + CompletenessScore) / 3.0;

		return Result;
	}
}
