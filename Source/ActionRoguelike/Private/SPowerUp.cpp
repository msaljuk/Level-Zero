// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUp.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASPowerUp::ASPowerUp()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComp");
	RootComponent = StaticMeshComp;

	InteractDisableDuration = 10.0f;

	bInteractDisabled = false;
}

// Called when the game starts or when spawned
void ASPowerUp::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASPowerUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASPowerUp::DeactivateInteract_Implementation()
{
	bInteractDisabled = true;

	StaticMeshComp->SetVisibility(false);

	GetWorldTimerManager().SetTimer(InteractTimer, this, &ASPowerUp::ActivateInteract, InteractDisableDuration);
}

void ASPowerUp::ActivateInteract_Implementation()
{
	bInteractDisabled = false;

	StaticMeshComp->SetVisibility(true);
}

