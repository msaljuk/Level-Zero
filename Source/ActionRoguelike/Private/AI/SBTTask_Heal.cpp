// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_Heal.h"
#include "..\..\Public\AI\SBTTask_Heal.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "SAtttributeComponent.h"
#include "AI/SAICharacter.h"

EBTNodeResult::Type USBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	AAIController* AIController = OwnerComp.GetAIOwner();
	
	if (ensure(AIController))
	{
		ASAICharacter* AIPawn = Cast<ASAICharacter>(AIController->GetPawn());
		if (AIPawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		USAtttributeComponent* AttributeComp = Cast<USAtttributeComponent>(AIPawn->GetComponentByClass(USAtttributeComponent::StaticClass()));

		if (ensure(AttributeComp))
		{
			const float HealDelta = 20.0f;

			AttributeComp->ApplyHealthChange(AIPawn, HealDelta);

			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
