// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckPlayerHeard.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/SAIController.h"

const float LAST_HEARD_DELTA = 5.0f;

void USBTService_CheckPlayerHeard::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (ensure(BlackboardComp))
	{
		ASAIController* MyController = Cast<ASAIController>(OwnerComp.GetAIOwner());

		if (ensure(MyController))
		{
			
			if (!MyController->HeardPlayerLocation.IsZero())
			{
				if (MyController->LastHeardTime - GetWorld()->GetTimeSeconds() < LAST_HEARD_DELTA)
				{
					BlackboardComp->SetValueAsBool(PlayerHeardKey.SelectedKeyName, true);
					BlackboardComp->SetValueAsVector(PlayerHeardLocationKey.SelectedKeyName, MyController->HeardPlayerLocation);

					return;
				}
			}
			
			BlackboardComp->SetValueAsBool(PlayerHeardKey.SelectedKeyName, false);
		}
	}
}
