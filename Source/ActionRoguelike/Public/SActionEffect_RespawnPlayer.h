// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SActionEffect.h"
#include "SActionEffect_RespawnPlayer.generated.h"


class USDefaultMenuWidget;

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USActionEffect_RespawnPlayer : public USActionEffect
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadOnly)
	TArray<APlayerController*> RespawnablePlayers;

protected:

	void StartAction_Implementation(AActor* Instigator) override;

	UFUNCTION(BlueprintCallable)
	void RespawnSelectedPlayer(AController* BuyingController, APlayerState* TargetPlayerState);

	UFUNCTION(BlueprintCallable)
	void CancelRespawn(AController* BuyingController);

	void CreateRespawnSelectorWidget(AActor* Instigator);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USDefaultMenuWidget> RespawnWidgetClass;

	UPROPERTY()
	USDefaultMenuWidget* RespawnWidgetInstance;
};
