#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LayoutCoreBridge.h"
#include "LayoutTestActor.generated.h"

// Drag this into a level and press Play: it runs the propose/evaluate pipeline
// on RoomOutlinePoints and draws the result, so the prototype is visible without
// needing any authored Content assets yet.
UCLASS()
class ATELIER3D_API ALayoutTestActor : public AActor
{
	GENERATED_BODY()

public:
	ALayoutTestActor();

	// Room outline in centimeters, wound in order. Defaults to a 4m x 3m rectangle.
	UPROPERTY(EditAnywhere, Category = "Atelier3D|Layout")
	TArray<FVector2D> RoomOutlinePoints;

	UPROPERTY(EditAnywhere, Category = "Atelier3D|Layout")
	EFurnitureDensity Density = EFurnitureDensity::Medium;

	UPROPERTY(EditAnywhere, Category = "Atelier3D|Layout")
	EDesignStyle Style = EDesignStyle::Japanese;

protected:
	virtual void BeginPlay() override;
};
