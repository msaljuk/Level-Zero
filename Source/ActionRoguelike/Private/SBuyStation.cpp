// Fill out your copyright notice in the Description page of Project Settings.


#include "SBuyStation.h"
#include "SAction.h"
#include "SPlayerState.h"
#include "SCharacter.h"
#include "SActionComponent.h"
#include "SActionEffect.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASBuyStation::ASBuyStation()
{
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASBuyStation::BeginPlay()
{
	Super::BeginPlay();

	for (TSubclassOf<USAction> ActionClass : BuyStationActions)
	{
		if (ActionClass)
		{
			USAction* NewAction = NewObject<USAction>(this, ActionClass);

			if (ensure(NewAction))
			{
				// set default duration to be negative i.e. permanent purchase
				float ActionDuration = -1.0f;

				if (NewAction->IsA(USActionEffect::StaticClass()))
				{
					USActionEffect* NewActionEffect = Cast<USActionEffect>(NewAction);

					if (NewActionEffect)
					{
						ActionDuration = NewActionEffect->GetDuration();
					}
				}

				AddToBuyStation(NewAction->ActionName, NewAction->ActionType, ActionClass, NewAction->CreditsValue, ActionDuration);
			}
		}
	}
}

void ASBuyStation::AddToBuyStation(FName Name, TEnumAsByte<ItemType> Type, TSubclassOf<USAction> ActionClass, int Cost /*= 0*/, float Duration /*= -1.0f */, bool IsAlreadyPurchased /*= false*/)
{
	USBuyStationItem* NewItem = NewObject<USBuyStationItem>(this);

	NewItem->ItemName = Name;
	NewItem->BuyItemType = Type;
	NewItem->BuyItemActionClass = ActionClass;
	NewItem->CreditsRequiredToPurchase = Cost;
	NewItem->Duration = Duration;
	NewItem->bIsAlreadyPurchased = IsAlreadyPurchased;

	BuyStationItems.Add(NewItem);
}

bool ASBuyStation::IsPurchasable(ASPlayerState* PlayerState, USBuyStationItem* BuyStationItem)
{
	if (PlayerState)
	{
		int ItemCost = BuyStationItem->CreditsRequiredToPurchase;

		int CurrentCredits = PlayerState->PlayerCredits;

		if (CurrentCredits - ItemCost >= 0)
		{
			if (!BuyStationItem->bIsAlreadyPurchased)
			{
				return true;
			}
		}
	}
	
	return false;
}

void ASBuyStation::PurchaseItem(ASCharacter* PlayerCharacter, USBuyStationItem* BuyStationItem)
{

	USActionComponent* ActionComp = Cast<USActionComponent>(PlayerCharacter->GetComponentByClass(USActionComponent::StaticClass()));

	if (ensure(ActionComp))
	{

		ASPlayerState* PlayerState = PlayerCharacter->GetPlayerState<ASPlayerState>();

		if (ensure(PlayerState))
		{
			ActionComp->ClientAddAction(PlayerCharacter, BuyStationItem->BuyItemActionClass);

			PlayerState->ClientUpdateCredits(-BuyStationItem->CreditsRequiredToPurchase);

			BuyStationItem->bIsAlreadyPurchased = true;

			// If ActionEffect, set timer to reset purchase status after effect ends
			bool bIsActionEffect = UKismetMathLibrary::ClassIsChildOf(BuyStationItem->BuyItemActionClass, USActionEffect::StaticClass());
			if (bIsActionEffect)
			{
				FTimerHandle TimerHandle_ActionEffectStopped;

				FTimerDelegate Delegate;
				Delegate.BindUFunction(this, "ResetPurchaseStatus", BuyStationItem);

				GetWorld()->GetTimerManager().SetTimer(TimerHandle_ActionEffectStopped, Delegate, BuyStationItem->Duration, false);
			}

			OnBuyStationChanged.Broadcast(this);
		}
	}
}

void ASBuyStation::ResetPurchaseStatus(USBuyStationItem* BuyStationItem)
{
	BuyStationItem->bIsAlreadyPurchased = false;

	OnBuyStationChanged.Broadcast(this);
}
