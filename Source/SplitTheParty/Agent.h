// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Hex.h"
#include "Agent.generated.h"

class URumor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAgentMoved, UAgent*, Agent);

/**
 * An agent who can move across the board and know rumors of perils.
 */
UCLASS(BlueprintType)
class SPLITTHEPARTY_API UAgent : public UObject
{
	GENERATED_BODY()

public:
	UAgent();

	// This agent's name that can be shown to the player.
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FText Name;

	// This agent's path
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<FHex> Path;

	// Construct the text that can be shown to the player in a rumor report.
	// This can be done with or without the rich text "<Overrides></>" styles.
	UFUNCTION(BlueprintCallable)
	FText GenerateReport(bool bIncludeOverrideStyles);

	// The hex where this agent currently is.
	UPROPERTY(BlueprintReadOnly,VisibleInstanceOnly)
	FHex Hex;

	// How long does this agent rest at a tile before moving upon update.
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int32 TileRestRounds;

	// Update this agent, ticking down his rest timer and potentially moving him centerward.
	void Update();

	// Notifies listeners when this agent moves
	UPROPERTY(BlueprintAssignable, VisibleInstanceOnly)
	FOnAgentMoved OnAgentMoved;

	// Set this agent's rumor
	void SetRumor(URumor* Rumor_);

	// Get this agent's rumor
	UFUNCTION(BlueprintCallable)
	URumor* GetRumor();

protected:
	// A peril that this agent believes to exist.
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	URumor* Rumor;

private:

	// How many rests are remaining before this agent moves.
	int32 RestsRemaining;

	void MoveTowardCenter();

	// The report that was generated on the first call to GenerateReport
	FText Report;
	// Indicates if a report has been generated or not, so we know whether to use the already-computed Report.
	bool bWasReportGenerated;

};