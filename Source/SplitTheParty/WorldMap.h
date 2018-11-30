// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hex.h"
#include "WorldMap.generated.h"

class AHexTile;
class UPerilReport;
class UAgent;

UCLASS()
class SPLITTHEPARTY_API AWorldMap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldMap();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly,EditAnywhere)
	TMap<FHex, AHexTile*> HexTileMap;

public:
	// Return the tile at the given coordinate or returns null if not found.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AHexTile* GetTile(FHex Coords);

	// Find the neighbors of one of this world's tiles.
	UFUNCTION(BlueprintCallable,BlueprintPure)
	TArray<AHexTile*> GetNeighborsOf(AHexTile* Tile);

	// Get all the tiles
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AHexTile*> GetTiles();

	// Remove the given peril from the world map.
	UFUNCTION(BlueprintCallable)
	void RemovePeril(UPeril* Peril);
};
