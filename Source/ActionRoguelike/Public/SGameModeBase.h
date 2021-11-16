// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Engine/DataTable.h"
#include "SGameModeBase.generated.h"

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class ASCharacter;
class USSaveGame;
class USMonsterData;

UENUM(BlueprintType)
enum AlertLevel
{
	None      UMETA(DisplayName = "None"),
	Caution   UMETA(DisplayName = "Caution"),
	Search	  UMETA(DisplayName = "Search"),
	Alert	  UMETA(DisplayName = "Alert")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAlertLevelChanged, ASGameModeBase*, GameModeBase, AlertLevel, GameAlertLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCompanionLifeChanged, bool, bIsCompanionCurrentlyAlive);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeFrozenChanged, bool, bIsTimeFrozen);

USTRUCT(BlueprintType)
struct FMonsterInfoRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	FMonsterInfoRow()
	{
		Weight = 1.0f;
		SpawnCost = 5.0f;
		KillReward = 3.0f;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MonsterId;

/*	TSubclassOf<AActor> MonsterClass;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;

};

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	FString SlotName;

	UPROPERTY()
	USSaveGame* CurrentSaveGame;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UDataTable* MonsterTable;

// 	UPROPERTY(EditDefaultsOnly, Category = "AI")
// 	TSubclassOf<AActor> MinionClass;

	FTimerHandle TimerHandle_SpawnTimer;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	int KillCredits;

	UPROPERTY(EditAnywhere, Category = "Credits")
	TSubclassOf<AActor> PlayerCreditsPickup;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UFUNCTION()
	bool CanSpawnNewBot();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	AlertLevel GameAlertLevel;

	UFUNCTION()
	void OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	void OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation);

	void UpdateGamePlayers(ASCharacter* Player);

	void SpawnPlayerCredits(ASCharacter* Player);

	void GivePlayerKillCredits(ASCharacter* Player);

	void PauseCompanion();

	void ResumeCompanion();

	void ChangeFreezeAllEnemies(bool bShouldFreeze);

	void AdjustPlayerSpeed(float Delta);

public:

	virtual void OnActorKilled(AActor* VictimActor, AActor* Killer);

	ASGameModeBase();

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	virtual void StartPlay() override;

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UFUNCTION(Exec)
	void KillAllAI();

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void RespawnPlayer(AController* Controller);

	UFUNCTION(BlueprintCallable, Category = "Save")
	void WriteSaveGame();

	void LoadSaveGame();

	UPROPERTY(BlueprintReadOnly)
	float NumberOfAlivePlayers;

	UFUNCTION(BlueprintCallable)
	AlertLevel GetGameAlertLevel();

	UFUNCTION(BlueprintCallable)
	AlertLevel SetGameAlertLevel(AlertLevel NewAlertLevel);

	UPROPERTY(BlueprintAssignable)
	FOnAlertLevelChanged OnAlertLevelChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	UObject* CoverPointAPI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	bool bIsCompanionAlive;

	UPROPERTY(BlueprintAssignable, Category = "Companion")
	FOnCompanionLifeChanged OnCompanionLifeChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion")
	bool bIsTimeFrozen;

	UPROPERTY(BlueprintAssignable, Category = "Companion")
	FOnTimeFrozenChanged OnTimeFrozenChanged;

	UFUNCTION(BlueprintCallable)
	void MarkCompanionAsKilled();

	UFUNCTION(BlueprintCallable)
	void MarkCompanionAsRevived();

	UFUNCTION(BlueprintCallable)
	void FreezeTime();
};
