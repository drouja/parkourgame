// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"
#include "../runchar.h"
#include "Components/BoxComponent.h"
// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Aroot = CreateDefaultSubobject<USceneComponent>("Root");
	this->SetRootComponent(Aroot);
	hitbox = CreateDefaultSubobject<UBoxComponent>("Hitbox");
	hitbox->SetupAttachment(Aroot);
	hitbox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpoint::BeginOverlap);
}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACheckpoint::BeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (Cast<Arunchar>(OtherActor) != nullptr)
	{
		checkreachdelegate.ExecuteIfBound(checktype);
		Destroy();
	}
}