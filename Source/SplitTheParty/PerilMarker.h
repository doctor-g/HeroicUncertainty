// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PerilMarker.generated.h"

class AHexTile;
class UAgent;
class UPeril;

UCLASS()
class SPLITTHEPARTY_API APerilMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APerilMarker();


protected:
	// Play the removal animation
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void PlayRemovalAnimation();

public:
	// Add a rumor to those represented by this marker.
	UFUNCTION(BlueprintCallable)
	void AddReportingAgent(UAgent* Agent);

	// Get the rumors represented by this marker
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UAgent*> GetReportingAgents();

	// Remove any agents referencing the given peril.
	void RemoveAgentsReferencing(UPeril* Peril);

	// Check if there are no agents left for this marker.
	bool IsEmpty();

	// Remove this marker from the world.
	void Remove();

protected:
	UPROPERTY()
	TArray<UAgent*> Agents;

	// Called by blueprints when the animation is done
	UFUNCTION(BlueprintCallable)
	void NotifyAnimationComplete();
};
