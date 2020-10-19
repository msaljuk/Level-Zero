// Fill out your copyright notice in the Description page of Project Settings.


#include "SCoin.h"
#include "SPlayerState.h"

ASCoin::ASCoin()
{
	CoinCredits = 1;
}

void ASCoin::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!bInteractDisabled)
	{
		DeactivateInteract();

		GivePlayerCoinCredits(InstigatorPawn);
	}
}

void ASCoin::GivePlayerCoinCredits(APawn* InstigatorPawn)
{
	if (InstigatorPawn)
	{
		ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();

		if (PlayerState)
		{
			// if Player Credits are less than max, give coin credits to player
			if (PlayerState->PlayerCredits < PlayerState->MaximumPlayerCredits)
			{
				PlayerState->AddCredits(CoinCredits);
			}
			// Otherwise, ignore interaction and enable Coin again
			else
			{
				GetWorldTimerManager().ClearTimer(InteractTimer);

				ActivateInteract();
			}
		}
	}
}

