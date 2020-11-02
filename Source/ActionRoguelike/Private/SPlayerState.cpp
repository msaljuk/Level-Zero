// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"

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

	if (ActualDelta != 0)
	{
		MulticastPlayerCreditsChange(GetPawn(), NewCredits, ActualDelta);
	}
}

void ASPlayerState::RemoveCredits(int NumCreditsToRemove)
{
	AddCredits(-NumCreditsToRemove);
}


int ASPlayerState::GetCredits()
{
	return PlayerCredits;
}

void ASPlayerState::MulticastPlayerCreditsChange_Implementation(APawn* Pawn, int NewCredits, int Delta)
{	
	OnCreditsChanged.Broadcast(Pawn, NewCredits, Delta);
}


void ASPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, PlayerCredits);
	DOREPLIFETIME(ASPlayerState, MaximumPlayerCredits);
}