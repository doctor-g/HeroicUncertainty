// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HexTile.h"
#include "Hero.generated.h"

class UQuestResult;
class AHero;

UENUM(BlueprintType)
enum class EHeroType : uint8
{
	Knight,
	Bard,
	Sage
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHeroMovementComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChanged, int32, Stamina);

UCLASS()
class SPLITTHEPARTY_API AHero : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHero();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// The tile that this hero is currently on.
	// A hero is always on a tile.
	UPROPERTY(BlueprintReadOnly, Meta=(ExposeOnSpawn=true))
	AHexTile* Tile;

	// A distinctive color for this hero.
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Meta = (ExposeOnSpawn = true))
	FLinearColor Color;

	// Strength is used in most quests
	UPROPERTY(BlueprintReadOnly)
	int32 Strength;

	// Spirit is used in magical quests
	UPROPERTY(BlueprintReadOnly)
	int32 Spirit;

	// The hero's maximum stamina
	uint32 MaxStamina;

	// The hero's current stamina
	UPROPERTY(BlueprintReadOnly)
	int32 Stamina;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Meta = (ExposeOnSpawn = true))
	EHeroType HeroType;

	// Indicates if a hero has acted this round or not.
	// This is needed since questing "uses" all the heroes, potentially out of turn.
	bool bActedThisRound;

public:
	// Get the hero's name
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetHeroName();

	// Get the hero's tile
	inline AHexTile* GetTile() { return Tile; }

	// Get the hero's strength
	inline int32 GetStrength() { return Strength; }

	inline int32 GetSpirit() { return Spirit; }

	// Move this actor to another tile.
	// Blueprint implementation can handle the animation; this handles the state.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MoveTo(AHexTile* Destination);

	// Engage in the Quest action on the current tile.
	UFUNCTION(BlueprintCallable)
	UQuestResult* Quest();

	// Reduce this hero's stamina by the given amount.
	// Returns the new stamina value.
	UFUNCTION(BlueprintCallable)
	int32 ReduceStamina(int32 Amount);

	// Rest, restoring stamina
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Rest();

	// Get the actions possible by this hero at the given tile.
	// This assumes it is actually the hero's turn.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<EHexAction> DeterminePossibleActions(AHexTile* AtTile);

	// Check if this hero reveals rumor overrides
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(Keywords="Show Reveal Rumor Override"))
	bool RevealsRumorOverrides();
	
	// Check if the hero acted this round
	bool ActedThisRound();

	// Broadcast when the hero's movement is complete
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnHeroMovementComplete OnHeroMovementComplete;

	// Broadcast when stamina changes
	UPROPERTY(BlueprintAssignable)
	FOnStaminaChanged OnStaminaChanged;

private:
	// Called when the game's round changes
	UFUNCTION()
	void OnRoundChange(int32 NewRoundNumber);

};
