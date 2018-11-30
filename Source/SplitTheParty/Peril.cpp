// Fill out your copyright notice in the Description page of Project Settings.

#include "Peril.h"
#include "HexTile.h"
#include "Engine/World.h"
#include "SplitThePartyGameMode.h"

UPeril::UPeril()
	: TurnsUntilTileDestruction(5)
{}

FHex UPeril::GetHex()
{
	return Hex;
}

void UPeril::SetPerilName(FText Name_)
{
	Name = Name_;
}

void UPeril::SetStrength(int32 Strength_)
{
	Strength = Strength_;
}

void UPeril::SetSize(EPerilSize Size_)
{
	Size = Size_;
}

void UPeril::SetHex(FHex Hex_)
{
	Hex = Hex_;
}

void UPeril::SetType(EPerilType Type_)
{
	Type = Type_;
}

void UPeril::UpdateForEndOfRound()
{
	--TurnsUntilTileDestruction;
	UE_LOG(LogTemp, Display, TEXT("Turns until %s defiles hex %d,%d: %d"), *Name.ToString(), Hex.Q, Hex.R, TurnsUntilTileDestruction);
	if (TurnsUntilTileDestruction <= 0)
	{
		ASplitThePartyGameMode* GameMode = CastChecked<ASplitThePartyGameMode>(GetWorld()->GetAuthGameMode());
		AHexTile* Tile = GameMode->GetTile(Hex);
		check(Tile);
		Tile->Defile();
		GameMode->RemovePeril(Tile->Peril);
	}
}