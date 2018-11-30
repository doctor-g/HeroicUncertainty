// Fill out your copyright notice in the Description page of Project Settings.

#include "WorldMap.h"
#include "HexTile.h"
#include "Peril.h"

// Sets default values
AWorldMap::AWorldMap()
{
 	// Turn off ticks, at least for now.
	PrimaryActorTick.bCanEverTick = false;
}

void AWorldMap::BeginPlay()
{
	Super::BeginPlay();
}

AHexTile * AWorldMap::GetTile(FHex Coords)
{
	AHexTile** Value = HexTileMap.Find(Coords);
	if (Value) 
	{
		return *Value;
	}
	else
	{
		return nullptr;
	}
}

TArray<AHexTile*> AWorldMap::GetNeighborsOf(AHexTile * Tile)
{
	checkf(Tile, TEXT("Tile pointer is null"));
	checkf(*HexTileMap.Find(Tile->Coordinates), TEXT("Tile coordinates not in world map."));
	checkf(*HexTileMap.Find(Tile->Coordinates) == Tile, TEXT("Tile at given coordinates do not refer to the given tile."));
	TArray<FHex> NeighboringHexes = UHexBlueprintLibrary::FindNeighbors(Tile->Coordinates);
	TArray<AHexTile*> Result;
	for (const auto Hex : NeighboringHexes)
	{
		AHexTile* PotentialNeighborTile = GetTile(Hex);
		if (PotentialNeighborTile)
		{
			Result.Add(PotentialNeighborTile);
		}
	}
	return Result;
}

TArray<AHexTile*> AWorldMap::GetTiles()
{
	TArray<AHexTile*> AllTiles;
	HexTileMap.GenerateValueArray(AllTiles);
	return AllTiles;
}

void AWorldMap::RemovePeril(UPeril * Peril)
{
	if (Peril)
	{
		// Go find the owner
		for (auto Entry : HexTileMap)
		{
			if (Entry.Value->Peril == Peril)
			{
				Entry.Value->Peril = nullptr;
				return;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Peril was not found in the map"));
	}
}

