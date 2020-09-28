// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SDashProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class UParticleSystemComponent;

UCLASS()
class ACTIONROGUELIKE_API ASDashProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASDashProjectile();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* EffectComp;

	UParticleSystem* ProjectileExplosion;
	UParticleSystem* PortalOpen;
	UParticleSystem* PortalClose;

	FTimerHandle TimerHandle_ProjectileExplosionDelay;
	FTimerHandle TimerHandle_TeleportDelay;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	void ExplodeProjectileAndTeleport();

	void Teleport();
	void Teleport_TimeElapsed();


	UFUNCTION()
	void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
