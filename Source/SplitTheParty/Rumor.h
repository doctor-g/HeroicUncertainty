// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Peril.h"
#include "Hex.h"
#include "Rumor.generated.h"

/**
 * A rumor believed about a peril.
 * Reifies the decorator pattern.
 */
UCLASS(BlueprintType)
class SPLITTHEPARTY_API URumor : public UObject
{
	GENERATED_BODY()

public:
	void Init(UPeril* Peril);

	// Check if this rumor is about a particular peril.
	bool IsRumorAbout(UPeril* Peril);

	UFUNCTION(BlueprintCallable,BlueprintPure)
	FText GetName() { return bOverrideName ? Name : Peril->GetPerilName(); }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetPower() { return bOverridePower ? Power : Peril->GetPower(); }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EPerilSize GetSize() { return bOverrideSize ? Size : Peril->GetSize(); }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FHex GetHex() { return bOverrideHex ? Hex : Peril->GetHex(); }
	UFUNCTION(BlueprintCallable, BlueprintPure)
	EPerilType GetType() { return bOverrideType ? Type : Peril->GetType(); }

	inline UPeril* GetPeril() { return Peril; }

protected:
	// The peril of which this is a rumor.
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	UPeril* Peril;

private:
	FText Name;
	int32 Power;
	EPerilSize Size;
	FHex Hex;
	EPerilType Type;

	bool bOverrideName = false;
	bool bOverridePower = false;
	bool bOverrideSize = false;
	bool bOverrideHex = false;
	bool bOverrideType = false;

public:
	void OverrideName(FText Value)
	{
		bOverrideName = true;
		Name = Value;
	}
	void OverridePower(int32 Value)
	{
		bOverridePower = true;
		Power = Value;
	}
	void OverrideSize(EPerilSize Value) {
		bOverrideSize = true;
		Size = Value;
	}
	void OverrideHex(FHex Value) {
		bOverrideHex = true;
		Hex = Value;
	}
	void OverrideType(EPerilType Value) {
		bOverrideType = true;
		Type = Value;
	}

	bool OverridesName() { return bOverrideName; }
	bool OverridesPower() { return bOverridePower; }
	bool OverridesSize() { return bOverrideSize; }
	bool OverridesHex() { return bOverrideHex;  }
	bool OverridesType() { return bOverrideType; }

};
