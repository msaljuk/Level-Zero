// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_ProjectileAttack.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "SCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "SActionComponent.h"


USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	HandSocketName = "Muzzle_01";
	AttackAnimDelay = 0.2f;
}


void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);

	if (Character)
	{
		Character->PlayAnimMontage(AttackAnim);

		UGameplayStatics::SpawnEmitterAttached(CastingEffect, Character->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

		if (Character->HasAuthority())
		{
			FTimerHandle TimerHandle_AttackDelay;
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "AttackDelay_Elapsed", Character);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, Delegate, AttackAnimDelay, false);
		}
	}
	else
	{
		StopAction(Instigator);
	}
}


void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (ensureAlways(ProjectileClass))
	{
		GetWorld()->SpawnActor<AActor>(ProjectileClass, GetProjectileSpawnTM(InstigatorCharacter), GetProjectileSpawnParams(InstigatorCharacter));
	}

	StopAction(InstigatorCharacter);
}

FTransform USAction_ProjectileAttack::GetProjectileSpawnTM(ACharacter* InstigatorCharacter)
{
	ASCharacter* Instigator = Cast<ASCharacter>(InstigatorCharacter);

	if (Instigator)
	{
		FVector HandLocation = Instigator->GetMesh()->GetSocketLocation(HandSocketName);

		FVector CameraLocation = Instigator->GetPawnViewLocation();
		FRotator CameraRotation = Instigator->GetPawnViewRotation();

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

	return FTransform(FVector(0, 0, 0));
}

FActorSpawnParameters USAction_ProjectileAttack::GetProjectileSpawnParams(ACharacter* InstigatorCharacter) {
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = InstigatorCharacter;

	return SpawnParams;
}
