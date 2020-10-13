// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckHealth.h"
#include "..\..\Public\AI\SBTService_CheckHealth.h"
#include "AI/SAICharacter.h"
#include "AIController.h"
#include "SAtttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

void USBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();

	if (ensure(AIController))
	{
		ASAICharacter* AIPlayer = Cast<ASAICharacter>(AIController->GetPawn());

		if (ensure(AIPlayer))
		{
			USAtttributeComponent* AttributeComp = Cast<USAtttributeComponent>(AIPlayer->GetComponentByClass(USAtttributeComponent::StaticClass()));

			if (ensure(AttributeComp))
			{
				const float Health = AttributeComp->GetHealth();

				UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

				if (ensure(BlackboardComp))
				{
					BlackboardComp->SetValueAsFloat(HealthKey.SelectedKeyName, Health);
				}
			}

		}
	}
}
