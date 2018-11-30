// Fill out your copyright notice in the Description page of Project Settings.

#include "AgentGenerator.h"
#include "Agent.h"

bool GenderMatch(EAgentGender AgentGender, EGenderRole RoleGender)
{
	if (RoleGender == EGenderRole::Either)
	{
		return true;
	}
	else
	{
		return (AgentGender == EAgentGender::Male && RoleGender == EGenderRole::Male)
			|| (AgentGender == EAgentGender::Female && RoleGender == EGenderRole::Female);
	}
}

UAgent* UAgentGenerator::GenerateAgent()
{
	checkf(Names, TEXT("Names data table must be specified"));
	checkf(Roles, TEXT("Roles data table must be specified"));
	TArray<FAgentNameRow*> NameArray;
	Names->GetAllRows(FString("GetAllNames"), NameArray);
	FAgentNameRow* ChosenNameRow = NameArray[FMath::RandRange(0, NameArray.Num() - 1)];

	// Find the role for this person based on their gender.
	EAgentGender Gender = ChosenNameRow->Gender;
	TArray<FAgentRoleRow*> RoleArray;
	Roles->GetAllRows(FString("GetAllRoles"), RoleArray);
	TArray<FText> PotentialRoles;
	for (auto Row : RoleArray)
	{
		if (GenderMatch(Gender, Row->Gender))
		{
			PotentialRoles.Add(Row->Role);
		}
	}

	FText ChosenRole = PotentialRoles[FMath::RandRange(0, PotentialRoles.Num() - 1)];
	FText Name = FText::FromString(ChosenNameRow->Name.ToString() + " the " + ChosenRole.ToString());

	UAgent* Agent = NewObject<UAgent>(this, FName(*Name.ToString()));
	Agent->Name = Name;
	return Agent;
}
