// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerUp.h"
#include "SGameplayInterface.h"
#include "SHealthPotion.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASHealthPotion : public ASPowerUp, public ISGameplayInterface
{
	GENERATED_BODY()

public:

	ASHealthPotion();

	void Interact_Implementation(APawn* InstigatorPawn);

	FText GetInteractText_Implementation(APawn* InstigatorPawn) override;

protected:

	float PotionHealingValue;

	UPROPERTY(EditDefaultsOnly)
	int PotionCreditsCost;

	void Heal(APawn* InstigatorPawn);
};