// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect_Thorns.h"
#include "SActionComponent.h"
#include "SAtttributeComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "SGameplayFunctionLibrary.h"

USActionEffect_Thorns::USActionEffect_Thorns()
{
	Duration = 0.0f;

	Period = 0.0f;

	ThornFraction = 0.2f;
}

void USActionEffect_Thorns::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (USAtttributeComponent* AttributeComp = GetOwnerAttributeComponent())
	{
		AttributeComp->OnHealthChanged.AddDynamic(this, &USActionEffect_Thorns::OnHealthChanged);

		// GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, "Added Thorn Dynamic");
	}
}

void USActionEffect_Thorns::StopAction_Implementation(AActor* Instigator)
{
	if (USAtttributeComponent* AttributeComp = GetOwnerAttributeComponent())
	{
		AttributeComp->OnHealthChanged.RemoveDynamic(this, &USActionEffect_Thorns::OnHealthChanged);

		// GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Removed Thorn Dynamic");
	}

	Super::StopAction_Implementation(Instigator);
}


void USActionEffect_Thorns::OnHealthChanged(AActor* InstigatorActor, USAtttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0)
	{
		AActor* VictimActor = GetOwningComponent()->GetOwner();

		if (VictimActor && InstigatorActor && VictimActor != InstigatorActor)
		{
			int ThornDamage = UKismetMathLibrary::Round(ThornFraction * -Delta);

			USGameplayFunctionLibrary::ApplyDamage(VictimActor, InstigatorActor, ThornDamage);
		}
	}

	StopAction_Implementation(InstigatorActor);
}



USAtttributeComponent* USActionEffect_Thorns::GetOwnerAttributeComponent()
{
	USActionComponent* ActionComp = Cast<USActionComponent>(GetOwningComponent());
	
	if (ensure(ActionComp))
	{
		AActor* OwningActor = ActionComp->GetOwner();
		
		if (ensure(OwningActor))
		{
			USAtttributeComponent* AttributeComp = USAtttributeComponent::GetAttributes(OwningActor);

			return AttributeComp;
		}
	}

	return nullptr;
}

