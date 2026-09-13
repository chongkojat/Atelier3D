#include "LayoutTestActor.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

ALayoutTestActor::ALayoutTestActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// A believable single-room scale (5m x 4m); shrink this in the Details
	// panel to see the Completeness score drop as fewer groups fit.
	RoomOutlinePoints = {
		FVector2D(0.0f, 0.0f),
		FVector2D(500.0f, 0.0f),
		FVector2D(500.0f, 400.0f),
		FVector2D(0.0f, 400.0f)
	};
}

namespace
{
	FColor ColorForGroupRole(const FString& GroupRole)
	{
		if (GroupRole == TEXT("Dining"))
		{
			return FColor::Orange;
		}
		if (GroupRole == TEXT("Storage"))
		{
			return FColor::Cyan;
		}
		return FColor::Yellow; // Seating, and anything else for now
	}
}

void ALayoutTestActor::BeginPlay()
{
	Super::BeginPlay();

	const FLayoutGenerationResult Result = ULayoutCoreBridge::GenerateRoomLayout(RoomOutlinePoints, Density, Style);

	const FVector Origin = GetActorLocation();
	for (const FPlacedFurnitureItem& Item : Result.Items)
	{
		const FVector WorldPos = Origin + FVector(Item.Position.X, Item.Position.Y, 50.0f);
		const FColor SphereColor = ColorForGroupRole(Item.GroupRole);
		DrawDebugSphere(GetWorld(), WorldPos, 25.0f, 12, SphereColor, true, -1.0f);
		DrawDebugString(GetWorld(), WorldPos + FVector(0.0f, 0.0f, 30.0f), Item.ItemId, nullptr, FColor::White, 0.0f, true);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Green,
			FString::Printf(TEXT("Atelier3D total score: %.1f (%d item(s))"), Result.TotalScore, Result.Items.Num()));

		for (const FCriterionScore& Criterion : Result.Breakdown)
		{
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::White,
				FString::Printf(TEXT("  %s: %.1f"), *Criterion.Name, Criterion.Value));
		}
	}
}
