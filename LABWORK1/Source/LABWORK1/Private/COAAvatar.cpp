// Fill out your copyright notice in the Description page of Project Settings.

#include "COAAvatar.h"
#include "GameFramework/CharacterMovementComponent.h"


ACOAAvatar::ACOAAvatar() :

	RunSpeed(500.0f),
	Stamina(100.0f),
	MaxStamina(100.0f),
	StaminaGainRate(5.0f),
	StaminaDrainRate(10.0f),
	bRunning(false),
	bStaminaDrained(false)
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	SpringArm->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ACOAAvatar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	PlayerInputComponent->BindAxis("Turn", this, &ACharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("MoveForward", this, &ACOAAvatar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACOAAvatar::MoveRight);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ACOAAvatar::RunPressed);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ACOAAvatar::RunReleased);
}

void ACOAAvatar::RunPressed() 
{
	if (!bStaminaDrained) 
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		bRunning = true;
	}

}

void ACOAAvatar::RunReleased() 
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	bRunning = false;
}

void ACOAAvatar::MoveForward(float value) 
{
	FRotator Rotation = GetController() -> GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
	FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, value);
}

void ACOAAvatar::MoveRight(float value)
{
	FRotator Rotation = GetController() -> GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
	FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, value);
}

void ACOAAvatar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Green, FString::Printf(TEXT("Stamina is : %f"), Stamina));

	if (bRunning && Stamina >= 0) 
	{
		Stamina = Stamina - StaminaDrainRate * DeltaTime;
	}
	else if (!bRunning && Stamina <= MaxStamina -1) 
	{
		Stamina = Stamina + StaminaGainRate * DeltaTime;
	}
	else if (Stamina <= 0)
	{
		Stamina = 0;
		UpdateMovementParams();
	}
	else if (Stamina > MaxStamina - 1) 
	{
		Stamina = 100;
		bStaminaDrained = false;
	}
}

void ACOAAvatar::UpdateMovementParams() 
{
	RunReleased();
	bStaminaDrained = true;
}