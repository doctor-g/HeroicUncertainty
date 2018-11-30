// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestResult.generated.h"

class UQuestEffect;
class UPeril;

// The result of a quest.
UCLASS(BlueprintType)
class SPLITTHEPARTY_API UQuestResult : public UObject
{
	GENERATED_BODY()

public:
	// The peril faced in this quest.
	// This can be null if this is a quest on an empty hex (right?).
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UPeril* Peril;

	// The total strength of the heroes at this location
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int32 TotalHeroPower;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	bool bSuccess;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	TArray<UQuestEffect*> Effects;

	// Commit all the effects in this quest result.
	UFUNCTION(BlueprintCallable)
	void CommitEffects();
};

// A single change that occurs as a result of the quest.
// Reifies the Command design pattern.
UCLASS(BlueprintType)
class SPLITTHEPARTY_API UQuestEffect : public UObject
{
	GENERATED_BODY()

public:
	// Commit this result.
	UFUNCTION(BlueprintCallable)
	virtual void Commit() {};
};

UCLASS(BlueprintType)
class SPLITTHEPARTY_API UStaminaLossQuestEffect : public UQuestEffect
{
	GENERATED_BODY()

public:
	// The hero who is losing stamina
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	class AHero* Hero;

	// The amount by which to reduce the hero's stamina
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int32 Reduction;

	virtual void Commit() override;
};