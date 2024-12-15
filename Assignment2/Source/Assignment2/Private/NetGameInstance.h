// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NetBaseCharacter.h"
#include "NetGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	// Without the 'const' keyword, PlayerInfo variables were showing up as 'Outputs'
	// in the Blueprint, that is why I added 'const' keywords instead of removing &s.

	UFUNCTION(BlueprintCallable)
	void host(FString MapName, const FSPlayerInfo &PlayerInfo);

	UFUNCTION(BlueprintCallable)
	void join(FString Address, const FSPlayerInfo& PlayerInfo);

	UPROPERTY(BlueprintReadWrite)
	FSPlayerInfo PlayerInfo;
};
