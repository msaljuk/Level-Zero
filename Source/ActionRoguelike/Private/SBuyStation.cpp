// Fill out your copyright notice in the Description page of Project Settings.


#include "SBuyStation.h"
#include "SAction.h"
#include "SPlayerState.h"
#include "SCharacter.h"
#include "SActionComponent.h"

// Sets default values
ASBuyStation::ASBuyStation()
{
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
				FName ActionName = NewAction->ActionName;
				int ActionCreditsCost = NewAction->CreditsValue;

				AddToBuyStation(ActionName, ItemType::Attack, ActionClass, ActionCreditsCost);
			}
		}
	}
}

void ASBuyStation::AddToBuyStation(FName Name, TEnumAsByte<ItemType> Type, TSubclassOf<USAction> ActionClass, int Cost /*= 0*/, bool IsAlreadyPurchased /*= false*/)
{
	USBuyStationItem* NewItem = NewObject<USBuyStationItem>(this);

	NewItem->ItemName = Name;
	NewItem->BuyItemType = Type;
	NewItem->BuyItemAction = ActionClass;
	NewItem->CreditsRequiredToPurchase = Cost;
	NewItem->bIsAlreadyPurchased = IsAlreadyPurchased;

	BuyStationItems.Add(NewItem);
}

bool ASBuyStation::IsPurchasable(ASPlayerState* PlayerState, USBuyStationItem* BuyStationItem)
{
	if (PlayerState)
	{
		int ItemCost = BuyStationItem->CreditsRequiredToPurchase;

		int CurrentCredits = PlayerState->PlayerCredits;

		if (CurrentCredits - ItemCost >= 0 && !BuyStationItem->bIsAlreadyPurchased)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	
	return false;
}

bool ASBuyStation::PurchaseItem(ASCharacter* PlayerCharacter, USBuyStationItem* BuyStationItem)
{
	USActionComponent* ActionComp = Cast<USActionComponent>(PlayerCharacter->GetComponentByClass(USActionComponent::StaticClass()));

	if (ActionComp)
	{
		ASPlayerState* PlayerState = PlayerCharacter->GetPlayerState<ASPlayerState>();

		if (PlayerState)
		{
			ActionComp->AddAction(PlayerCharacter, BuyStationItem->BuyItemAction);

			int ItemCost = BuyStationItem->CreditsRequiredToPurchase;
			PlayerState->RemoveCredits(ItemCost);

			BuyStationItem->bIsAlreadyPurchased = true;

			OnBuyStationChanged.Broadcast(this);

			return true;
		}
	}

	return false;
}