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
				if (GetWorld()->GetTimeSeconds() - MyController->LastHeardTime < LAST_HEARD_DELTA)
				{
					BlackboardComp->SetValueAsBool(PlayerHeardKey.SelectedKeyName, true);
					BlackboardComp->SetValueAsVector(PlayerHeardLocationKey.SelectedKeyName, MyController->HeardPlayerLocation);
					BlackboardComp->SetValueAsBool(PreviousPlayerHeardKey.SelectedKeyName, true);

					return;
				}
			}

			BlackboardComp->SetValueAsBool(PlayerHeardKey.SelectedKeyName, false);

			// going from caution mode to no alert mode
			if (BlackboardComp->GetValueAsBool(PreviousPlayerHeardKey.SelectedKeyName))
			{
				BlackboardComp->SetValueAsBool(RevertToNoAlertModeKey.SelectedKeyName, true);
			} 
			else
			{
				BlackboardComp->SetValueAsBool(RevertToNoAlertModeKey.SelectedKeyName, false);
			}

			BlackboardComp->SetValueAsBool(PreviousPlayerHeardKey.SelectedKeyName, false);
		}
	}
}
