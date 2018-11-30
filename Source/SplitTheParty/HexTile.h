// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hex.h"
#include "TextProperty.h"
#include "HexTile.generated.h"

class AWorldMap;
class UPeril;
class AHexTile;

UENUM(BlueprintType)
enum class EHexAction : uint8
{
	Move,
	Quest,
	Rest
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileDefiled, AHexTile*, Tile);

UCLASS()
class SPLITTHEPARTY_API AHexTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHexTile();

protected:
	// Player-facing name for this hex
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FText Name;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TWeakObjectPtr<AWorldMap> WorldMap;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<EHexAction> Actions;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FLinearColor ActionTint;

public:

	// The position of this tile within the hex grid structure.
	// It has to be non-const since it is set after instantiation.
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FHex Coordinates;

	// The peril at this location, which may be null.
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UPeril* Peril;

	inline FText GetTileName() { return Name; }

	// Change the name of this tile
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetTileName(const FText& Name_);

	// Change the coordinates of this tile.
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void SetCoordinates(const FHex Coords);

	// Specify the owning map of this tile.
	// This is needed so that the tile can find neighbors, for example.
	UFUNCTION(BlueprintCallable)
	void SetWorldMap(AWorldMap* WorldMap_);
	
	// Get the neighbors of this tile on the map
	UFUNCTION(BlueprintCallable,BlueprintPure)
	TArray<AHexTile*> GetNeighbors();

	// Update the actions allowable on this tile.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetTileActions(const TArray<EHexAction>& Actions_);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetActionTint(FLinearColor Tint_);

	// Mark this tile as defiled (e.g. destroyed by an unaddressed peril)
	void Defile();

	// Check if this tile is defiled.
	bool IsDefiled();

	// Should tiles show their coordinates
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool bShowCoordinates;

	UFUNCTION(BlueprintImplementableEvent)
	FTransform GetPerilMarkerSocketTransform();

private:
	bool bDefiled;

public:
	UPROPERTY(BlueprintAssignable)
	FOnTileDefiled OnTileDefiled;
};
