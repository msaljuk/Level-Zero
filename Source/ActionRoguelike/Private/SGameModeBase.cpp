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
#include "SMonsterData.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/Controller.h"
#include "SGameplayInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "Engine/AssetManager.h"
#include "SActionComponent.h"
#include "../ActionRoguelike.h"
#include "SCoin.h"
#include "SBuyStation.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), false, TEXT("Enable spawning of bots via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;

	KillCredits = 2;

	PlayerStateClass = ASPlayerState::StaticClass();

	SlotName = "SaveGame01";

	NumberOfAlivePlayers = 0;

	GameAlertLevel = None;
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString SelectedSaveSlot = UGameplayStatics::ParseOption(Options, "SaveGame");
	if (SelectedSaveSlot.Len() > 0)
	{
		SlotName = SelectedSaveSlot;
	}

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
		if (MonsterTable)
		{
			TArray<FMonsterInfoRow*> Rows;
			MonsterTable->GetAllRows("", Rows);

			int32 RandomIndex = FMath::RandRange(0, Rows.Num() - 1);
			FMonsterInfoRow* SelectedRow = Rows[RandomIndex];

			UAssetManager* Manager = UAssetManager::GetIfValid();
			if (Manager)
			{
				LogOnScreen(this, "Loading monster...", FColor::Green);

				TArray<FName> Bundles;
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectedRow->MonsterId, Locations[0]);
				Manager->LoadPrimaryAsset(SelectedRow->MonsterId, Bundles, Delegate);
			}

		}
	}
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation)
{
	LogOnScreen(this, "Finished loading.", FColor::Green);

	UAssetManager* Manager = UAssetManager::GetIfValid();
	if (Manager)
	{
		USMonsterData* MonsterData = Cast<USMonsterData>(Manager->GetPrimaryAssetObject(LoadedId));
		if (MonsterData)
		{
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator);
			if (NewBot)
			{
				LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

				// Grant special actions, buffs etc.
				USActionComponent* ActionComp = Cast<USActionComponent>(NewBot->GetComponentByClass(USActionComponent::StaticClass()));
				if (ActionComp)
				{
					for (TSubclassOf<USAction> ActionClass : MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}
			}
		}
	}
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	ASCharacter* PlayerVictim = Cast<ASCharacter>(VictimActor);

	// if player was killed, respawn player
	if (PlayerVictim)
	{
		SpawnPlayerCredits(PlayerVictim);

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

void ASGameModeBase::SpawnPlayerCredits(ASCharacter* Player)
{
	ASPlayerState* PlayerState = Player->GetPlayerState<ASPlayerState>();

	if (PlayerState)
	{
		if (PlayerState->PlayerCredits > 0)
		{
			FVector SpawnLocation = Player->GetActorLocation();

			AActor* DeadPlayerCredits = GetWorld()->SpawnActor<AActor>(PlayerCreditsPickup, SpawnLocation, FRotator::ZeroRotator);

			if (DeadPlayerCredits)
			{
				ASCoin* CoinCredits = Cast<ASCoin>(DeadPlayerCredits);

				if (CoinCredits)
				{
					CoinCredits->SetCoinCredits(PlayerState->PlayerCredits);

					CoinCredits->bShouldDestroyAfterFirstUse = true;
				}
			}
		}
	}
}

void ASGameModeBase::UpdateGamePlayers(ASCharacter* Player)
{
	AController* Controller = Player->GetController();

	if (ensure(Controller))
	{
		ASPlayerState* PlayerState = Controller->GetPlayerState<ASPlayerState>();
		if (ensure(PlayerState))
		{
			Controller->UnPossess();

			NumberOfAlivePlayers -= 1;

			PlayerState->ServerToggleIsAlive();

			FString DebugMsg = "Number of Alive Players: " + FString::FromInt(NumberOfAlivePlayers);
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, DebugMsg);

// 			if (NumberOfAlivePlayers <= 0)
// 			{
// 				UGameplayStatics::OpenLevel(GetWorld(), "MainMenu_Entry");
// 			}
		}
	}
}

void ASGameModeBase::GivePlayerKillCredits(ASCharacter* Player)
{
	ASPlayerState* PlayerState = Player->GetPlayerState<ASPlayerState>();

	PlayerState->AddCredits(KillCredits);

	UE_LOG(LogTemp, Log, TEXT("PlayerCredits: %d"), PlayerState->PlayerCredits);
}

void ASGameModeBase::RespawnPlayer_Implementation(AController* Controller)
{
	if (ensure(Controller))
	{
		RestartPlayer(Controller);

		NumberOfAlivePlayers += 1;

		ASPlayerState* PlayerState = Controller->GetPlayerState<ASPlayerState>();
		PlayerState->ServerToggleIsAlive();
	}
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

AlertLevel ASGameModeBase::GetGameAlertLevel()
{
	return GameAlertLevel;
}

AlertLevel ASGameModeBase::SetGameAlertLevel(AlertLevel NewAlertLevel)
{
	GameAlertLevel = NewAlertLevel;
	OnAlertLevelChanged.Broadcast(this, GameAlertLevel);
	return GameAlertLevel;
}
