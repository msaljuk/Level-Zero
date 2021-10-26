// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "SWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stealth Data")
	TArray<AActor*> CoverPoints;
	
};
