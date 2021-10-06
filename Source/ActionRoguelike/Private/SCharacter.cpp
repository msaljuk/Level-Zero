// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetMathLibrary.h"
#include "SAtttributeComponent.h"
#include "SActionComponent.h"
#include "SPlayerState.h"
#include <Kismet/GameplayStatics.h>
#include <Components/PawnNoiseEmitterComponent.h>

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SocketOffset = FVector(0, 90, 0);
	SpringArmComp->TargetArmLength = 250.0f;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<USAtttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");

	PawnNoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("PawnNoiseEmitterComp"));

	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetMesh()->SetGenerateOverlapEvents(true);

	bUseControllerRotationYaw = false;

	bIsCrouched = false;
}


void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	AttributeComp->OnRageChanged.AddDynamic(this, &ASCharacter::OnRageChanged);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

FRotator ASCharacter::GetPawnViewRotation() const
{
	return CameraComp->GetComponentRotation();
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::SecondaryAttack);

	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASCharacter::Dash);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::StopSprint);

	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
}


void ASCharacter::HealSelf(float Amount /*= 100*/)
{
	AttributeComp->ApplyHealthChange(this, Amount);
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	AddMovementInput(ControlRot.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	// get right vector of PlayerController
	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, Value);
}

void ASCharacter::PrimaryAttack()
{
	ActionComp->StartActionByName(this, "PrimaryAttack");
}

void ASCharacter::SecondaryAttack()
{
	ActionComp->StartActionByName(this, "SecondaryAttack");
}


void ASCharacter::Dash()
{
	ActionComp->StartActionByName(this, "Dash");
}

void ASCharacter::StartSprint()
{
	ActionComp->StartActionByName(this, "Sprint");
}

void ASCharacter::StopSprint()
{
	ActionComp->StopActionByName(this, "Sprint");
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}


void ASCharacter::ReportNoise(USoundBase* SoundToPlay, float Volume)
{
	//If we have a valid sound to play, play the sound and
	//report it to our game
	if (SoundToPlay)
	{
		//Play the actual sound
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay, GetActorLocation(), Volume);

		//Report that we've played a sound with a certain volume in a specific location
		MakeNoise(Volume, this, GetActorLocation());
	}
}

void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAtttributeComponent* OwningComp, float NewHealth, float Delta)
{
	USkeletalMeshComponent* MeshComp = GetMesh();

	// Damaged
	if (Delta < 0.0f)
	{
		// Alive
		if (NewHealth > 0.0f)
		{
			if (MeshComp)
			{
				MeshComp->SetScalarParameterValueOnMaterials("TimeToHit", GetWorld()->TimeSeconds);
			}

			AttributeComp->ApplyRageChange(InstigatorActor, -Delta);
		}
		// Dead
		else
		{
			APlayerController* PlayerController = Cast<APlayerController>(GetController());

			DisableInput(PlayerController);

			// set Lifespan
			SetLifeSpan(10.0f);
		}
	}
}

void ASCharacter::OnRageChanged(AActor* InstigatorActor, USAtttributeComponent* OwningComp, float NewRage, float Delta)
{
	if (NewRage < BlackholeRageCost)
	{
		if (!ActionComp->ActiveGameplayTags.HasTag(InsufficientBlackholeRageTag))
		{
			ActionComp->ActiveGameplayTags.AddTag(InsufficientBlackholeRageTag);
		}
	}
	else 
	{
		if (ActionComp->ActiveGameplayTags.HasTag(InsufficientBlackholeRageTag))
		{
			ActionComp->ActiveGameplayTags.RemoveTag(InsufficientBlackholeRageTag);
		}
	}
}


void ASCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	ASPlayerState* CustomPlayerState = Cast<ASPlayerState>(GetPlayerState());

	if (CustomPlayerState)
	{
		CustomPlayerState->OnCreditsChanged.AddDynamic(this, &ASCharacter::OnCreditsChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player State not found for %s"), *GetNameSafe(GetOwner()));
	}
}


void ASCharacter::OnCreditsChanged(APawn* Player, int NewCredits, int Delta)
{
	FString DebugMsg = (GetNameSafe(Player->GetOwner()) + " Credits: " + FString::FromInt(NewCredits));
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, DebugMsg);
}
