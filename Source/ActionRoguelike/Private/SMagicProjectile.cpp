	// Fill out your copyright notice in the Description page of Project Settings.

#include "SMagicProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "SAtttributeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Components/AudioComponent.h"
#include "SGameplayFunctionLibrary.h"
#include "SActionComponent.h"
#include "SActionEffect.h"
#include "AI/SAIController.h"
#include "SCharacter.h"
#include "AI/SAICharacter.h"
#include "Companion AI/SCompanionAIController.h"
#include "SGameModeBase.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComp->SetCollisionProfileName("Projectile");
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
	SphereComp->OnComponentHit.AddDynamic(this, &ASMagicProjectile::OnActorHit);
	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 4000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

	ExplosionDelay = 7.0f;
	ProjectileDamage = 20.0f;

	InitialLifeSpan = 10.0f;

	AudioComp = CreateDefaultSubobject<UAudioComponent>("AudioComp");
	AudioComp->SetupAttachment(RootComponent);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_ProjectileExplosionDelay, this, &ASMagicProjectile::Explode, ExplosionDelay);
	
}

// Called every frame
void ASMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator())
	{
		if (OtherActor->GetName().Contains("InstancedFoliageActor"))
		{
			return;
		}

		TryEnemyPawnKillCompanionActor(GetInstigator(), OtherActor);

		Explode();

// 		USAtttributeComponent* AttributeComp = Cast<USAtttributeComponent>(OtherActor->GetComponentByClass(USAtttributeComponent::StaticClass()));
// 
// 		USActionComponent* ActionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));
// 		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
// 		{
// 			MovementComp->Velocity = -MovementComp->Velocity;
// 
// 			SetInstigator(Cast<APawn>(OtherActor));
// 			return;
// 		}
// 
// // 		if (ActionComp && HasAuthority())
// // 		{
// // 			if (ThornActionEffect)
// // 			{
// // 				ActionComp->AddAction(GetInstigator(), ThornActionEffect);
// // 			}
// // 		}
// 
// 		// Double Projectile Damage Value if Double Damage Buff Present For Instigator
// 		USActionComponent* InstigatorActionComp = Cast<USActionComponent>(GetInstigator()->GetComponentByClass(USActionComponent::StaticClass()));
// 
// 		if (InstigatorActionComp && HasAuthority())
// 		{	
// 			if (InstigatorActionComp->ContainsAction(DoubleDamageActionEffect))
// 			{
// 				ProjectileDamage *= 2;
// 			} 
// 		}
		

		// Apply Damage and Impulse
// 		if (USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, ProjectileDamage, SweepResult))
// 		{
//			Explode();
// 
// 			// Apply Burning Buff (If Present)
// 			if (ActionComp && HasAuthority())
// 			{
// 				if (BurningActionEffect)
// 				{
// 					ActionComp->AddAction(GetInstigator(), BurningActionEffect);
// 				}
// 			}
/*		}*/

	}
}

void ASMagicProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		if (OtherActor->GetName().Contains("InstancedFoliageActor"))
		{
			return;
		}

		TryEnemyPawnKillCompanionActor(GetInstigator(), OtherActor);

		Explode();
	}
}

void ASMagicProjectile::Explode()
{
	AudioComp->Stop();

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());

	APawn* OwnerPawn = GetInstigator();

	ASCharacter* OwnerCharacter = Cast<ASCharacter>(OwnerPawn);
	if (OwnerCharacter)
	{
		MakeNoise(1.0f, OwnerPawn, GetActorLocation());
	} 
	else {
		ASAICharacter* EnemyCharacter = Cast<ASAICharacter>(OwnerPawn);
		if (EnemyCharacter)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), BeingAttackedSound, GetActorLocation());
		}
	}
	// Debug
// 	FActorSpawnParameters SpawnParams;
// 	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
// 	SpawnParams.Instigator = OwnerPawn;
// 	GetWorld()->SpawnActor<AActor>(DebugProjectileClass, FTransform(GetActorLocation()), SpawnParams);

	Destroy();
}

void ASMagicProjectile::TryEnemyPawnKillCompanionActor(APawn* Enemy, AActor* OtherActor)
{
	if (Cast<ASAICharacter>(Enemy))
	{
		APawn* OtherPawn = Cast<APawn>(OtherActor);
		if (OtherPawn)
		{
			ASCompanionAIController* CompanionAIController = Cast<ASCompanionAIController>(OtherPawn->GetController());
			if (CompanionAIController)
			{
				OtherPawn->Destroy();
				CompanionAIController->Destroy();

				ASGameModeBase* GameModeBase = Cast<ASGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
				if (GameModeBase)
				{
					GameModeBase->MarkCompanionAsKilled();
				}
			}
		}
	}
}

