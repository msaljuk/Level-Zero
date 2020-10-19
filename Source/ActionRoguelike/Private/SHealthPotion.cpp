// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"
#include "SAtttributeComponent.h"
#include "SPlayerState.h"

ASHealthPotion::ASHealthPotion() 
{
	PotionHealingValue = 30.0f;

	PotionCreditsCost = 2;
}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn) 
{
	if (!bInteractDisabled)
	{
		DeactivateInteract();

		Heal(InstigatorPawn);
	}
}

void ASHealthPotion::Heal(APawn* InstigatorPawn)
{
	if (InstigatorPawn)
	{
		USAtttributeComponent* AttributeComp = USAtttributeComponent::GetAttributes(InstigatorPawn);

		ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>();

		if (AttributeComp && PlayerState)
		{
			float CurrentHealth = AttributeComp->Health;
			
			int PlayerCredits = PlayerState->PlayerCredits;
			
			// if Health is not at Max (Player is heal-able) AND Player has enough credits to use potion
			if (CurrentHealth < AttributeComp->HealthMax && PlayerCredits >= PotionCreditsCost)
			{
				AttributeComp->ApplyHealthChange(this, PotionHealingValue);

				PlayerState->RemoveCredits(PotionCreditsCost);
			}
			// if Health already at Max or Player does not have enough credits, ignore interaction and enable Potion again
			else
			{
				GetWorldTimerManager().ClearTimer(InteractTimer);

				ActivateInteract();
			}
		}
	}
}