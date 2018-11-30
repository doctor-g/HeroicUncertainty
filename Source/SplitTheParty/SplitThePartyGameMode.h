// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Hex.h"
#include "SplitThePartyGameMode.generated.h"

class UPeril;
class UAgent;
class AWorldMap;
class AHero;
class UDataTable;
class UAgentGenerator;
class URumor;
class APerilMarker;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAgentRumorReported, UAgent*, Agent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHeroTurnStart, AHero*, Hero);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRoundChange, int32, NewRoundNumber);

UENUM()
enum class EPerilGenerationBounds : uint8
{
	InnerCircleOnly,
	OuterCircleOnly,
	AnyCircle
};

/**
 * Base game mode for the Split The Party Game.
 */
UCLASS()
class SPLITTHEPARTY_API ASplitThePartyGameMode : public AGameModeBase
{
	GENERATED_BODY()

	ASplitThePartyGameMode();

protected:
	virtual void BeginPlay() override;

	// The game world
	UPROPERTY(BlueprintReadOnly)
	AWorldMap* WorldMap;

	// Called to indicate that a hero's turn is finished.
	UFUNCTION(BlueprintCallable)
	void FinishHeroTurn();

	// The current hero whose turn it is.
	// May be null while it is not a hero's turn.
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "World State")
	AHero* CurrentHero;

	// The array of all the heroes.
	// This should not be modified outside of initialization.
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "World State")
	TArray<AHero*> Heroes;

	// The array of active agents
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "World State")
	TArray<UAgent*> Agents;

	// Called to start the current hero's turn
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void StartTurn();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SpawnHeroOnTile(int32 index, AHexTile* Tile);

	// Called when a hero's movement action is complete
	UFUNCTION(BlueprintNativeEvent)
	void OnHeroMovementComplete();

	// Called when a tile is defiled
	UFUNCTION()
	void OnTileDefiled(AHexTile* Tile);

	// Call to end the game
	UFUNCTION(BlueprintImplementableEvent)
	void EndGame();

	// The agent generator
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "World Config")
	TSubclassOf<UAgentGenerator> AgentGeneratorClass;

	UPROPERTY()
	UAgentGenerator* AgentGenerator;

	// The PerilMarker blueprint class
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "World Config")
	TSubclassOf<APerilMarker> PerilMarkerClass;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<APerilMarker*> PerilMarkers;

	// The enemies in this world
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "World Config")
	UDataTable* EnemyTable;

	// The number of rounds between peril generation.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "World Config")
	int32 RoundsBetweenPerilGeneration;

	// The number of rounds until the next peril.
	// This counts down.
	int32 RoundsUntilNextPerilGeneration;

public:
	void MarkReportedPeril(UAgent* ReportingAgent);

	UPROPERTY(BlueprintAssignable)
	FOnHeroTurnStart OnHeroTurnStart;

	UPROPERTY(BlueprintAssignable)
	FOnRoundChange OnRoundChange;

	UPROPERTY(BlueprintReadOnly)
	int32 Round;

	// Determine which heroes are on a tile.
	// These heroes are returned in order of increasing index.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AHero*> GetHeroesAt(AHexTile* Tile);

	// Get the agents at a particular tile.
	UFUNCTION(BlueprintCallable,BlueprintPure)
	TArray<UAgent*> GetAgentsAt(AHexTile* Tile);

	// Look up a particular tile.
	AHexTile* GetTile(FHex Hex);

	// Remove a peril and its markers and resources
	void RemovePeril(UPeril* Peril);

	AWorldMap* GetWorldMap();

private:
	// Initialize the first set of perils and markers.
	// This is done outside of BeginPlay so we can ensure all other actors are up and ready.
	void SetupTestMap();

	// Generate a peril
	UPeril* GeneratePeril(EPerilGenerationBounds Bounds = EPerilGenerationBounds::AnyCircle);

	// Create a rumor
	URumor* CreateRumor(UPeril* Source, UDataTable* EnemyTable, float MutationChance = 0);

	// The index of the current hero.
	// This is used to go through all the players when a turn ends.
	int32 CurrentHeroIndex;

	UFUNCTION()
	void OnAgentMoved(UAgent* Agent);

	// Keep track of the agents to remove in mark-and-sweep style
	TArray<UAgent*> AgentsToRemoveDuringUpdate;

	// The number of tiles that can be defiled before the game is over.
	int32 DefilableTilesRemaining;


};
