// Fill out your copyright notice in the Description page of Project Settings.


#include "NetAvatar.h"
#include "GameFramework/CharacterMovementComponent.h"

ANetAvatar::ANetAvatar() :
	MovementScale(1.0f)
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
}

void ANetAvatar::BeginPlay()
{
	Super::BeginPlay();

	Camera->bUsePawnControlRotation = false;
	SpringArm->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ANetAvatar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("Turn", this, &ACharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Lookup", this, &ACharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAxis("MoveForward", this, &ANetAvatar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ANetAvatar::MoveRight);

	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ANetAvatar::RunPressed);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ANetAvatar::RunReleased);
}

void ANetAvatar::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetAvatar, bIsRunning);
}

void ANetAvatar::MoveForward(float Scale)
{
	FRotator Rotation = GetController()->GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
	FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementScale*Scale);
}

void ANetAvatar::MoveRight(float Scale)
{
	FRotator Rotation = GetController()->GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);
	FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(ForwardDirection, MovementScale*Scale);
}

void ANetAvatar::SetRunAction(bool bRunningNew)
{
	bIsRunning = bRunningNew;


	UCharacterMovementComponent* LocalCharacterMovement = GetCharacterMovement();
	if (bIsRunning)
	{
		LocalCharacterMovement->MaxWalkSpeed = 900.0f;
	}
	else
	{
		LocalCharacterMovement->MaxWalkSpeed = 600.0f;
	}
}

void ANetAvatar::OnRep_IsRunning()
{
	SetRunAction(bIsRunning);
}

void ANetAvatar::ServerSetRunAction_Implementation(bool bRunningNew)
{
	SetRunAction(bRunningNew);
}

bool ANetAvatar::ServerSetRunAction_Validate(bool bRunningNew)
{
	return true;
}

void ANetAvatar::RunPressed()
{
	if (HasAuthority())
	{
		SetRunAction(true);
	}
	else
	{
		ServerSetRunAction(true);
	}
}

void ANetAvatar::RunReleased()
{
	if (HasAuthority())
	{
		SetRunAction(false);
	}
	else
	{
		ServerSetRunAction(false);
	}
}

void ANetAvatar::Run(float RunSpeed)
{
	UCharacterMovementComponent* RunCharacterMovement = GetCharacterMovement();
	if (RunCharacterMovement)
	{
		RunCharacterMovement->MaxWalkSpeed = RunSpeed;
	}
}
