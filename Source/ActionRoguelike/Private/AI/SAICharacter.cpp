// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include "Perception/PawnSensingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"
#include "SAtttributeComponent.h"
#include "BrainComponent.h"
#include "SWorldUserWidget.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SActionComponent.h"
#include "SCharacter.h"
#include "AI/SAIController.h"

// Sets default values
ASAICharacter::ASAICharacter()
{
    PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

    AttributeComp = CreateDefaultSubobject<USAtttributeComponent>("AttributeComp");

    ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	// GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
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
    ASAICharacter* EnemyPawn = Cast<ASAICharacter>(Pawn);
    if (EnemyPawn)
    {
        return;
    }

    ASCharacter* CharacterPawn = Cast<ASCharacter>(Pawn);
    if (CharacterPawn)
    {
        if (CharacterPawn->bIsObstructed)
        {
            float DistanceTo = FVector::Distance(CharacterPawn->GetActorLocation(), GetActorLocation());
            if (DistanceTo < 500.0f)
            {
                SetTargetActor(Pawn);
                GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, "Character obstructed but close enough to be seen");
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, "Character obstructed and not close enough to be seen");
            }
        } 
        else
        {
			ASAIController *AIController = Cast<ASAIController>(GetController());
            if (AIController)
            {
                AIController->HeardPlayerLocation = CharacterPawn->GetActorLocation();
                AIController->LastHeardTime = GetWorld()->GetTimeSeconds();

                GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Updating player location");
            }
            SetTargetActor(Pawn);
            GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, "Character seen");
        }
    }
}

void ASAICharacter::SetTargetActor(AActor* NewActor)
{
	ASAIController* AIController = Cast<ASAIController>(GetController());

    if (AIController)
    {
        UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();

        BlackboardComp->SetValueAsObject("TargetActor", NewActor);
    }
}
