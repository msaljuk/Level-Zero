// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SBTService_CheckAttackRange.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USBTService_CheckAttackRange : public UBTService
{
	GENERATED_BODY()
	
protected:

	bool PreviousPlayerPotentiallySeenKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector PlayerSeenKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector CompanionSeenKey;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

};
