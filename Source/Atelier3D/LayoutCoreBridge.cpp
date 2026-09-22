#include "LayoutCoreBridge.h"
#include "LayoutCore/RoomLayout.h"

namespace
{
	LayoutCore::EDensity ToLayoutCore(EFurnitureDensity Density)
	{
		switch (Density)
		{
		case EFurnitureDensity::None: return LayoutCore::EDensity::None;
		case EFurnitureDensity::Low: return LayoutCore::EDensity::Low;
		case EFurnitureDensity::Medium: return LayoutCore::EDensity::Medium;
		case EFurnitureDensity::High: return LayoutCore::EDensity::High;
		}
		return LayoutCore::EDensity::None;
	}

	LayoutCore::EStyle ToLayoutCore(EDesignStyle Style)
	{
		switch (Style)
		{
		case EDesignStyle::Japanese: return LayoutCore::EStyle::Japanese;
		case EDesignStyle::Scottish: return LayoutCore::EStyle::Scottish;
		}
		return LayoutCore::EStyle::Japanese;
	}

	LayoutCore::RoomModel ToLayoutCore(const TArray<FVector2D>& RoomOutline)
	{
		LayoutCore::RoomModel Room;
		Room.Polygon.reserve(RoomOutline.Num());
		for (const FVector2D& Point : RoomOutline)
		{
			Room.Polygon.push_back(LayoutCore::Vec2{ Point.X, Point.Y });
		}
		return Room;
	}
}

FLayoutGenerationResult ULayoutCoreBridge::GenerateRoomLayout(const TArray<FVector2D>& RoomOutline, EFurnitureDensity Density, EDesignStyle Style)
{
	const LayoutCore::RoomModel Room = ToLayoutCore(RoomOutline);
	const LayoutCore::LayoutResult Layout = LayoutCore::GenerateLayout(Room, ToLayoutCore(Density), ToLayoutCore(Style));

	FLayoutGenerationResult Result;
	Result.TotalScore = static_cast<float>(Layout.TotalScore);

	Result.Items.Reserve(static_cast<int32>(Layout.Items.size()));
	for (const LayoutCore::PlacedItem& Item : Layout.Items)
	{
		FPlacedFurnitureItem UEItem;
		UEItem.ItemId = FString(Item.ItemId.c_str());
		UEItem.GroupRole = FString(Item.GroupRole.c_str());
		UEItem.Position = FVector2D(Item.Position.X, Item.Position.Y);
		UEItem.RotationDegrees = static_cast<float>(Item.RotationDegrees);
		Result.Items.Add(UEItem);
	}

	Result.Breakdown.Reserve(static_cast<int32>(Layout.Breakdown.size()));
	for (const LayoutCore::CriterionScore& Criterion : Layout.Breakdown)
	{
		FCriterionScore UECriterion;
		UECriterion.Name = FString(Criterion.Name.c_str());
		UECriterion.Value = static_cast<float>(Criterion.Value);
		Result.Breakdown.Add(UECriterion);
	}

	return Result;
}
