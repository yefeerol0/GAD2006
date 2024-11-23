// Fill out your copyright notice in the Description page of Project Settings.


#include "Commands/MoveCommand.h"
#include "GameGrid.h"

MoveCommand::MoveCommand(FSGridPosition Src, FSGridPosition Dst) :
	Source(Src),
	Destination(Dst)
{

}

MoveCommand::~MoveCommand()
{

}

void MoveCommand::Execute()
{
	UE_LOG(LogTemp, Warning, TEXT("Executing MoveCommand..."));
	AGameSlot* SlotA = AGameGrid::FindSlot(Source);
	AGameSlot* SlotB = AGameGrid::FindSlot(Destination);

	AUnitbase* UnitA = SlotA->Unit;
	check(UnitA);
	UnitA->AssignToSlot(SlotB);
	SlotB->SetState(GS_Highlighted);
}

void MoveCommand::Revert()
{
	UE_LOG(LogTemp, Warning, TEXT("Reverting MoveCommand..."));
	AGameSlot* SlotA = AGameGrid::FindSlot(Source);
	AGameSlot* SlotB = AGameGrid::FindSlot(Destination);

	AUnitbase* UnitB = SlotB->Unit;
	check(UnitB);
	UnitB->AssignToSlot(SlotA);

	//this if statement is just to fix grid highlights if player makes a repetitive movement
	if (SlotA->GridState == GS_Default)
	{
		SlotA->SetState(GS_Highlighted);
	}

	SlotB->SetState(GS_Default);
}
