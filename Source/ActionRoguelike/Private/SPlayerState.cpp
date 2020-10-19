// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

ASPlayerState::ASPlayerState()
{
	PlayerCredits = 0;

	MaximumPlayerCredits = 100;
}

void ASPlayerState::AddCredits(int NumCreditsToAdd)
{
	int NewCredits = FMath::Clamp(PlayerCredits + NumCreditsToAdd, 0, MaximumPlayerCredits);

	int ActualDelta = NewCredits - PlayerCredits;

	PlayerCredits = NewCredits;

	OnCreditsChanged.Broadcast(GetPawn(), PlayerCredits, ActualDelta);
}

void ASPlayerState::RemoveCredits(int NumCreditsToRemove)
{
	AddCredits(-NumCreditsToRemove);
}
