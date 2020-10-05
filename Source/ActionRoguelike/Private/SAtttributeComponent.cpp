// Fill out your copyright notice in the Description page of Project Settings.


#include "SAtttributeComponent.h"


// Sets default values for this component's properties
USAtttributeComponent::USAtttributeComponent()
{
	Health = 100.0f;

	HealthMax = 100.0f;
}

bool USAtttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAtttributeComponent::ApplyHealthChange(float Delta)
{
	if (Health != 0)
	{
		Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

		OnHealthChanged.Broadcast(nullptr, this, Health, Delta);

		return true;
	}

	return false;
}
