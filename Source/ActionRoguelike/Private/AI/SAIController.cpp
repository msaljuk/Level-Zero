// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/SAICharacter.h"
#include "SAtttributeComponent.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("BehaviorTree is nullptr. Please assign BehaviorTree in AIController")))
	{
		RunBehaviorTree(BehaviorTree);
	}

	InitializeBlackboardHealth();
}

void ASAIController::InitializeBlackboardHealth()
{
	ASAICharacter* AIPlayer = Cast<ASAICharacter>(GetPawn());

	if (ensure(AIPlayer))
	{
		USAtttributeComponent* AttributeComp = Cast<USAtttributeComponent>(AIPlayer->GetComponentByClass(USAtttributeComponent::StaticClass()));

		if (ensure(AttributeComp))
		{
			UBlackboardComponent* BlackboardComp = GetBlackboardComponent();

			if (ensure(BlackboardComp))
			{
				BlackboardComp->SetValueAsFloat("Health", AttributeComp->HealthMax);
			}
		}
	}
}
