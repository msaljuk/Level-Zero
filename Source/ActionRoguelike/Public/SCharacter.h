// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USInteractionComponent;
class UAnimMontage;
class USAtttributeComponent;
class USActionComponent;

UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent* InteractionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USAtttributeComponent* AttributeComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USActionComponent* ActionComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BlackholeRageCost;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InsufficientBlackholeRageTag;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	void PrimaryAttack();

	void SecondaryAttack();

	void Dash();

	void StartSprint();
	void StopSprint();

	void PrimaryInteract();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAtttributeComponent* OwningComp, float NewHealth, float Delta);

	UFUNCTION()
	void OnRageChanged(AActor* InstigatorActor, USAtttributeComponent* OwningComp, float NewRage, float Delta);

	UFUNCTION()
	void OnCreditsChanged(APawn* Player, int NewCredits, int Delta);

	virtual void PostInitializeComponents();

	void OnRep_PlayerState() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	virtual FRotator GetPawnViewRotation() const;

	UFUNCTION(Exec)
	void HealSelf(float Amount = 100);
};
