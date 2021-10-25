// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "SCharacter.h"
#include "Companion AI/SCompanionAIController.h"

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Check distance between AIPlayer and Player
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (ensure(BlackboardComp))
	{
		ASCharacter* TargetCharacter = Cast<ASCharacter>(BlackboardComp->GetValueAsObject("TargetActor"));

		if (TargetCharacter)
		{
			AAIController* MyController = OwnerComp.GetAIOwner();

			if (ensure(MyController))
			{
				APawn* AIPawn = MyController->GetPawn();
				if (ensure(AIPawn))
				{
					float DistanceTo = FVector::Distance(TargetCharacter->GetActorLocation(), AIPawn->GetActorLocation());

					bool bWithinRange = DistanceTo < 1000.0f;

					if (bWithinRange)
					{
						bool bHasLineOfSight = MyController->LineOfSightTo(TargetCharacter);

						if (bHasLineOfSight)
						{
							if (TargetCharacter->bIsCompanion)
							{
								BlackboardComp->SetValueAsBool(CompanionSeenKey.SelectedKeyName, true);
							}
							else
							{
								BlackboardComp->SetValueAsBool(PlayerSeenKey.SelectedKeyName, true);
							}

							return;
						}
					}

					BlackboardComp->SetValueAsBool(PlayerSeenKey.SelectedKeyName, false);
					BlackboardComp->SetValueAsBool(CompanionSeenKey.SelectedKeyName, false);
				}
			}
		}
	}
}
