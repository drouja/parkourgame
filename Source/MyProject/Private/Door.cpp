// Fill out your copyright notice in the Description page of Project Settings.

#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "../runchar.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include <Components/WidgetComponent.h>

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Actorinrange = false;
	open = false;
	desiredrot = 120.0f;
	openspeed = 30.0f;
	openingdoor = false;

	Arootcomponent = CreateDefaultSubobject<USceneComponent>("Root");
	this->SetRootComponent(Arootcomponent);

	doorpivot = CreateDefaultSubobject<USceneComponent>("Door Pivot");
	doorpivot->SetupAttachment(Arootcomponent);

	doormesh = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh");
	doormesh->SetupAttachment(doorpivot);

	lockmesh = CreateDefaultSubobject<UStaticMeshComponent>("LockMesh");
	lockmesh->SetupAttachment(Arootcomponent);

	Hitbox = CreateDefaultSubobject<UBoxComponent>("Hitbox");
	Hitbox->SetupAttachment(doormesh);

	HUD = CreateDefaultSubobject<UWidgetComponent>("InteractPrompt");
	HUD->SetupAttachment(Arootcomponent);

}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(getcharacterinrangehandle, this, &ADoor::Getcharacterinrange, 0.01f, true, 0.0f);
	asrunchar = Cast<Arunchar>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn());
	ledmat=UMaterialInstanceDynamic::Create(lockmesh->GetMaterial(0), this);
	lockmesh->SetMaterial(0, ledmat);

}

// Called every frame
void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoor::Getcharacterinrange()
{
	Actorinrange = (GetActorLocation() - asrunchar->GetActorLocation()).Size() <= 1200;
	if (Actorinrange && !open)
	{
		asrunchar->inrangeofinteractible = true;
		if (asrunchar->interactbuttondown)
		{
			ledmat->SetVectorParameterValue(TEXT("color"), FVector{ 0,1,0 });
			openingdoor = true;
			Actorinrange = false;
			rotationprogress = 0.f;
			GetWorldTimerManager().SetTimer(smoothopendoorhandle, this, &ADoor::Smoothopendoor, 0.01f, true);
			GetWorldTimerManager().ClearTimer(getcharacterinrangehandle);
		}
		return;
	}
	asrunchar->inrangeofinteractible = false;
}

void ADoor::Smoothopendoor()
{
	if (rotationprogress >= desiredrot)
	{
		GetWorldTimerManager().ClearTimer(smoothopendoorhandle);
		open = true;
		asrunchar->inrangeofinteractible = false;
		return;
	}
	rotationprogress += openspeed * 0.01;
	doorpivot->AddLocalRotation(FRotator{ 0,openspeed * 0.01f,0 });
}
