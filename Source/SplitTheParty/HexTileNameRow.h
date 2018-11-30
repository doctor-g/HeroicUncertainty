// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Hex.h"
#include "HexTileNameRow.generated.h"

USTRUCT(BlueprintType)
struct FHexTileNameRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite,VisibleInstanceOnly)
	FHex Hex;

	UPROPERTY(BlueprintReadWrite,VisibleInstanceOnly)
	FText Name;
};