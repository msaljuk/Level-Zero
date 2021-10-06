// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SAIController.generated.h"

class UBehaviorTree;

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASAIController : public AAIController
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FVector HeardPlayerLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float LastHeardTime;


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BehaviorTree;

	virtual void BeginPlay() override;

	void InitializeBlackboardHealth();
};
