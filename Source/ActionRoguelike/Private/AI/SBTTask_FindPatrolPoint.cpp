// Fill out your copyright notice in the Description page of Project Settings.
#include "AI/SBTTask_FindPatrolPoint.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/SAICharacter.h"

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
				if (FVector::Dist(PawnLocation, NextPatrolPoint->GetActorLocation()) < 50.0f) {
					NextPatrolPoint = PatrolPoints[rand() % PatrolPoints.Num()];
				}
				else
				{
					bIsNextPatrolPointConfirmed = true;
				}
			}

			UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
			if (ensure(BlackboardComp))
			{
				BlackboardComp->SetValueAsVector(PatrolPointKey.SelectedKeyName, NextPatrolPoint->GetActorLocation());
			}

			return EBTNodeResult::Succeeded;
		}

		
	}

	return EBTNodeResult::Failed;
}
