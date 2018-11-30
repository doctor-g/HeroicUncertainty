// Fill out your copyright notice in the Description page of Project Settings.

#include "Hero.h"
#include "SplitThePartyGameMode.h"
#include "QuestResult.h"
#include "Peril.h"
#include "Engine/World.h"


// Sets default values
AHero::AHero()
	: bActedThisRound(false)
	, Strength(2)
	, Spirit(2)
	, MaxStamina(5)
	, Stamina(5)
{
 	// Currently does not tick.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AHero::BeginPlay()
{
	Super::BeginPlay();

	switch (HeroType)
	{
	case EHeroType::Knight:
		Strength = 6;
		break;
	case EHeroType::Bard:
		Strength = 3;
		Spirit = 3;
		break;
	case EHeroType::Sage:
		Spirit = 8;
		MaxStamina = Stamina = 4;
		break;
	}

	ASplitThePartyGameMode* GameMode = CastChecked<ASplitThePartyGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->OnRoundChange.AddDynamic(this, &AHero::OnRoundChange);
}

int32 TotalStrengthOf(TArray<AHero*> Heroes)
{
	int32 Total = 0;
	for (auto Hero : Heroes)
	{
		Total += Hero->GetStrength();
	}
	return Total;
}

int32 TotalSpiritOf(TArray<AHero*> Heroes)
{
	int32 Total = 0;
	for (auto Hero : Heroes)
	{
		Total += Hero->GetSpirit();
	}
	return Total;
}

FText AHero::GetHeroName()
{
	switch (HeroType)
	{
	case EHeroType::Bard:
		return NSLOCTEXT("UiText", "Bard", "Bard");
	case EHeroType::Knight:
		return NSLOCTEXT("UiText", "Knight", "Knight");
	case EHeroType::Sage:
		return NSLOCTEXT("UiText", "Sage", "Sage");
	default:
		checkf(false, TEXT("Uncaught hero type constant"));
		return FText::FromString("None");
	}
}

UQuestResult* AHero::Quest()
{
	checkf(Tile, TEXT("Hero's tile not specified"));
	UQuestResult* Result = NewObject<UQuestResult>();

	// Regardless of whether there is a peril, all the heroes on this tile have quested this turn.
	ASplitThePartyGameMode* GameMode = Cast<ASplitThePartyGameMode>(GetWorld()->GetAuthGameMode());
	TArray<AHero*> HeroesOnTile = GameMode->GetHeroesAt(Tile);
	for (auto Hero : HeroesOnTile)
	{
		Hero->bActedThisRound = true;
	}

	// Move forward with the quest based on whether there is a peril there or not.
	UPeril* Peril = Tile->Peril;
	UE_LOG(LogTemp, Display, TEXT("Questing and peril is %s"), Peril ? (*Peril->GetPerilName().ToString()) : TEXT("not here"));
	if (Peril)
	{
		Result->Peril = Peril;
		TArray<UQuestEffect*> Effects;
		Result->TotalHeroPower = Peril->GetType() == EPerilType::Strength ? TotalStrengthOf(HeroesOnTile) : TotalSpiritOf(HeroesOnTile);

		if (Result->TotalHeroPower < Peril->GetPower())
		{
			Result->bSuccess = false;
		}
		else {
			Result->bSuccess = true;
			Tile->Peril = nullptr;
			GameMode->RemovePeril(Peril);
		}

		// Each hero loses stamina, but the heroes earlier in the index order lose it first.
		for (int32 i = 0; i < HeroesOnTile.Num(); i++)
		{
			UStaminaLossQuestEffect* StaminaLoss = NewObject<UStaminaLossQuestEffect>();
			StaminaLoss->Hero = HeroesOnTile[i];
			const int32 Power = Peril->GetPower();
			const int32 NumHeroes = HeroesOnTile.Num();
			const int32 CommonLoss = Power / NumHeroes;
			const int32 RemainderToAssign = Power % NumHeroes;
			const int32 ExtraLossForThisHero = i < RemainderToAssign ? 1 : 0;
			StaminaLoss->Reduction = CommonLoss + ExtraLossForThisHero;
			Effects.Add(StaminaLoss);
		}
		Result->Effects = Effects;
	}
	return Result;
}

int32 AHero::ReduceStamina(int32 Amount)
{
	checkf(Amount >= 0, TEXT("Amount to lose must be non-negative"));
	Stamina = FMath::Max(0, Stamina - Amount);
	OnStaminaChanged.Broadcast(Stamina);
	return Stamina;
}

void AHero::Rest_Implementation()
{
	Stamina = MaxStamina;
	OnStaminaChanged.Broadcast(Stamina);
}

TArray<EHexAction> AHero::DeterminePossibleActions(AHexTile * AtTile)
{
	TArray<EHexAction> Result;
	int32 DistanceFromHero = UHexBlueprintLibrary::Distance(AtTile->Coordinates, Tile->Coordinates);
	if (DistanceFromHero == 0)
	{
		if (Stamina > 0 && !AtTile->IsDefiled())
		{
			Result.Add(EHexAction::Quest);
		}
		if (Stamina < 5) // TODO Replace with abstraction
		{
			Result.Add(EHexAction::Rest);
		}
	}
	else if (DistanceFromHero == 1) 
	{
		if (AtTile->IsDefiled())
		{
			if (Stamina >= 2)
			{
				Result.Add(EHexAction::Move);
			}
		}
		else
		{
			if (Stamina >= 1)
			{
				Result.Add(EHexAction::Move);
			}
		}
	}
	return Result;
}

bool AHero::RevealsRumorOverrides()
{
	return HeroType == EHeroType::Bard;
}

bool AHero::ActedThisRound()
{
	return bActedThisRound;
}

void AHero::OnRoundChange(int32 NewRoundNumber)
{
	bActedThisRound = false;
}

void AHero::MoveTo_Implementation(AHexTile * Destination)
{
	// BlueprintNative. Make sure to call this from the blueprint implementation.
	check(Destination);
	Tile = Destination;
	ReduceStamina(Tile->IsDefiled() ? 2 : 1);
}
