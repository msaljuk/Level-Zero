// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "SSaveGame.h"

ASPlayerState::ASPlayerState()
{
	PlayerCredits = 100;

	MaximumPlayerCredits = 100;

	bIsPlayerAlive = true;
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

void ASPlayerState::ClientUpdateCredits(int NumCredits)
{
	ServerUpdateCredits(NumCredits);
}

void ASPlayerState::ServerUpdateCredits_Implementation(int NumCredits)
{
	AddCredits(NumCredits);
}

void ASPlayerState::ServerToggleIsAlive_Implementation()
{
	bIsPlayerAlive = !bIsPlayerAlive;
}

int ASPlayerState::GetCredits()
{
	return PlayerCredits;
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveGame)
{
	if (SaveGame)
	{
		SaveGame->Credits = PlayerCredits;
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveGame)
{
	if (SaveGame)
	{
		AddCredits(SaveGame->Credits);
	}

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
	DOREPLIFETIME(ASPlayerState, bIsPlayerAlive);
}