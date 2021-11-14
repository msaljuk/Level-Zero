// Fill out your copyright notice in the Description page of Project Settings.


#include "Companion AI/SBTService_CheckCompanionMode.h"
#include "Companion AI/SCompanionAIController.h"
#include <BehaviorTree/BlackboardComponent.h>

void USBTService_CheckCompanionMode::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (ensure(BlackboardComp))
	{
		ASCompanionAIController* CompanionAIController = Cast<ASCompanionAIController>(OwnerComp.GetAIOwner());

		if (ensure(CompanionAIController))
		{
			Mode CompanionMode = CompanionAIController->GetCompanionAIMode();

			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, CompanionMode == Follow ? "Follow" : "Hide");

			BlackboardComp->SetValueAsEnum(CompanionModeKey.SelectedKeyName, CompanionMode);
		}
	}
}
