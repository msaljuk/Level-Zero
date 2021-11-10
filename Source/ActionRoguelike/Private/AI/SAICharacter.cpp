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

    float DistanceTo = FVector::Distance(Pawn->GetActorLocation(), GetActorLocation());

	ASAIController* AIController = Cast<ASAIController>(GetController());
    if (AIController)
    {
		if (!IsSeenPawnObstructed(Pawn) || DistanceTo < 750.0f)
		{
			AIController->HeardPlayerLocation = Pawn->GetActorLocation();
			AIController->LastHeardTime = GetWorld()->GetTimeSeconds();

			SetTargetActor(Pawn);
			// GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Updating player location");

			// GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, "Character seen");
        }

		ASCharacter* DetectableCharacter = Cast<ASCharacter>(Pawn);
		if (DetectableCharacter)
		{
			if (DistanceTo < 1500.0f)
			{
				AIController->SetCharacterAboutToBeDetected(DetectableCharacter);
                DetectableCharacter->bIsFeelingSafe = false;
			}
			else
			{
				DetectableCharacter->bIsFeelingSafe = true;
			}
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

bool ASAICharacter::IsSeenPawnObstructed(APawn* Pawn)
{
    ASCharacter* CharacterPawn = Cast<ASCharacter>(Pawn);
    if (ensure(CharacterPawn))
    {
		AActor* MyOwner = GetOwner();

		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector TargetLocation = Pawn->GetActorLocation();

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Vehicle);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

		FHitResult Hit;
		GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, TargetLocation, ObjectQueryParams);

		if (Hit.GetActor())
		{
			if (Hit.GetActor()->GetName().Contains("InstancedFoliageActor"))
			{
				if (!CharacterPawn->bIsCrouched && !CharacterPawn->bIsCompanion)
				{
                    CharacterPawn->bIsObstructed = false;
					return false;
				}

                CharacterPawn->bIsObstructed = true;
// 				FString Message = "Saw " + Pawn->GetName() + " but they are behind " + Hit.GetActor()->GetName();
// 				GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, Message);
				return true;
			}
		}

        CharacterPawn->bIsObstructed = false;

		return false;
    }

    return false;
}