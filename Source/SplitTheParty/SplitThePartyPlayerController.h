// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SplitThePartyPlayerController.generated.h"

class UAgent;
/**
 * Player controller for the regular game.
 */
UCLASS()
class SPLITTHEPARTY_API ASplitThePartyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void ShowRumorReport(UAgent* Agent);
};
