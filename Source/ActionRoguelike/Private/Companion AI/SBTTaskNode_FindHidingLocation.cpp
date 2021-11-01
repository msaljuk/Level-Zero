// Fill out your copyright notice in the Description page of Project Settings.


#include "Companion AI/SBTTaskNode_FindHidingLocation.h"
#include <Kismet/GameplayStatics.h>
#include "SWorldSettings.h"
#include <AIController.h>
#include "AI/SAIController.h"

EBTNodeResult::Type USBTTaskNode_FindHidingLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (ensure(AIController))
	{
		if (ensure(BlackboardComp))
		{
			AActor* TargetActor = Cast<AActor>(AIController->GetPawn());
			if (ensure(TargetActor))
			{
				ASWorldSettings* WorldSettings = Cast<ASWorldSettings>(UGameplayStatics::GetActorOfClass(GetWorld(), ASWorldSettings::StaticClass()));
				if (ensure(WorldSettings))
				{
					TArray<AActor*> CoverPoints = WorldSettings->CoverPoints;
					if (ensure(CoverPoints.Num() > 0))
					{
						SortCoverPointsByActorDistance(CoverPoints, TargetActor);

						for (AActor* CoverPoint : CoverPoints)
						{
							if (IsValidCoverPoint(CoverPoint))
							{
								BlackboardComp->SetValueAsVector(HideLocationKey.SelectedKeyName, CoverPoint->GetActorLocation());

								return EBTNodeResult::Succeeded;
							}
						}
					}
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}

void USBTTaskNode_FindHidingLocation::SortCoverPointsByActorDistance(TArray<AActor*>& CoverPoints, AActor* TargetActor)
{
	FVector SourceLocation = TargetActor->GetActorLocation();

	CoverPoints.Sort([&SourceLocation](const AActor& LHCoverPoint, const AActor& RHCoverPoint) {
		float DistanceToLHCoverPoint = FVector::DistSquared(SourceLocation, LHCoverPoint.GetActorLocation());
		float DistanceToRHCoverPoint = FVector::DistSquared(SourceLocation, RHCoverPoint.GetActorLocation());

		return DistanceToLHCoverPoint < DistanceToRHCoverPoint;
	});
}

bool USBTTaskNode_FindHidingLocation::IsValidCoverPoint(AActor* CoverPoint)
{
	TArray<AActor*> EnemyAIControllers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASAIController::StaticClass(), EnemyAIControllers);

	if (ensure(EnemyAIControllers.Num() > 0))
	{
		for (AActor* EnemyAIControllerActor : EnemyAIControllers)
		{
			ASAIController* EnemyAIController = Cast<ASAIController>(EnemyAIControllerActor);
			if (EnemyAIController->LineOfSightTo(CoverPoint)) {
				return false;
			}
		}
	}

	return true;
}


