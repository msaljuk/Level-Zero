// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "SAtttributeComponent.h"
#include "BrainComponent.h"
#include "SWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SActionComponent.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

    AttributeComp = CreateDefaultSubobject<USAtttributeComponent>("AttributeComp");

    ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    TimeToHitParamName = "TimeToHit";
}

void ASAICharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);

    PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAtttributeComponent* OwningComp, float NewHealth, float Delta)
{
    // Damaged
    if (Delta < 0.0f)
    {
        if (InstigatorActor != this)
        {
            SetTargetActor(InstigatorActor);
        }

        if (ActiveHealthBar == nullptr)
        {
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
            if (ActiveHealthBar)
            {
                ActiveHealthBar->AttachedActor = this;
                ActiveHealthBar->AddToViewport();
            }
        }

        GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParamName, GetWorld()->TimeSeconds);

        // Bot has just died
        if (NewHealth <= 0.0f)
        {
            // stop BT
            AAIController* AIC = Cast<AAIController>(GetController());

            if (AIC)
            {
                AIC->GetBrainComponent()->StopLogic("Killed");
            }

            // rag doll
            GetMesh()->SetAllBodiesSimulatePhysics(true);
            GetMesh()->SetCollisionProfileName("Ragdoll");

            GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            GetCharacterMovement()->DisableMovement();

            // set Lifespan
            SetLifeSpan(10.0f);
        }
    }
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
    SetTargetActor(Pawn);
}

void ASAICharacter::SetTargetActor(AActor* NewActor)
{
	AAIController* AIController = Cast<AAIController>(GetController());

    if (AIController)
    {
        UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();

        BlackboardComp->SetValueAsObject("TargetActor", NewActor);
    }
}
