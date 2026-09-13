#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LayoutCoreBridge.generated.h"

// UE-facing mirrors of the LayoutCore:: types (Source/LayoutCore/Public/LayoutCore/RoomLayout.h).
// This header never includes LayoutCore/RoomLayout.h directly -- only LayoutCoreBridge.cpp does --
// so the engine-independent core stays out of anything Unreal Header Tool parses.

UENUM(BlueprintType)
enum class EFurnitureDensity : uint8
{
	None,
	Low,
	Medium,
	High
};

UENUM(BlueprintType)
enum class EDesignStyle : uint8
{
	Japanese,
	Scottish
};

USTRUCT(BlueprintType)
struct FPlacedFurnitureItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Atelier3D|Layout")
	FString ItemId;

	UPROPERTY(BlueprintReadOnly, Category = "Atelier3D|Layout")
	FString GroupRole;

	UPROPERTY(BlueprintReadOnly, Category = "Atelier3D|Layout")
	FVector2D Position = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Atelier3D|Layout")
	float RotationDegrees = 0.0f;
};

USTRUCT(BlueprintType)
struct FCriterionScore
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Atelier3D|Layout")
	FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Atelier3D|Layout")
	float Value = 0.0f;
};

USTRUCT(BlueprintType)
struct FLayoutGenerationResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Atelier3D|Layout")
	TArray<FPlacedFurnitureItem> Items;

	UPROPERTY(BlueprintReadOnly, Category = "Atelier3D|Layout")
	float TotalScore = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Atelier3D|Layout")
	TArray<FCriterionScore> Breakdown;
};

UCLASS()
class ATELIER3D_API ULayoutCoreBridge : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Selection + Placement + Evaluation for one room, per docs/architecture.md.
	// RoomOutline must be a closed polygon, in centimeters (Unreal units), wound in order.
	UFUNCTION(BlueprintCallable, Category = "Atelier3D|Layout")
	static FLayoutGenerationResult GenerateRoomLayout(const TArray<FVector2D>& RoomOutline, EFurnitureDensity Density, EDesignStyle Style);
};
