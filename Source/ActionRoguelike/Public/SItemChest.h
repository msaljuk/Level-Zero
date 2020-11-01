// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SItemChest.generated.h"

class UStaticMeshComponent;

UCLASS()
class ACTIONROGUELIKE_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere)
	float TargetOpenPitch;

	void Interact_Implementation(APawn* InstigatorPawn);

public:	
	// Sets default values for this actor's properties
	ASItemChest();

protected:

	UPROPERTY(ReplicatedUsing="OnRep_LidOpened", BlueprintReadOnly) // RepNotify in Blueprints/Documentation
	bool bLidOpened;

	UFUNCTION()
	void OnRep_LidOpened();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent *BaseMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LidMesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
