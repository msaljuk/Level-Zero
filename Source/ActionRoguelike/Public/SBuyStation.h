// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SBuyStationItem.h"
#include "SBuyStation.generated.h"

class USAction;
class ASPlayerState;
class USBuyStationItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuyStationChanged, ASBuyStation*, BuyStation);

UCLASS()
class ACTIONROGUELIKE_API ASBuyStation : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASBuyStation();

	UPROPERTY(BlueprintReadOnly)
	TArray<USBuyStationItem*> BuyStationItems;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<USAction>> BuyStationActions;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	UFUNCTION(BlueprintCallable)
	void AddToBuyStation(FName Name, TEnumAsByte<ItemType> Type, TSubclassOf<USAction> ActionClass, int Cost = 0, bool IsAlreadyPurchased = false);

	UFUNCTION(BlueprintCallable)
	bool IsPurchasable(ASPlayerState* PlayerState, USBuyStationItem* BuyStationItem);

	UFUNCTION(BlueprintCallable)
	bool PurchaseItem(ASCharacter* PlayerCharacter, USBuyStationItem* BuyStationItem);

	UPROPERTY(BlueprintAssignable)
	FOnBuyStationChanged OnBuyStationChanged;
};
