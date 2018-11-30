// Fill out your copyright notice in the Description page of Project Settings.

#include "HexTile.h"
#include "Hex.h"
#include "WorldMap.h"
#include "PerilMarker.h"

// Sets default values
AHexTile::AHexTile()
	: bShowCoordinates(false)
{}

void AHexTile::SetWorldMap(AWorldMap * WorldMap_)
{
	if (ensure(WorldMap_))
	{
		WorldMap = TWeakObjectPtr<AWorldMap>(WorldMap_);
	}
}

TArray<AHexTile*> AHexTile::GetNeighbors()
{
	return WorldMap->GetNeighborsOf(this);
}

void AHexTile::Defile()
{
	bDefiled = true;
	OnTileDefiled.Broadcast(this);
}

bool AHexTile::IsDefiled()
{
	return bDefiled;
}


void AHexTile::SetTileName_Implementation(const FText& Name_)
{
	Name = Name_;
}


void AHexTile::SetCoordinates_Implementation(const FHex Coordinates_)
{
	Coordinates = Coordinates_;
}

void AHexTile::SetTileActions_Implementation(const TArray<EHexAction>& Actions_)
{
	Actions = Actions_;
}

void AHexTile::SetActionTint_Implementation(FLinearColor Tint_)
{
	ActionTint = Tint_;
}