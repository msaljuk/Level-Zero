// Fill out your copyright notice in the Description page of Project Settings.


#include "SCoin.h"
#include "SPlayerState.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

ASCoin::ASCoin()
{
	CoinCredits = 1;

	bShouldDestroyAfterFirstUse = false;
}

void ASCoin::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!bInteractDisabled)
	{
		DeactivateInteract();

		GivePlayerCoinCredits(InstigatorPawn);

		if (bShouldDestroyAfterFirstUse)
		{
			GetWorldTimerManager().ClearTimer(InteractTimer);

			Destroy();
		}
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

void ASCoin::SetCoinCredits(int UpdatedCoinCredits)
{
	CoinCredits = UpdatedCoinCredits;
}

FText ASCoin::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	return FText::Format(LOCTEXT("CoinCredits_InteractMessage", "Pickup {0} Credits"), CoinCredits);
}

#undef LOCTEXT_NAMESPACE 
