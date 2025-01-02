// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameState.h"
#include "NetPlayerState.h"
#include "NetBaseCharacter.h"
#include "NetGameMode.h"
#include "Net/UnrealNetwork.h"

ANetGameState::ANetGameState() :
	WinningPlayer(-1)
{

}

void ANetGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ANetGameState, WinningPlayer);
	DOREPLIFETIME(ANetGameState, TimerSecond);
}

void ANetGameState::OnRep_Timer()
{
	if (TimerSecond <= 0)
	{
		OnTimesUp();
	}
}

void ANetGameState::OnRep_Winner()
{
	if (WinningPlayer >= 0)
	{
		OnVictory();
	}
}

ANetPlayerState* ANetGameState::GetPlayerStateByIndex(int PlayerIndex)
{
	for (APlayerState* PS : PlayerArray)
	{
		ANetPlayerState* State = Cast<ANetPlayerState>(PS);
		if (State && State->PlayerIndex == PlayerIndex)
		{
			return State;
		}
	}
	return nullptr;
}

void ANetGameState::TriggerRestart_Implementation()
{
	TimerSecond = 30;
	OnRestart();
}




