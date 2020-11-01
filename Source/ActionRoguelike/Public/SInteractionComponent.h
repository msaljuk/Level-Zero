// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInteractionComponent.generated.h"

class USWorldUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONROGUELIKE_API USInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* InFocus);

	void PrimaryInteract();

public:

	// Sets default values for this component's properties
	USInteractionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void FindBestInteractable();


	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float TraceRadius;
	
	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	UPROPERTY()
	AActor* FocusedActor;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USWorldUserWidget> DefaultWidgetClass;

	UPROPERTY()
	USWorldUserWidget* DefaultWidgetInstance;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		
};
