// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTaskNode_FindHidingLocation.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API USBTTaskNode_FindHidingLocation : public UBTTaskNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "CompanionAI")
	FBlackboardKeySelector HideLocationKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	void SortCoverPointsByActorDistance(TArray<AActor*>& CoverPoints, AActor* TargetActor);

	bool IsValidCoverPoint(AActor* CoverPoint);
	
};
