// Fill out your copyright notice in the Description page of Project Settings.


#include "Companion AI/SCompanionAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void ASCompanionAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("BehaviorTree is nullptr. Please assign BehaviorTree in AIController")))
	{
		RunBehaviorTree(BehaviorTree);
	}
}

