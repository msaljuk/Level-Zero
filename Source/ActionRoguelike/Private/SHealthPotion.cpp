// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"
#include "SAtttributeComponent.h"

ASHealthPotion::ASHealthPotion() {
	PotionHealingValue = 30.0f;
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
		USAtttributeComponent* AttributeComp = Cast<USAtttributeComponent>(InstigatorPawn->GetComponentByClass(USAtttributeComponent::StaticClass()));

		if (AttributeComp)
		{
			float CurrentHealth = AttributeComp->Health;

			if (CurrentHealth < AttributeComp->HealthMax)
			{
				AttributeComp->ApplyHealthChange(PotionHealingValue);
			}
			// if Health already at Max, ignore interaction and enable Potion again
			else
			{
				GetWorldTimerManager().ClearTimer(InteractTimer);

				ActivateInteract();
			}
		}
	}
}