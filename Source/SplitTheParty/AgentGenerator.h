// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "AgentGenerator.generated.h"

class UAgent;

/**
 * Factory class for UAgent objects.
 */
UCLASS(BlueprintType,Blueprintable)
class SPLITTHEPARTY_API UAgentGenerator : public UObject
{
	GENERATED_BODY()

public:
	UAgent* GenerateAgent();
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UDataTable* Names;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	UDataTable* Roles;
};

UENUM(BlueprintType)
enum class EAgentGender : uint8
{
	Male, 
	Female
};

// A row of names.
USTRUCT(BlueprintType)
struct FAgentNameRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FText Name;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EAgentGender Gender;
};

// Indicates the gender that can use a certain role.
UENUM(BlueprintType)
enum class EGenderRole : uint8
{
	Male,
	Female,
	Either
};

// A row that specifies a role an agent can have.
USTRUCT(BlueprintType)
struct FAgentRoleRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FText Role;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EGenderRole Gender;
};