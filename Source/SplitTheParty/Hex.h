// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Hex.generated.h"

USTRUCT(BlueprintType)
struct FHex
{
	GENERATED_BODY()

	// Q Coordinate.
	// Semantically a constant, but must be EditAnywhere,BlueprintReadWrite for BP support.
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Coordinates)
	int32 Q;

	// R Coordinate. 
	// Semantically a constant, but must be EditAnywhere,BlueprintReadWrite for BP support.
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Coordinates)
	int32 R;

	FHex();

	FHex(int32 Q, int32 R);

	FORCEINLINE bool operator== (const FHex& Other) const
	{
		return Q == Other.Q && R == Other.R;
	}

	FORCEINLINE bool operator!= (const FHex& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE FHex operator+(const FHex& Other) const
	{
		return FHex(Q + Other.Q, R + Other.R);
	}

	// Get the hash for a FHex.
	// Algorithm taken from https://www.redblobgames.com/grids/hexagons/implementation.html
	friend FORCEINLINE uint32 GetTypeHash(const FHex& Hex)
	{
		int32 QHash = GetTypeHash(Hex.Q);
		int32 RHash = GetTypeHash(Hex.R);
		return QHash ^ (RHash + 0x9e3779b9 + (QHash << 6) + (QHash >> 2));
	}
};

UCLASS()
class SPLITTHEPARTY_API UHexBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Find the neighbors of the given hex.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Hex Utils")
	static TArray<FHex> FindNeighbors(const FHex& Hex);

	// Determine the distance between two hex coordinates.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Hex Utils")
	static int32 Distance(const FHex A, const FHex B);

	// Compute the unit vector pointing in the direction the coordinate is from the center
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Hex Utils")
	static FVector2D HexToUnitVector(const FHex Hex);

	// Custom '==' node in blueprint
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal FHex", CompactNodeTitle = "==", Keywords = "= equal"), Category = "Math")
	static bool EqualEqual_FHexFHex(const FHex &A, const FHex &B) { return A == B; }
};