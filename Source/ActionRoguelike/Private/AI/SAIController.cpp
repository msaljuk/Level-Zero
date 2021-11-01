// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/SAICharacter.h"
#include "SAtttributeComponent.h"
#include "SCharacter.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("BehaviorTree is nullptr. Please assign BehaviorTree in AIController")))
	{
		RunBehaviorTree(BehaviorTree);
	}
}

void ASAIController::SetCharacterAboutToBeDetected(ASCharacter* DetectableCharacter)
{
	OnCharacterAboutToBeDetected.Broadcast(this, DetectableCharacter);
}

