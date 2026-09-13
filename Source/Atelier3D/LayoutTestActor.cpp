#include "LayoutTestActor.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"

ALayoutTestActor::ALayoutTestActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RoomOutlinePoints = {
		FVector2D(0.0f, 0.0f),
		FVector2D(400.0f, 0.0f),
		FVector2D(400.0f, 300.0f),
		FVector2D(0.0f, 300.0f)
	};
}

void ALayoutTestActor::BeginPlay()
{
	Super::BeginPlay();

	const FLayoutGenerationResult Result = ULayoutCoreBridge::GenerateRoomLayout(RoomOutlinePoints, Density, Style);

	const FVector Origin = GetActorLocation();
	for (const FPlacedFurnitureItem& Item : Result.Items)
	{
		const FVector WorldPos = Origin + FVector(Item.Position.X, Item.Position.Y, 50.0f);
		DrawDebugSphere(GetWorld(), WorldPos, 30.0f, 12, FColor::Yellow, true, -1.0f);
		DrawDebugString(GetWorld(), WorldPos + FVector(0.0f, 0.0f, 40.0f), Item.ItemId, nullptr, FColor::White, 0.0f, true);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green,
			FString::Printf(TEXT("Atelier3D layout score: %.1f (%d item(s))"), Result.TotalScore, Result.Items.Num()));
	}
}
