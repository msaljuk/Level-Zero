// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SPowerUp.generated.h"

class UStaticMeshComponent;

UCLASS()
class ACTIONROGUELIKE_API ASPowerUp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerUp();

protected:
	
	float InteractDisableDuration;

	UPROPERTY(ReplicatedUsing = "OnRep_InteractDisabled", BlueprintReadOnly) // RepNotify in Blueprints/Documentation
	bool bInteractDisabled;

	UFUNCTION()
	void OnRep_InteractDisabled();

	FTimerHandle InteractTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComp;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ActivateInteract();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DeactivateInteract();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
