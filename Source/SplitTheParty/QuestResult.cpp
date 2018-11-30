// Fill out your copyright notice in the Description page of Project Settings.

#include "QuestResult.h"
#include "Hero.h"

void UStaminaLossQuestEffect::Commit()
{
	checkf(Hero, TEXT("Hero is not specified!"));
	Hero->ReduceStamina(Reduction);
}

void UQuestResult::CommitEffects()
{
	for (auto Effect : Effects)
	{
		Effect->Commit();
	}
}
