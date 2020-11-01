// Fill out your copyright notice in the Description page of Project Settings.
// Fill out your copyright notice in the Description page of Project Settings.


#include "SAtttributeComponent.h"
#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"


static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f, TEXT("Global damage modifier for Attribute Component."), ECVF_Cheat);

// Sets default values for this component's properties
USAtttributeComponent::USAtttributeComponent()
{
	Health = 100.0f;
	HealthMax = 100.0f;

	Rage = 0.0f;
	RageMax = 100.0f;

	SetIsReplicatedByDefault(true);
}

bool USAtttributeComponent::Kill(AActor* InstigatingActor)
{
	return ApplyHealthChange(InstigatingActor, -HealthMax);
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
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f)
	{
		return false;
	}

	float OldHealth = Health;

	if (Delta < 0.0f)
	{
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiplier;
	}

	
	Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = Health - OldHealth;
	// OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	if (ActualDelta != 0.0f)
	{
		MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
	}
	
	// Owning Actor has died
	if (ActualDelta < 0.0f && Health == 0.0f)
	{
		ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if (GM)
		{
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}

	return ActualDelta != 0;
}

bool USAtttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	float OldRage = Rage;

	Rage = FMath::Clamp(Rage + Delta, 0.0f, RageMax);

	float ActualDelta = Rage - OldRage;

	if (ActualDelta != 0)
	{
		OnRageChanged.Broadcast(InstigatorActor, this, Rage, ActualDelta);

		return true;
	}

	return false;
}

void USAtttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAtttributeComponent, Health);
	DOREPLIFETIME(USAtttributeComponent, HealthMax);
}

void USAtttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, Health, Delta);
}