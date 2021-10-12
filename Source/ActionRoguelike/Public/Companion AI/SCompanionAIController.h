// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SCompanionAIController.generated.h"

class UBehaviorTree;

UENUM()
enum Mode
{
	Hide     UMETA(DisplayName = "Hide"),
	Follow   UMETA(DisplayName = "Follow"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnModeChanged, ASCompanionAIController*, CompanionAIController, Mode, mode);

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASCompanionAIController : public AAIController
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnModeChanged OnModeChanged;

	UFUNCTION(BlueprintCallable)
	Mode GetCompanionAIMode() const;

	UFUNCTION(BlueprintCallable)
	Mode SetCompanionAIMode(Mode newMode);


protected:

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	TEnumAsByte<Mode> mode;

	virtual void BeginPlay() override;
	
};
