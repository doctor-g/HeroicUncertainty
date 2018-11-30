// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Peril.h"
#include "EnemyTableRow.generated.h"

USTRUCT(BlueprintType)
struct FEnemyTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	FText Name;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EPerilSize Size;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	int32 Power;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	EPerilType Type;
};