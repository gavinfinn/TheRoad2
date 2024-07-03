// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "TheRoad2/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Characters/RoadCharacter.h"
#include "NiagaraComponent.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// returns pointer to new construction object
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	// sets root component to new item mesh
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	EmbersEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Embers"));
	EmbersEffect->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	// bind callback to delegate
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);



	/*
	UE_LOG(LogTemp, Warning, TEXT("Begin Play called"));

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Cyan, FString("Item OnScreen Message"));
	}
	*/

	// UWorld* World = GetWorld();


	// SetActorLocation(FVector(0.f, 0.f, 150.f));
	// SetActorRotation(FRotator(0.f, 45.f, 0.f));
	// FVector Location = GetActorLocation();
	// FVector Forward = GetActorForwardVector();

	// DRAW_SPHERE(Location, FColor::Cyan);
	// DRAW_VECTOR(Location, Location + Forward * 100)
}

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ARoadCharacter* RoadCharacter = Cast<ARoadCharacter>(OtherActor);
	if (RoadCharacter)
	{
		RoadCharacter->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ARoadCharacter* RoadCharacter = Cast<ARoadCharacter>(OtherActor);
	if (RoadCharacter)
	{
		RoadCharacter->SetOverlappingItem(nullptr);
	}
}


// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Movement rate in unites of cm/s
	// float MovementRate = 50.f;
	// float RotationRate = 45.f;

	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0, 0, TransformedSin()));
	}


	// float DeltaZ = Amplitude * FMath::Sin(RunningTime * TimeConstant);
	// AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));

	// UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %f"), DeltaTime);
	/* 
	if (GEngine) {
		FString Name = GetName();
		FString Message = FString::Printf(TEXT("Item Name: %s"), *Name);
		GEngine->AddOnScreenDebugMessage(1, 60.f, FColor::Cyan, Message);

		UE_LOG(LogTemp, Warning, TEXT("Item Name: %s"), *Name);
	}
	*/

	// MovementRate * DeltaTime = (cm/frame)
	// AddActorWorldOffset(FVector(MovementRate * DeltaTime, 0.f, 0.f));
	// AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));
	
	// DRAW_SPHERE_SingleFrame(GetActorLocation());
	// DRAW_VECTOR_SingleFrame(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100);

	// FVector AvgVector = Avg<FVector>(GetActorLocation(), FVector::ZeroVector);
	// DRAW_POINT_SingleFrame(AvgVector);
	
	/*AddActorWorldRotation(FQuat(TransformedCos()));*/
}

