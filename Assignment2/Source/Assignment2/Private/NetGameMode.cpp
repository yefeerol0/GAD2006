// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameMode.h"
#include "NetBaseCharacter.h"
#include "NetGameState.h"
#include "NetPlayerState.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerStart.h"
#include "Components/CapsuleComponent.h"

ANetGameMode::ANetGameMode()
{
	DefaultPawnClass = ANetBaseCharacter::StaticClass();
	PlayerStateClass = ANetPlayerState::StaticClass();
	GameStateClass = ANetGameState::StaticClass();
}

AActor* ANetGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* Start = AssignTeamAndPlayerStart(Player);
	return Start ? Start : Super::ChoosePlayerStart_Implementation(Player);
}

void ANetGameMode::AvatarsOverlapped(ANetAvatar* AvatarA, ANetAvatar* AvatarB)
{

	ANetGameState* GState = GetGameState<ANetGameState>();

	if (GState == nullptr || GState->WinningPlayer >= 0) return;

	ANetPlayerState* StateA = AvatarA->GetPlayerState<ANetPlayerState>();
	ANetPlayerState* StateB = AvatarB->GetPlayerState<ANetPlayerState>();
	if (StateA->TeamID == StateB->TeamID) return;

	if (StateA->TeamID == EPlayerTeam::TEAM_Red)
	{
		GState->WinningPlayer = StateA->PlayerIndex;
	}
	else
	{
		GState->WinningPlayer = StateB->PlayerIndex;
	}

	AvatarA->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	AvatarB->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GState->OnVictory();

	for (APlayerController* Player : AllPlayers)
	{
		auto State = Player->GetPlayerState<ANetPlayerState>();

		if (State->TeamID == EPlayerTeam::TEAM_Blue)
		{
			State->Result = EGameResults::RESULT_Lost;
		}
		else
		{
			State->Result = EGameResults::RESULT_Won;
		}
	}

	FTimerHandle EndGameTimerHandle;
	GWorld->GetTimerManager().SetTimer(EndGameTimerHandle, this, &ANetGameMode::EndGame, 2.5f, false);
}

void ANetGameMode::EndGame()
{
	PlayerStartIndex = 0;
	TotalGames++;
	GetGameState<ANetGameState>()->WinningPlayer = -1;

	for (APlayerController *Player : AllPlayers)
	{
		APawn* Pawn = Player->GetPawn();
		Player->UnPossess();
		Pawn->Destroy();
		Player->StartSpot.Reset();
		RestartPlayer(Player);
	}

	ANetGameState* GState = GetGameState<ANetGameState>();

	GetGameState<ANetGameState>()->TimerSecond = 30;
	GState->TriggerRestart();

}

void ANetGameMode::Countdown()
{
	if (TotalPlayerCount > 1)
	{
		if (GetGameState<ANetGameState>()->TimerSecond > 0)
		{
			GetGameState<ANetGameState>()->TimerSecond--;
			GWorld->GetTimerManager().SetTimer(CountDownTimer, this, &ANetGameMode::Countdown, 1.0f, false);
			GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Green, FString::Printf(TEXT("%d"), GetGameState<ANetGameState>()->TimerSecond));
		}
		else
		{
			ANetGameState* GState = GetGameState<ANetGameState>();
			GState->OnTimesUp();
			for (APlayerController* Player : AllPlayers)
			{

				auto PState = Player->GetPlayerState<ANetPlayerState>();

				if (PState->TeamID == EPlayerTeam::TEAM_Blue)
				{
					PState->Result = EGameResults::RESULT_Won;
				}
				else
				{
					PState->Result = EGameResults::RESULT_Lost;
				}
			}
			GWorld->GetTimerManager().SetTimer(CountDownTimer, this, &ANetGameMode::EndGame, 2.5f, false);
			// Blue wins!
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(0, 1.0f, FColor::Cyan, FString::Printf(TEXT("Waiting for players...")));
		GWorld->GetTimerManager().SetTimer(CountDownTimer, this, &ANetGameMode::Countdown, 1.0f, false);
	}

}

void ANetGameMode::BeginPlay()
{
	Super::BeginPlay();
	GWorld->GetTimerManager().SetTimer(CountDownTimer, this, &ANetGameMode::Countdown, 1.0f, false);
}

AActor* ANetGameMode::GetPlayerStart(FString Name, int Index)
{
	FName PSName;
	if (Index < 0)
	{
		PSName = *Name;
	}
	else
	{
		PSName = *FString::Printf(TEXT("%s%d"), *Name, Index % 4);
	}

	for (TActorIterator<APlayerStart> It(GWorld); It; ++It)
	{
		if (APlayerStart* PS = Cast<APlayerStart>(*It))
		{
			if (PS->PlayerStartTag == PSName) return *It;
		}
	}

	return nullptr;
}

AActor* ANetGameMode::AssignTeamAndPlayerStart(AController* Player)
{
	AActor* Start = nullptr;
	ANetPlayerState* State = Player->GetPlayerState<ANetPlayerState>();
	if (State)
	{
		if (TotalGames == 0)
		{
			State->TeamID = TotalPlayerCount == 0 ? EPlayerTeam::TEAM_Blue : EPlayerTeam::TEAM_Red;
			State->PlayerIndex = TotalPlayerCount++;
			AllPlayers.Add(Cast<APlayerController>(Player));
		}
		else
		{
			State->TeamID = State->Result == EGameResults::RESULT_Won ? EPlayerTeam::TEAM_Blue : EPlayerTeam::TEAM_Red;
		}

		if (State->TeamID == EPlayerTeam::TEAM_Blue)
		{
			Start = GetPlayerStart("Blue", -1);
		}
		else
		{
			Start = GetPlayerStart("Red", PlayerStartIndex++);
		}
	}
	return Start;
}