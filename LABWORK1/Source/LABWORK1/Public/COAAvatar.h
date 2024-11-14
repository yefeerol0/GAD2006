// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "COABaseCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "COAAvatar.generated.h"


UCLASS()
class LABWORK1_API ACOAAvatar : public ACOABaseCharacter
{
	GENERATED_BODY()

public:
	ACOAAvatar();

	UPROPERTY(EditAnywhere, Category = "Character")
	float RunSpeed;

	UPROPERTY(EditAnywhere, Category = "Character")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Character")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float StaminaGainRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	float StaminaDrainRate;

	UPROPERTY(EditAnywhere, Category = "Character")
	bool bStaminaDrained;

	UPROPERTY(EditAnywhere, Category = "Character")
	bool bRunning;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character")
	USpringArmComponent* SpringArm;

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void MoveForward(float value);
	void MoveRight(float value);
	void RunPressed();
	void RunReleased();
	void UpdateMovementParams();
};