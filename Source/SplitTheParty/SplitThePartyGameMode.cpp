// Fill out your copyright notice in the Description page of Project Settings.

#include "SplitThePartyGameMode.h"
#include "Peril.h"
#include "HexTile.h"
#include "WorldMap.h"
#include "EngineUtils.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "Hero.h"
#include "Agent.h"
#include "PerilMarker.h"
#include "Rumor.h"
#include "EnemyTableRow.h"
#include "AgentGenerator.h"
#include "Engine/World.h"
#include "SplitThePartyPlayerController.h"
#include "Engine/Public/TimerManager.h"
#include <random>

ASplitThePartyGameMode::ASplitThePartyGameMode()
	: Round(1)
	, RoundsBetweenPerilGeneration(2)
	, RoundsUntilNextPerilGeneration(2)
	, DefilableTilesRemaining(3)
{
}

void ASplitThePartyGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Instantiate the agent generator
	checkf(AgentGeneratorClass, TEXT("Agent generator class must be specified"));
	AgentGenerator = NewObject<UAgentGenerator>(this, AgentGeneratorClass, TEXT("AgentGenerator"));

	// Get the game world
	WorldMap = *TActorIterator<AWorldMap>(GetWorld());
	checkf(WorldMap, TEXT("World map not found."));

	// Spawn all the heroes and update the heroes array.
	AHexTile* Center = WorldMap->GetTile(FHex(0, 0));
	for (int32 i = 0; i < 3; i++)
	{
		SpawnHeroOnTile(i, Center);
	}
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHero::StaticClass(), Actors);
	for (auto Actor : Actors)
	{
		AHero* Hero = Cast<AHero>(Actor);
		Heroes.Add(Hero);
		Hero->OnHeroMovementComplete.AddDynamic(this, &ASplitThePartyGameMode::OnHeroMovementComplete);
	}

	// Set it to be the first hero's turn.
	checkf(Heroes.Num() > 0, TEXT("Heroes array is not yet initialized"));
	CurrentHero = Heroes[0];

	// Register the defilement listener
	Actors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHexTile::StaticClass(), Actors);
	for (auto Actor : Actors)
	{
		AHexTile* Tile = CastChecked<AHexTile>(Actor);
		Tile->OnTileDefiled.AddDynamic(this, &ASplitThePartyGameMode::OnTileDefiled);
	}

	// Set up the test map after an imperceptible delay, to allow all the other initialization required.
	FTimerHandle UnusedHandle;
	GetWorldTimerManager().SetTimer(UnusedHandle, this, &ASplitThePartyGameMode::SetupTestMap, 0.1, false);
}

#define GETENUMSTRING(etype,evalue) ( (FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true) != nullptr) ? FindObject<UEnum>(ANY_PACKAGE, TEXT(etype), true)->GetNameStringByIndex((int32)evalue) : FString("Invalid - are you sure enum uses UENUM() macro?") )

URumor* ASplitThePartyGameMode::CreateRumor(UPeril* Source, UDataTable* Enemies, float MutationChance /* = 0*/)
{
	checkf(Source, TEXT("Source may not be null."));

	UE_LOG(LogTemp, Display, TEXT("Creating a rumor around a perilous %s"), *Source->GetPerilName().ToString());

	URumor* Rumor = NewObject<URumor>();
	Rumor->Init(Source);

	// Name Mutation
	if (FMath::FRand() < MutationChance)
	{
		TArray<FEnemyTableRow*> Rows;
		Enemies->GetAllRows(FString("GetAllRowsForNameMutation"), Rows);
		FEnemyTableRow* RandomRow;
		do
		{
			RandomRow = Rows[FMath::RandRange(0, Rows.Num() - 1)];
		} while (RandomRow->Name.EqualTo(Source->GetPerilName()));
		Rumor->OverrideName(RandomRow->Name);
		UE_LOG(LogTemp, Display, TEXT("Mutated name from %s to %s"), *Source->GetPerilName().ToString(), *Rumor->GetName().ToString());
	}

	// Power Mutation
	if (FMath::FRand() < MutationChance)
	{
		std::default_random_engine generator;
		std::normal_distribution<float> normaldist(0, 2); // Mean, StdDev
		int32 Modification = 0;
		while (Modification == 0) // Discard zero changes
		{
			Modification = FMath::RoundFromZero(normaldist(generator));
		}
		Rumor->OverridePower(FMath::Max(0, Source->GetPower() + Modification)); // Max ensures it is not negative.
		UE_LOG(LogTemp, Display, TEXT("Mutated power from %d to %d"), Source->GetPower(), Rumor->GetPower());
	}

	// Hex Mutation
	if (FMath::FRand() < MutationChance)
	{
		FHex Location = Source->GetHex();
		TArray<FHex> Neighbors = UHexBlueprintLibrary::FindNeighbors(Location);
		Rumor->OverrideHex(Neighbors[FMath::RandRange(0, Neighbors.Num() - 1)]);
		UE_LOG(LogTemp, Display, TEXT("Mutated hex from %d,%d to %d,%d"), Source->GetHex().Q, Source->GetHex().R, Rumor->GetHex().Q, Rumor->GetHex().R);
	}

	// Size Mutation
	if (FMath::FRand() < MutationChance)
	{
		float Roll = FMath::FRand();
		switch (Source->GetSize())
		{
		case EPerilSize::Small:
			Rumor->OverrideSize(Roll < 0.9 ? EPerilSize::Medium : EPerilSize::Large);
			break;
		case EPerilSize::Medium:
			Rumor->OverrideSize(Roll < 0.5 ? EPerilSize::Small : EPerilSize::Large);
			break;
		case EPerilSize::Large:
			Rumor->OverrideSize(Roll < 0.1 ? EPerilSize::Small : EPerilSize::Medium);
			break;
		default:
			UE_LOG(LogTemp, Warning, TEXT("Should not reach here."));
		}
		UE_LOG(LogTemp, Display, TEXT("Mutated size from %s to %s"), *GETENUMSTRING("EPerilSize", Source->GetSize()), *GETENUMSTRING("EPerilSize", Rumor->GetSize()));
	}

	// Type mutation
	if (FMath::FRand() < MutationChance)
	{
		Rumor->OverrideType(Source->GetType() == EPerilType::Strength ? EPerilType::Spirit : EPerilType::Strength);
	}

	return Rumor;
}

void ASplitThePartyGameMode::FinishHeroTurn()
{
	if (CurrentHeroIndex == Heroes.Num() - 1)
	{
		Round++;

		// Update the perils
		for (auto Tile : WorldMap->GetTiles())
		{
			if (Tile->Peril)
			{
				Tile->Peril->UpdateForEndOfRound();
			}
		}

		// If updating the perils ended the game, just short-circuit out.
		if (DefilableTilesRemaining == 0)
		{
			return;
		}

		// Update the agents already on the board.
		// This may mark some for removal in the AgentsToRemove array.
		for (auto Agent : Agents)
		{
			Agent->Update();
		}
		for (auto Agent : AgentsToRemoveDuringUpdate)
		{
			Agents.Remove(Agent);
			UE_LOG(LogTemp, Display, TEXT("Permanently removing agent %s, there are now %d left."), *Agent->Name.ToString(), Agents.Num());
		}
		AgentsToRemoveDuringUpdate.Empty();

		RoundsUntilNextPerilGeneration--;
		if (RoundsUntilNextPerilGeneration <= 0)
		{
			UPeril* Peril = GeneratePeril();
			UAgent* Agent = AgentGenerator->GenerateAgent();

			Agent->OnAgentMoved.AddDynamic(this, &ASplitThePartyGameMode::OnAgentMoved);
			URumor* Rumor = CreateRumor(Peril, EnemyTable, 0.2);
			Agent->SetRumor(Rumor);
			Agent->TileRestRounds = 1;
			Agent->Hex = Peril->GetHex(); 
			Agents.Emplace(Agent);
			UE_LOG(LogTemp, Display, TEXT("Generated agent %s at %d, %d"), *Agent->Name.ToString(), Agent->Hex.Q, Agent->Hex.R);

			RoundsUntilNextPerilGeneration = RoundsBetweenPerilGeneration;
		}
		
		OnRoundChange.Broadcast(Round);
	}

	// Determine the next hero
	CurrentHeroIndex = (CurrentHeroIndex + 1) % Heroes.Num();
	CurrentHero = Heroes[CurrentHeroIndex];

	// If the current hero already acted, then just loop around again.
	if (CurrentHero->ActedThisRound())
	{
		FinishHeroTurn();
	}
	else {
		StartTurn();
		OnHeroTurnStart.Broadcast(CurrentHero);
	}
}

void ASplitThePartyGameMode::OnTileDefiled(AHexTile * Tile)
{
	--DefilableTilesRemaining;
	if (DefilableTilesRemaining <= 0)
	{
		EndGame();
	}
}

void ASplitThePartyGameMode::OnHeroMovementComplete_Implementation()
{
	TArray<UAgent*> AgentsAtTile = GetAgentsAt(CurrentHero->GetTile());
	for (auto Agent : AgentsAtTile)
	{
		MarkReportedPeril(Agent);
		Agents.Remove(Agent);
	}
}

AWorldMap * ASplitThePartyGameMode::GetWorldMap()
{
	return WorldMap;
}

void ASplitThePartyGameMode::SetupTestMap()
{
	// A tile to place the sample peril
	AHexTile* Tile = WorldMap->GetTile(FHex(0, 1));

	// Generate the actual peril and the rumored peril.
	UPeril* Peril = GeneratePeril(EPerilGenerationBounds::InnerCircleOnly);

	// Create an agent who reports on a peril at the capital.
	UAgent* Agent = AgentGenerator->GenerateAgent();
	Agent->Hex = FHex(0,0); // Already at the capital.
	Agent->TileRestRounds = 0; // Immediately report.
	Agents.Add(Agent);

	// Make the rumor
	const float ChanceOfRumorMutation = 0.2;
	URumor* Rumor = CreateRumor(Peril, EnemyTable, ChanceOfRumorMutation);
	
	// Tell the agent about his rumor
	Agent->SetRumor(Rumor);
	UE_LOG(LogTemp, Display, TEXT("The rumor is of %s"), *Agent->GetRumor()->GetName().ToString())

	// Force a call as if the agent had just moved to the capital.
	OnAgentMoved(Agent);

	StartTurn();
}

AHexTile* ChooseRandomTile(EPerilGenerationBounds Bounds, AWorldMap* WorldMap)
{
	TArray<AHexTile*> Options;
	TArray<AHexTile*> Tiles = WorldMap->GetTiles();
	for (auto Tile : Tiles)
	{
		FHex TileCoordinates = Tile->Coordinates;
		int32 DistanceToCenter = UHexBlueprintLibrary::Distance(TileCoordinates, FHex(0, 0));
		if ((Bounds == EPerilGenerationBounds::InnerCircleOnly || Bounds == EPerilGenerationBounds::AnyCircle)
			&& (DistanceToCenter == 1))
		{
			Options.Add(Tile);
		}
		else if ((Bounds == EPerilGenerationBounds::OuterCircleOnly || Bounds == EPerilGenerationBounds::AnyCircle)
			&& (DistanceToCenter == 2))
		{
			Options.Add(Tile);
		}
	}
	AHexTile* SelectedTile = Options[FMath::RandRange(0, Options.Num()-1)];
	return SelectedTile;
}

UPeril * ASplitThePartyGameMode::GeneratePeril(EPerilGenerationBounds Bounds /*= EPerilGenerationBounds::AnyCircle*/)
{
	checkf(EnemyTable, TEXT("Enemy table has not been specified."));

	// Choose a tile that does not already have a peril and is not defiled.
	AHexTile* Tile;
	do 
	{
		Tile = ChooseRandomTile(Bounds, WorldMap);
	} while (Tile->Peril || Tile->IsDefiled());

	TArray<FEnemyTableRow*> Rows;
	EnemyTable->GetAllRows(FString("GetAllRows"), Rows);
	FEnemyTableRow* Enemy = Rows[FMath::RandRange(0, Rows.Num() - 1)];

	UPeril* Peril = NewObject<UPeril>(this, *Enemy->Name.ToString());
	Peril->SetPerilName(Enemy->Name);
	Peril->SetStrength(Enemy->Power);
	Peril->SetSize(Enemy->Size);
	Peril->SetHex(Tile->Coordinates);
	Peril->SetType(Enemy->Type);
	Tile->Peril = Peril;
	UE_LOG(LogTemp, Log, TEXT("Generated an enemy %s"), *Peril->GetPerilName().ToString());

	return Peril;
}

void ASplitThePartyGameMode::OnAgentMoved(UAgent * Agent)
{
	checkf(Agent, TEXT("Agent may not be null"));
	UE_LOG(LogTemp, Display, TEXT("Agent %s moved to %d,%d"), *Agent->Name.ToString(), Agent->Hex.Q, Agent->Hex.R);

	// Check if the agent reached the capital.
	if (Agent->Hex == FHex(0, 0))
	{
		// Notify via delegate of the report and stop tracking the agent.
		MarkReportedPeril(Agent);
		AgentsToRemoveDuringUpdate.Add(Agent);
	}
	else {
		// Check if a hero is at the location of a hero
		for (auto Hero : Heroes)
		{
			FHex HeroLocation = Hero->GetTile()->Coordinates;
			if (Agent->Hex == HeroLocation)
			{
				MarkReportedPeril(Agent);
				AgentsToRemoveDuringUpdate.Add(Agent);
				return; // If this agent hit one hero, we're done here.
			}
		}
	}
}

void ASplitThePartyGameMode::MarkReportedPeril(UAgent * ReportingAgent)
{
	checkf(PerilMarkerClass, TEXT("Peril marker class must be specified"));

	FHex Hex = ReportingAgent->GetRumor()->GetHex();

	// See if there is already a peril marker for the given hex
	for (auto PerilMarker : PerilMarkers)
	{
		if (PerilMarker->GetReportingAgents()[0]->GetRumor()->GetHex() == Hex)
		{
			PerilMarker->AddReportingAgent(ReportingAgent);
			CastChecked<ASplitThePartyPlayerController>(GetWorld()->GetFirstPlayerController())->ShowRumorReport(ReportingAgent);
			return;
		}
	}
	
	// If we got this far, then it's a new report marker required.
	AHexTile* Tile = WorldMap->GetTile(Hex);
	FTransform PerilMarkerTransform = Tile->GetPerilMarkerSocketTransform();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APerilMarker* PerilMarker = GetWorld()->SpawnActor<APerilMarker>(PerilMarkerClass, PerilMarkerTransform, SpawnParameters);
	PerilMarker->AddReportingAgent(ReportingAgent);
	PerilMarkers.Emplace(PerilMarker);

	CastChecked<ASplitThePartyPlayerController>(GetWorld()->GetFirstPlayerController())->ShowRumorReport(ReportingAgent);
}

TArray<AHero*> ASplitThePartyGameMode::GetHeroesAt(AHexTile * Tile)
{
	TArray<AHero*> HeroesOnTile;
	for (auto Hero : Heroes)
	{
		if (Hero->GetTile() == Tile)
		{
			HeroesOnTile.Add(Hero);
		}
	}
	return HeroesOnTile;
}

TArray<UAgent*> ASplitThePartyGameMode::GetAgentsAt(AHexTile * Tile)
{
	checkf(Tile, TEXT("Tile may not be null"));
	TArray<UAgent*> Result;
	for (auto Agent : Agents)
	{
		if (Agent->Hex == Tile->Coordinates)
		{
			Result.Add(Agent);
		}
	}
	return Result;
}

AHexTile * ASplitThePartyGameMode::GetTile(FHex Hex)
{
	return WorldMap->GetTile(Hex);
}

void ASplitThePartyGameMode::RemovePeril(UPeril * Peril)
{
	check(Peril);
	UE_LOG(LogTemp, Display, TEXT("Removing peril %s"), *Peril->GetPerilName().ToString());

	// Find the tile that has this peril and null it out.
	for (auto Tile : WorldMap->GetTiles())
	{
		if (Tile->Peril == Peril)
		{
			UE_LOG(LogTemp, Display, TEXT("Found peril at %s and nulling it out"), *Tile->GetTileName().ToString());
			Tile->Peril = nullptr;
			break;
		}
	}

	// Go through all the peril markers and remove the ones relevant to that peril.
	TArray<APerilMarker*> MarkersToRemove;
	for (auto PerilMarker : PerilMarkers)
	{
		UE_LOG(LogTemp, Display, TEXT("This is one marker with %d agents."), PerilMarker->GetReportingAgents().Num());
		for (auto Agent : Agents)
		{
			UE_LOG(LogTemp, Display, TEXT("%s reports on %s"), *Agent->Name.ToString(), *Agent->GetRumor()->GetPeril()->GetPerilName().ToString());
		}

		PerilMarker->RemoveAgentsReferencing(Peril);
		if (PerilMarker->IsEmpty())
		{
			UE_LOG(LogTemp, Display, TEXT("Marker is now empty and will be removed."));
			MarkersToRemove.Add(PerilMarker);
		}
	}
	for (auto PerilMarker : MarkersToRemove)
	{
		PerilMarkers.Remove(PerilMarker);
		PerilMarker->Remove();
	}

	// The peril object itself can be deallocated now, but there should be no more
	// references to it, so the gc should get it.
}
