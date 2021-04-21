// Fill out your copyright notice in the Description page of Project Settings.


#include "Omnitool.h"
#include "Components/StaticMeshComponent.h"
#include "../runchar.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"

// Sets default values
AOmnitool::AOmnitool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Arootcomponent = CreateDefaultSubobject<USceneComponent>("Root");
	this->SetRootComponent(Arootcomponent);
	omnitoolmesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	omnitoolmesh->SetupAttachment(Arootcomponent);
	statuslight = CreateDefaultSubobject<UPointLightComponent>("Light");
	statuslight->SetupAttachment(Arootcomponent);

	eroderate = 5;
}

// Called when the game starts or when spawned
void AOmnitool::BeginPlay()
{
	Super::BeginPlay();
	omnitoolmat = UMaterialInstanceDynamic::Create(omnitoolmesh->GetMaterial(0), this);

	omnitoolmesh->SetMaterial(0,omnitoolmat);

	omnitoolmat->GetScalarParameterDefaultValue(TEXT("erode"),erode);
	
	asrunchar = Cast<Arunchar>(GetParentActor());

	GetWorldTimerManager().SetTimer(materializeomnitoolhandle, this, &AOmnitool::materializeomnitool, 0.01f, true, 0.0f);
}

// Called every frame
void AOmnitool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AOmnitool::materializeomnitool()
{
	bool inrange= asrunchar->inrangeofinteractible;
	if (inrange && asrunchar->interactbuttondown)
	{
		statuslight->SetVisibility(true);
	}
	else
	{
		statuslight->SetVisibility(false);
	}

	if (erode < 610 && !inrange)
	{
		erode += eroderate;
		omnitoolmat->SetScalarParameterValue(TEXT("erode"), erode); 
	}
	else if (erode > -200 && inrange)
	{
		erode -= eroderate;
		omnitoolmat->SetScalarParameterValue(TEXT("erode"), erode);
	}

}

