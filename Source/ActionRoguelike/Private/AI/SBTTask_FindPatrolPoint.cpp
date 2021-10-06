// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/SBTTask_FindPatrolPoint.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/SAICharacter.h"

const float SMALL_DELTA = 50.0f;

EBTNodeResult::Type USBTTask_FindPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AAIController* AIController = OwnerComp.GetAIOwner();

	if (ensure(AIController))
	{
		ASAICharacter* AIPawn = Cast<ASAICharacter>(AIController->GetPawn());
		if (AIPawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		AActor* NextPatrolPoint = CalculateNextPatrolPoint(AIPawn);
		if (ensure(NextPatrolPoint))
		{
			UpdateBlackboardPatrolPoint(NextPatrolPoint, OwnerComp);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}

AActor* USBTTask_FindPatrolPoint::CalculateNextPatrolPoint(ASAICharacter* AIPawn)
{
	// retrieve list of marked patrol points for given NPC
	TArray<AActor*> PatrolPoints = AIPawn->PatrolPoints;

	if (PatrolPoints.Num() > 0)
	{
		// pick next patrol point at random
		AActor* NextPatrolPoint = PatrolPoints[rand() % PatrolPoints.Num()];

		// confirm that AI is not already at patrol point
		bool bIsNextPatrolPointConfirmed = false;
		FVector PawnLocation = AIPawn->GetActorLocation();

		while (!bIsNextPatrolPointConfirmed)
		{
			if (FVector::Dist(PawnLocation, NextPatrolPoint->GetActorLocation()) < SMALL_DELTA) {
				NextPatrolPoint = PatrolPoints[rand() % PatrolPoints.Num()];
			}
			else
			{
				bIsNextPatrolPointConfirmed = true;
			}
		}

		return NextPatrolPoint;
	}

	return NULL;
}

void USBTTask_FindPatrolPoint::UpdateBlackboardPatrolPoint(AActor* NextPatrolPoint, UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (ensure(BlackboardComp))
	{
		BlackboardComp->SetValueAsVector(PatrolPointKey.SelectedKeyName, NextPatrolPoint->GetActorLocation());
	}
}
