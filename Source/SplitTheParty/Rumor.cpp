// Fill out your copyright notice in the Description page of Project Settings.

#include "Rumor.h"

void URumor::Init(UPeril* Source)
{
	checkf(Source, TEXT("Source cannot be null"));
	Peril = Source;
}

bool URumor::IsRumorAbout(UPeril * Peril_)
{
	checkf(Peril_, TEXT("Parameter may not be null"));
	UE_LOG(LogTemp, Display, TEXT("Peril %s is being compared to %s and the result is %s"), *Peril->GetPerilName().ToString(), *Peril_->GetPerilName().ToString(), (Peril == Peril_) ? TEXT("true") : TEXT("false"));
	return Peril_ == Peril;
}
