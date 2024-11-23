// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameGrid.h"
#include "Unitbase.h"
#include "Commands/Command.h"
#include "Commands/MoveCommand.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

USTRUCT(BlueprintType)
struct FSUnitInfo
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AUnitbase> UnitClass;

	UPROPERTY(EditAnywhere)
	FSGridPosition StartPosition;

};

USTRUCT(BlueprintType)
struct FSLevelInfo
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditAnywhere)
	TArray<FSUnitInfo> Units;
};

UCLASS()
class AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

	void OnActorClicked(AActor* Actor, FKey button);

	virtual void Tick(float DeltaTime) override;

	void CreateLevelActors(FSLevelInfo& Info);

	UFUNCTION(BlueprintCallable)
	bool UndoLastMove();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CurrentLevel;

	UPROPERTY(EditAnywhere)
	TArray<FSLevelInfo> Levels;

	UPROPERTY(EditAnywhere)
	AGameGrid* GameGrid;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	AUnitbase* ThePlayer;
	TArray<TSharedRef<Command>> CommandPool;
	TSharedPtr<Command> CurrentCommand;
};
