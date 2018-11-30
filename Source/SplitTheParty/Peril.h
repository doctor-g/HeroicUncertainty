// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Hex.h"
#include "Peril.generated.h"

UENUM(BlueprintType)
enum class EPerilSize : uint8
{
	Small,
	Medium,
	Large
};

// The type of challenge a peril presents.
UENUM(BlueprintType)
enum class EPerilType : uint8
{
	Strength,
	Spirit
};

/**
 * An actual peril that a hero can face in a quest.
 */
UCLASS(BlueprintType)
class SPLITTHEPARTY_API UPeril : public UObject
{
	GENERATED_BODY()

public:
	UPeril();

private:
	FText Name;
	int32 Strength;
	EPerilSize Size;
	FHex Hex;
	EPerilType Type;

	int32 TurnsUntilTileDestruction;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FText GetPerilName() { return Name; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetPower() { return Strength;  }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EPerilSize GetSize() { return Size;  }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FHex GetHex();

	UFUNCTION(BlueprintCallable, BlueprintPure)
	EPerilType GetType() { return Type; }

	void SetPerilName(FText Name_);
	void SetStrength(int32 Strength_);
	void SetSize(EPerilSize Size_);
	void SetHex(FHex Hex_);
	void SetType(EPerilType Type_);

	// Update the state of this peril for a round's ending.
	// This will tick down the counter for tile destruction.
	void UpdateForEndOfRound();
};
