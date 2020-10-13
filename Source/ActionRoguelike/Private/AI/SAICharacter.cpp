// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "SAtttributeComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

    AttributeComp = CreateDefaultSubobject<USAtttributeComponent>("AttributeComp");

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASAICharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
    AAIController* AIController = Cast<AAIController>(GetController());

    if (AIController)
    {
        UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();

        BlackboardComp->SetValueAsObject("TargetActor", Pawn);

//      DrawDebugString(GetWorld(), GetActorLocation(), TEXT("PLAYER SPOTTED"), nullptr, FColor::White, 4.0f, true);
    }
}
