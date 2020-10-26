// Fill out your copyright notice in the Description page of Project Settings.


#include "SDashProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASDashProjectile::ASDashProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ProjectileExplosionEffect(TEXT("/Game/ParagonGideon/FX/Particles/Gideon/Abilities/Portal/FX/P_Portal_Cast.P_Portal_Cast"));
	ProjectileExplosion = ProjectileExplosionEffect.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PortalOpenEffect(TEXT("/Game/ParagonGideon/FX/Particles/Gideon/Abilities/Portal/FX/P_Portal_Teleport_Enter.P_Portal_Teleport_Enter"));
	PortalOpen = PortalOpenEffect.Object;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> PortalCloseEffect(TEXT("/Game/ParagonGideon/FX/Particles/Gideon/Abilities/Portal/FX/P_Portal_Teleport_Exit.P_Portal_Teleport_Exit"));
	PortalClose = PortalCloseEffect.Object;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComp->SetCollisionProfileName("Projectile");
	SphereComp->SetEnableGravity(false);
	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 5000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
	MovementComp->ProjectileGravityScale = 0.0f;
}

void ASDashProjectile::PostInitializeComponents()
{
	// Don't forget to call parent function
	Super::PostInitializeComponents();

	SphereComp->OnComponentHit.AddDynamic(this, &ASDashProjectile::OnActorHit);
}

// Called when the game starts or when spawned
void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_ProjectileExplosionDelay, this, &ASDashProjectile::ExplodeProjectileAndTeleport, 0.4f);
}

// Called every frame
void ASDashProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASDashProjectile::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ExplodeProjectileAndTeleport();
}

void ASDashProjectile::ExplodeProjectileAndTeleport()
{
	MovementComp->StopMovementImmediately();

	if (ProjectileExplosion)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileExplosion, GetActorLocation());
	}

	Teleport();
}

void ASDashProjectile::Teleport()
{
	if (PortalOpen)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PortalOpen, GetActorLocation());
	}

	GetWorldTimerManager().SetTimer(TimerHandle_TeleportDelay, this, &ASDashProjectile::Teleport_TimeElapsed, 0.2f);
}

void ASDashProjectile::Teleport_TimeElapsed()
{
	AActor* InstigatingActor = GetInstigator();

	if (InstigatingActor)
	{
		FVector TeleportLocation = GetActorLocation();

		// raise height coordinate to make player "drop" in that location
		TeleportLocation.Z += 100.0f;

		InstigatingActor->SetActorLocation(TeleportLocation);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), PortalClose, TeleportLocation);
	}

	Destroy();
}
