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

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;

}

void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
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

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	GetWorld()->SpawnActor<AActor>(PrimaryProjectileClass, GetProjectileSpawnTM(), GetProjectileSpawnParams());
}

void ASCharacter::SecondaryAttack()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_SecondaryAttack, this, &ASCharacter::SecondaryAttack_TimeElapsed, 0.2f);
}

void ASCharacter::SecondaryAttack_TimeElapsed()
{
	GetWorld()->SpawnActor<AActor>(SecondaryProjectileClass, GetProjectileSpawnTM(), GetProjectileSpawnParams());
}

void ASCharacter::Dash()
{
	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &ASCharacter::Dash_TimeElapsed, 0.2f);
}

void ASCharacter::Dash_TimeElapsed()
{
	GetWorld()->SpawnActor<AActor>(DashProjectileClass, GetProjectileSpawnTM(), GetProjectileSpawnParams());
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

FTransform ASCharacter::GetProjectileSpawnTM()
{
	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	
	FVector CameraLocation = CameraComp->GetComponentLocation();
	FRotator CameraRotation = CameraComp->GetComponentRotation();

	FVector WorldEnd = CameraLocation + (CameraRotation.Vector() * 5000);

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Vehicle);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);

	FHitResult Hit;
	GetWorld()->LineTraceSingleByObjectType(Hit, CameraLocation, WorldEnd, ObjectQueryParams);

	FVector ImpactLocation;
	if (Hit.GetActor())
	{
		ImpactLocation = Hit.Location;
	}
	else
	{
		ImpactLocation = WorldEnd;
	}

	FRotator SpawnRotator = UKismetMathLibrary::FindLookAtRotation(HandLocation, ImpactLocation);

	return FTransform(SpawnRotator, HandLocation);
}

FActorSpawnParameters ASCharacter::GetProjectileSpawnParams() {
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;

	return SpawnParams;
}
