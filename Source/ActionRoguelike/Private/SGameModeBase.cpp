// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "AI/SAICharacter.h"
#include "SAtttributeComponent.h"
#include "EngineUtils.h"
#include "SCharacter.h"
#include "SPlayerState.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), false, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;

	KillCredits = 2;

	PlayerStateClass = ASPlayerState::StaticClass();
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnTimer, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}


void ASGameModeBase::KillAllAI()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;

		USAtttributeComponent* AttributeComp = USAtttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this);
		}
	}
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	if (CanSpawnNewBot())
	{
		UEnvQueryInstanceBlueprintWrapper* QueryInst = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

		if (ensure(QueryInst))
		{
			QueryInst->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);
		}
	}
	
}

bool ASGameModeBase::CanSpawnNewBot()
{
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disabled via cvar 'CVarSpawnBots'."));
		return false;
	}

	int32 NrOfAliveBots = 0;
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;

		USAtttributeComponent* AttributeComp = USAtttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			NrOfAliveBots++;
		}
	}

	float MaxBotCount = 10.0f;

	if (DifficultyCurve)
	{
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (NrOfAliveBots >= MaxBotCount)
	{
		return false;
	}

	return true;
}

void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);
	}
}

void ASGameModeBase::RespawnPlayer(ASCharacter* Player)
{
	FTimerHandle TimerHandle_RespawnDelay;

	FTimerDelegate Delegate;
	Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

	float RespawnDelay = 2.0f;
	GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();

		RestartPlayer(Controller);
	}
}


void ASGameModeBase::GivePlayerKillCredits(ASCharacter* Player)
{
	ASPlayerState* PlayerState = Player->GetPlayerState<ASPlayerState>();

	PlayerState->AddCredits(KillCredits);

	UE_LOG(LogTemp, Log, TEXT("PlayerCredits: %d"), PlayerState->PlayerCredits);
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	ASCharacter* PlayerVictim = Cast<ASCharacter>(VictimActor);

	// if player was killed, respawn player
	if (PlayerVictim)
	{
		RespawnPlayer(PlayerVictim);
	} 

	// if player killed bot, give player kill credits
	else 
	{
		ASCharacter* PlayerKiller = Cast<ASCharacter>(Killer);

		if (PlayerKiller)
		{
			GivePlayerKillCredits(PlayerKiller);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));
}

