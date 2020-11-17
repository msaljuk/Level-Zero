// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect_RespawnPlayer.h"
#include "SGameModeBase.h"
#include "SPlayerState.h"
#include "SDefaultMenuWidget.h"

void USActionEffect_RespawnPlayer::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	FindRespawnablePlayers();

	CreateRespawnSelectorWidget(Instigator);
}

void USActionEffect_RespawnPlayer::FindRespawnablePlayers()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();
		if (!PlayerController->GetPawn())
		{
			RespawnablePlayers.Add(PlayerController);
		}
	}
}

void USActionEffect_RespawnPlayer::CreateRespawnSelectorWidget(AActor* Instigator)
{
	APawn* MyPawn = Cast<APawn>(Instigator);
	if (MyPawn->IsLocallyControlled())
	{
		if (ensure(RespawnWidgetClass))
		{
			RespawnWidgetInstance = CreateWidget<USDefaultMenuWidget>(GetWorld(), RespawnWidgetClass);
			RespawnWidgetInstance->AttachedObject = this;
			RespawnWidgetInstance->AddToViewport();
		}
	}
}

void USActionEffect_RespawnPlayer::RespawnSelectedPlayer(AController* Controller)
{
	ASGameModeBase* GameMode = GetWorld()->GetAuthGameMode<ASGameModeBase>();
	if (GameMode)
	{
		GameMode->RespawnPlayer(Controller);
	}

	RespawnWidgetInstance->RemoveFromParent();
	RespawnWidgetInstance = nullptr;
}

void USActionEffect_RespawnPlayer::CancelRespawn(AController* BuyingController)
{
	ASPlayerState* PlayerState = BuyingController->GetPlayerState<ASPlayerState>();

	// Add Removed Credits Back
	if (PlayerState)
	{
		PlayerState->AddCredits(CreditsValue);
	}

	RespawnWidgetInstance->RemoveFromParent();
	RespawnWidgetInstance = nullptr;
}

