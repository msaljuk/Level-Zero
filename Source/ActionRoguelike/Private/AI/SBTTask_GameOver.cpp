// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_GameOver.h"
#include <Kismet/KismetSystemLibrary.h>

EBTNodeResult::Type USBTTask_GameOver::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
	
	return EBTNodeResult::Succeeded;
}
