// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.


#include "SAtttributeComponent.h"

// Sets default values for this component's properties
USAtttributeComponent::USAtttributeComponent()
{
	Health = 100.0f;

	HealthMax = 100.0f;
}

USAtttributeComponent* USAtttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<USAtttributeComponent>(FromActor->GetComponentByClass(USAtttributeComponent::StaticClass()));
	}

	return nullptr;
}



bool USAtttributeComponent::IsActorAlive(AActor* Actor)
{
	USAtttributeComponent* AttributeComponent = GetAttributes(Actor);

	if (AttributeComponent)
	{
		return AttributeComponent->IsAlive();
	}

	return false;
}

bool USAtttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

float USAtttributeComponent::GetHealth() const
{
	return Health;
}

bool USAtttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	float OldHealth = Health;
	
	Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = Health - OldHealth;
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);
	
	return ActualDelta != 0;
}
