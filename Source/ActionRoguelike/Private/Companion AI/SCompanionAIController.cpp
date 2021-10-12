// Fill out your copyright notice in the Description page of Project Settings.


#include "Companion AI/SCompanionAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void ASCompanionAIController::BeginPlay()
{
	Super::BeginPlay();

	// Companion set to follow player at startup
	mode = Mode::Follow;

	if (ensureMsgf(BehaviorTree, TEXT("BehaviorTree is nullptr. Please assign BehaviorTree in AIController")))
	{
		RunBehaviorTree(BehaviorTree);
	}
}

Mode ASCompanionAIController::GetCompanionAIMode() const
{
	return mode;
}

Mode ASCompanionAIController::SetCompanionAIMode(Mode newMode)
{
	mode = newMode;
	OnModeChanged.Broadcast(this, mode);
	return mode;
}

