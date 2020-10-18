// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"
#include "SAtttributeComponent.h"

static TAutoConsoleVariable<float> CVarDirectionalDamageImpulse(TEXT("su.DirectionalDamageImpulse"), 300000.0f, TEXT("Set value of directional damage impulse."), ECVF_Cheat);

bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	USAtttributeComponent* AttributeComp = USAtttributeComponent::GetAttributes(TargetActor);
	if (AttributeComp)
	{
		return AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount);
	}
	return false;
}


bool USGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount, const FHitResult& HitResult)
{
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		UPrimitiveComponent* HitComp = HitResult.GetComponent();
		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			HitComp->AddImpulseAtLocation(-HitResult.ImpactNormal * CVarDirectionalDamageImpulse.GetValueOnGameThread(), HitResult.ImpactPoint, HitResult.BoneName);
		}
		return true;
	}

	return false;
}