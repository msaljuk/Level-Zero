// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_FindPatrolPoint.generated.h"

class ASAICharacter;

/**
 *
 */
UCLASS()
class ACTIONROGUELIKE_API USBTTask_FindPatrolPoint : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	AActor* CalculateNextPatrolPoint(ASAICharacter* AIPawn);

	void UpdateBlackboardPatrolPoint(AActor* NextPatrolPoint, UBehaviorTreeComponent& OwnerComp);

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector PatrolPointKey;
};
