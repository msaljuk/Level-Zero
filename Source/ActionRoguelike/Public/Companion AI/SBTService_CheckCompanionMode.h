// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CheckCompanionMode.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USBTService_CheckCompanionMode : public UBTService
{
	GENERATED_BODY()

protected:


	UPROPERTY(EditAnywhere, Category = "CompanionAI")
	FBlackboardKeySelector CompanionModeKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
