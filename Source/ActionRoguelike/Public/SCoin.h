// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerUp.h"
#include "SCoin.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASCoin : public ASPowerUp, public ISGameplayInterface
{
	GENERATED_BODY()

public:

	ASCoin();

	void Interact_Implementation(APawn* InstigatorPawn);

protected:

	UPROPERTY(EditDefaultsOnly)
	int CoinCredits;

	void GivePlayerCoinCredits(APawn* InstigatorPawn);
	
};

