// Fill out your copyright notice in the Description page of Project Settings.


#include "TileGameManager.h"
#include "GameFramework/PlayerInput.h"
#include "TilePlayerController.h"

// Sets default values
ATileGameManager::ATileGameManager() :
	GridSize(100),
	GridOffset(0,0,0.5f)
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	GridSelection = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GridMesh"));
	GridSelection->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		PlaneMesh(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));

	static ConstructorHelpers::FObjectFinder<UMaterialInterface>
		GridMaterial(TEXT("Material'/Game/UI/MAT_GridSlot.MAT_GridSlot'"));

	GridSelection->SetStaticMesh(PlaneMesh.Object);
	GridSelection->SetMaterial(0, GridMaterial.Object);
	GridSelection->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void ATileGameManager::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto PlayerController = Cast<ATilePlayerController>(GWorld->GetFirstPlayerController()))
	{
		PlayerController->GameManager = this;
	}

}

// Called every frame
void ATileGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATileGameManager::OnActorInteraction(AActor* HitActor, FVector& Location, bool IsPressed)
{
	if (TileTypes.Num() == 0) return;

	FVector GridLoc = GridOffset;
	GridLoc.X += FMath::GridSnap(Location.X, GridSize);
	GridLoc.Y += FMath::GridSnap(Location.Y, GridSize);
	GridLoc.Z += Location.Z;

	UPlayerInput* Input = GWorld->GetFirstPlayerController()->PlayerInput;

	if (Input->WasJustPressed(EKeys::RightMouseButton)) //This is to rotate the GridSelection, so I can rotate the tiles below.
	{
		FRotator CurrentRotation = GridSelection->GetRelativeRotation();
		FRotator RotationChange(0.0f, 90.0f, 0.0f);
		GridSelection->SetRelativeRotation(CurrentRotation + RotationChange);
	}
	if (Input->WasJustPressed(EKeys::LeftMouseButton))
	{
		int GridX = GridLoc.X / GridSize + MapExtendsInGrids;
		int GridY = GridLoc.Y / GridSize + MapExtendsInGrids;

		if (GridX < 0 || GridY < 0 || GridX >= MapExtendsInGrids * 2 || GridY >= MapExtendsInGrids * 2)
		{
			return;
		}

		if (Map[GridX][GridY] != nullptr) return;

		if (TileTypes.IsValidIndex(CurrentTileIndex))
		{
			ATileBase* SelectedTile = TileTypes[CurrentTileIndex];
			Map[GridX][GridY] = SelectedTile;

			FTransform RotationTransform = FTransform(GridSelection->GetRelativeRotation()); //This part is to rotate the tiles by leveling their rotation with GridSelection.
			FTransform TileTransform = FTransform(FQuat::Identity, GridLoc + GridOffset);
			FTransform FinalTransform = SelectedTile->InstancedMesh->GetRelativeTransform() * RotationTransform * TileTransform;
			SelectedTile->InstancedMesh->AddInstance(FinalTransform, true);


		}

		UE_LOG(LogTemp, Warning, TEXT("Hit: %s - %f,%f,%f"),
			HitActor ? *HitActor->GetActorLabel() : TEXT("None"),
			Location.X,
			Location.Y,
			Location.Z);
	}
	else if (Input->WasJustPressed(EKeys::MouseScrollDown))
	{
		CurrentTileIndex = (CurrentTileIndex + 1) % TileTypes.Num();
		UE_LOG(LogTemp, Warning, TEXT("TileType: %s"), *TileTypes[CurrentTileIndex]->GetActorLabel());
		GridSelection->SetStaticMesh(TileTypes[CurrentTileIndex]->BaseMesh); //This is to set GridSelection's StaticMesh same with the BaseMesh
		GridSelection->SetWorldScale3D(TileTypes[CurrentTileIndex]->InstancedMesh->GetRelativeScale3D()); //This is to scale GridSelection's StaticMesh same with the BaseMesh
	}
	else if (Input->WasJustPressed(EKeys::MouseScrollUp)) //This is to cycle the tiles backwards.
	{
		if (CurrentTileIndex == 0)
		{
			CurrentTileIndex = TileTypes.Num() - 1;
		}
		else 
		{
			CurrentTileIndex = (CurrentTileIndex - 1) % TileTypes.Num();
		}
		UE_LOG(LogTemp, Warning, TEXT("TileType: %s"), *TileTypes[CurrentTileIndex]->GetActorLabel());
		GridSelection->SetStaticMesh(TileTypes[CurrentTileIndex]->BaseMesh); //This is to set GridSelection's StaticMesh same with the BaseMesh
		GridSelection->SetWorldScale3D(TileTypes[CurrentTileIndex]->InstancedMesh->GetRelativeScale3D()); //This is to scale GridSelection's StaticMesh same with the BaseMesh
	} 
	 
	else 
	{
		GridSelection->SetWorldLocation(GridLoc + GridOffset);
	}
}

