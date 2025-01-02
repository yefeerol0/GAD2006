// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NetGameInstance.generated.h"

UENUM(BlueprintType)
enum class EPlayerTeam : uint8
{
	TEAM_Unknown,
	TEAM_Blue,
	TEAM_Red,
};

UENUM(BlueprintType)
enum class EGameResults : uint8
{
	RESULT_Undefined,
	RESULT_Lost,
	RESULT_Won,
};

USTRUCT(BlueprintType)
struct FSPlayerInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Nickname;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CustomizationData;

	UPROPERTY(BlueprintReadOnly)
	EPlayerTeam TeamID;
};

UCLASS()
class UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	// Without the 'const' keyword, PlayerInfo variables were showing up as 'Outputs'
	// in the Blueprint, that is why I added 'const' keywords instead of removing &s.

	UFUNCTION(BlueprintCallable)
	void host(FString MapName, const FSPlayerInfo PlayerInfo);

	UFUNCTION(BlueprintCallable)
	void join(FString Address, const FSPlayerInfo PlayerInfo);

	UPROPERTY(BlueprintReadWrite)
	FSPlayerInfo PlayerInfo;
};
