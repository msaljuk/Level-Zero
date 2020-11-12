// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "AI/SAICharacter.h"
#include "SAtttributeComponent.h"
#include "EngineUtils.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "SSaveGame.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Controller.h"
#include "SGameplayInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), false, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;

	KillCredits = 2;

	PlayerStateClass = ASPlayerState::StaticClass();

	SlotName = "SaveGame01";

	NumberOfAlivePlayers = 0;
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	LoadSaveGame();
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnTimer, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}


void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();
	if (PS)
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}

	NumberOfAlivePlayers += 1;

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
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

void ASGameModeBase::UpdateGamePlayers(ASCharacter* Player)
{
	AController* Controller = Player->GetController();

	if (ensure(Controller))
	{
		Controller->UnPossess();

		NumberOfAlivePlayers -= 1;

		FString DebugMsg = "Number of Alive Players: " + FString::FromInt(NumberOfAlivePlayers);
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, DebugMsg);

		if (NumberOfAlivePlayers <= 0)
		{
			UGameplayStatics::OpenLevel(GetWorld(), "MainMenu_Entry", "?listen");
		}
	}
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
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
		UpdateGamePlayers(PlayerVictim);
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


void ASGameModeBase::WriteSaveGame()
{
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if (PS)
		{
			PS->SavePlayerState(CurrentSaveGame);

			break; // single player only right now; TO-DO
		}

	}

	CurrentSaveGame->SavedActors.Empty();

	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;

		if (Actor->Implements<USGameplayInterface>())
		{
			FActorSaveData ActorData;
			ActorData.ActorName = Actor->GetName();
			ActorData.Transform = Actor->GetActorTransform();

			FMemoryWriter MemWriter(ActorData.BytesData);

			FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);

			// Find only variables with UPROPERTY(SaveGame)
			Ar.ArIsSaveGame = true;

			Actor->Serialize(Ar);

			CurrentSaveGame->SavedActors.Add(ActorData);
		}
	}


	UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0);
}

void ASGameModeBase::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame Data"));
			return;
		}


		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;

			if (Actor->Implements<USGameplayInterface>())
			{
				for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
				{
					if (ActorData.ActorName == Actor->GetName())
					{
						Actor->SetActorTransform(ActorData.Transform);

						FMemoryReader MemReader(ActorData.BytesData);

						FObjectAndNameAsStringProxyArchive Ar(MemReader, true);

						// Find only variables with UPROPERTY(SaveGame)
						Ar.ArIsSaveGame = true;

						Actor->Serialize(Ar);

						ISGameplayInterface::Execute_OnActorLoaded(Actor);

						break;
					}
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Loaded SaveGame Data."))
	}
	else
	{
		CurrentSaveGame = Cast<USSaveGame>(UGameplayStatics::CreateSaveGameObject(USSaveGame::StaticClass()));

		UE_LOG(LogTemp, Log, TEXT("Created new SaveGame Data"));
	}
}