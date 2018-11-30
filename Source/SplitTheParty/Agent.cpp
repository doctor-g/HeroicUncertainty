// Fill out your copyright notice in the Description page of Project Settings.

#include "Agent.h"
#include "Peril.h"
#include "HexTile.h"
#include "Rumor.h"
#include "Engine/World.h"
#include "SplitThePartyGameMode.h"
#include "WorldMap.h"

UAgent::UAgent()
	: TileRestRounds(3)
	, RestsRemaining(0)
	, bWasReportGenerated(false)
{
}

FText RandomFrom(TArray<FString> Array)
{
	checkf(Array.Num() > 0, TEXT("Array may not be empty."));
	FString Entry = Array[FMath::RandRange(0, Array.Num() - 1)]; // For now, the key and value are the same since I'm not doing i18n.
	return FText::FromString(Entry);
}

FText GenerateSizeText(EPerilSize Size)
{
	TArray<FString> Array; 
	switch (Size)
	{
	case EPerilSize::Small:
		Array = { TEXT("small "), TEXT("short ") };
		return RandomFrom(Array);
	case EPerilSize::Medium:
		//  Medium-sized things don't get qualifiers.
		return FText();
	case EPerilSize::Large:
		Array = { TEXT("large "), TEXT("huge ") };
		return RandomFrom(Array);
	default:
		checkf(false, TEXT("Unhandled EPerilSize value."));
		return FText();
	}
}

FText GenerateTypeText(EPerilType Type)
{
	if (Type == EPerilType::Spirit)
	{
		return NSLOCTEXT("UiText", "SpiritMessage", " This will require magical prowess.");
	}
	else
	{
		return FText::GetEmpty();
	}
}

FString AnnotateProperty(FString Text, bool bIsOverridden, bool bIncludeOverrideStyles)
{
	FString BracedText = TEXT("{") + Text + TEXT("}");
	return (bIsOverridden && bIncludeOverrideStyles) ? (TEXT("<Overridden>") + BracedText + TEXT("</>")) : BracedText;
}

#define STRENGTH(Mesg) NSLOCTEXT("UiText", Mesg, Mesg)
FText GeneratePowerText(uint32 Strength)
{
	switch (Strength)
	{
	case 0:
	case 1:
		return STRENGTH(" It should be a trivial matter (Power 0-1).");
	case 2:
	case 3:
		return STRENGTH(" It may pose a challenge (Power 2-3).");
	case 4:
	case 5:
		return STRENGTH(" Make sure you are prepared (Power 4-5).");
	default:
		return STRENGTH(" It is terribly powerful! (Power 6+)");
	}
}
#undef STRENGTH

#define ARTICLE(Char) (Vowels.Contains(Char) ? "an" : "a")

FText UAgent::GenerateReport(bool bIncludeOverrideStyles)
{
	if (!bWasReportGenerated)
	{
		checkf(Rumor, TEXT("Rumor is null."));

		FText Size = GenerateSizeText(Rumor->GetSize());
		FText PerilName = Rumor->GetName();
		FString Article;
		TCHAR VowelArray[] = { 'a', 'e', 'i', 'o', 'u', 'A', 'E', 'I', 'O', 'U' };
		TArray<TCHAR> Vowels;
		Vowels.Append(VowelArray, ARRAY_COUNT(VowelArray));
		FString SizeString = Size.ToString();
		FString PerilNameString = PerilName.ToString();
		Article = (SizeString.IsEmpty()) ? ARTICLE(PerilNameString[0]) : ARTICLE(SizeString[0]);

		FFormatNamedArguments Arguments;
		Arguments.Add(TEXT("AgentName"), Name);
		Arguments.Add(TEXT("PerilName"), PerilName);
		Arguments.Add(TEXT("TileName"), CastChecked<ASplitThePartyGameMode>(GetWorld()->GetAuthGameMode())->GetWorldMap()->GetTile(Rumor->GetHex())->GetTileName());
		Arguments.Add(TEXT("Size"), Size);
		Arguments.Add(TEXT("Type"), GenerateTypeText(Rumor->GetType()));
		Arguments.Add(TEXT("Power"), GeneratePowerText(Rumor->GetPower()));

		FString FormattableString =
			FString::Printf(
				TEXT("{AgentName} reports that %s %s%s is active at %s.%s%s"),
				*Article,
				*AnnotateProperty("Size", Rumor->OverridesSize(), bIncludeOverrideStyles),
				*AnnotateProperty("PerilName", Rumor->OverridesName(), bIncludeOverrideStyles),
			    *AnnotateProperty("TileName", Rumor->OverridesHex(), bIncludeOverrideStyles),
				*AnnotateProperty("Type", Rumor->OverridesType(), bIncludeOverrideStyles),
				*AnnotateProperty("Power", Rumor->OverridesPower(), bIncludeOverrideStyles)
			);
		Report = FText::Format(FText::FromString(FormattableString), Arguments);
		bWasReportGenerated = true;
	}
	return Report;
}

void UAgent::Update()
{
	--RestsRemaining;
	if (RestsRemaining <= 0)
	{
		if (Hex != FHex(0, 0))
		{
			MoveTowardCenter();
			OnAgentMoved.Broadcast(this);
		}
		RestsRemaining = TileRestRounds;
	}
}

void UAgent::SetRumor(URumor * Rumor_)
{
	checkf(Rumor_, TEXT("Rumor may not be set to null"));
	UE_LOG(LogTemp, Display, TEXT("Setting the rumor of %s to %s"), *Name.ToString(), *Rumor_->GetPeril()->GetPerilName().ToString());
	Rumor = Rumor_;
}

URumor * UAgent::GetRumor()
{
	return Rumor;
}

void UAgent::MoveTowardCenter()
{
	// I am having trouble finding or figuring out a concise expression for this, so brute force it is.
	TArray<FHex> Neighbors = UHexBlueprintLibrary::FindNeighbors(Hex);
	FHex Candidate;
	const FHex Center(0, 0);
	int32 MinDistance = 1000; // Some big number
	for (auto Neighbor : Neighbors)
	{
		int32 Distance = UHexBlueprintLibrary::Distance(Neighbor, Center);
		if (Distance < MinDistance)
		{
			Candidate = Neighbor;
			MinDistance = Distance;
		}
	}
	Hex = Candidate;
}

