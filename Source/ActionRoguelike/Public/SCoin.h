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

	void SetCoinCredits(int UpdatedCoinCredits);

	UPROPERTY(EditDefaultsOnly)
	bool bShouldDestroyAfterFirstUse;

	FText GetInteractText_Implementation(APawn* InstigatorPawn) override;

protected:

	UPROPERTY(EditDefaultsOnly)
	int CoinCredits;

	void GivePlayerCoinCredits(APawn* InstigatorPawn);
	
};

