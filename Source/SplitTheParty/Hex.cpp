// Fill out your copyright notice in the Description page of Project Settings.

#include "Hex.h"

FHex::FHex()
	: Q(0)
	, R(0) {
}

FHex::FHex(int32 Q_, int32 R_) 
 : Q(Q_)
 , R(R_) {
}

TArray<FHex> UHexBlueprintLibrary::FindNeighbors(const FHex & Hex)
{
	const FHex Directions[] = {
		FHex(+1,0), FHex(+1,-1), FHex(0,-1),
		FHex(-1,0), FHex(-1,+1), FHex(0,+1)
	};
	TArray<FHex> Result;
	for (int i = 0; i < 6; i++)
	{
		Result.Add(FHex(Hex + Directions[i]));
	}
	return Result;
}

int32 UHexBlueprintLibrary::Distance(const FHex A, const FHex B)
{
	return (abs(A.Q - B.Q)
		+ abs(A.Q + A.R - B.Q - B.R)
		+ abs(A.R - B.R)) / 2;
}

FVector2D UHexBlueprintLibrary::HexToUnitVector(const FHex Hex)
{
	// See https://www.redblobgames.com/grids/hexagons/#hex-to-pixel
	float X = sqrt(3)*Hex.Q + sqrt(3) / 2 * Hex.R;
	float Y = 3. / 2 * Hex.R;
	return FVector2D(X, Y);
}
