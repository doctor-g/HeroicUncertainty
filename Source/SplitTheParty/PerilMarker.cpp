// Fill out your copyright notice in the Description page of Project Settings.

#include "PerilMarker.h"
#include "Engine/World.h"
#include "Agent.h"
#include "Rumor.h"

// Sets default values
APerilMarker::APerilMarker()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APerilMarker::AddReportingAgent(UAgent * Agent)
{
	checkf(Agent, TEXT("Parameter may not be null"));
	UE_LOG(LogTemp, Display, TEXT("Adding reporting agent to peril marker: %s, who carries a rumor about %s"), *Agent->Name.ToString(), *Agent->GetRumor()->GetPeril()->GetPerilName().ToString());
	Agents.Emplace(Agent);
}

TArray<UAgent*> APerilMarker::GetReportingAgents()
{
	return Agents;
}

void APerilMarker::RemoveAgentsReferencing(UPeril * Peril)
{
	UE_LOG(LogTemp, Display, TEXT("In the marker of the rumor of %s about %s, comparing against the peril being removed, %s"),
		*(Agents[0]->Name.ToString()),
		*(Agents[0]->GetRumor()->GetPeril()->GetPerilName().ToString()),
		*Peril->GetPerilName().ToString());

	TArray<UAgent*> AgentsToRemove;
	for (auto Agent : Agents)
	{
		if (Agent->GetRumor()->GetPeril() == Peril)
		{
			AgentsToRemove.Emplace(Agent);
		}
	}
	for (auto Agent : AgentsToRemove)
	{
		Agents.Remove(Agent);
	}
}

bool APerilMarker::IsEmpty()
{
	return Agents.Num() == 0;
}

void APerilMarker::Remove()
{
	PlayRemovalAnimation();
}

void APerilMarker::NotifyAnimationComplete()
{
	Destroy();
}
