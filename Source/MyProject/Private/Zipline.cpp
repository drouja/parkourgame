// Fill out your copyright notice in the Description page of Project Settings.


#include "Zipline.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "../../../Plugins/Runtime/CableComponent/Source/CableComponent/Classes/CableComponent.h"
#include "../runchar.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyProject/UEHelperfuncts.h"

// Sets default values
AZipline::AZipline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	Arootcomponent = CreateDefaultSubobject<USceneComponent>("Root");
	this->SetRootComponent(Arootcomponent);

	spline = CreateDefaultSubobject<USplineComponent>("Spline");
	spline->SetupAttachment(Arootcomponent);

	cable = CreateDefaultSubobject<UCableComponent>("Cable");
	cable->SetupAttachment(spline);
	cable->NumSegments = 1;
	Updatecable();

	hitbox = CreateDefaultSubobject<UCapsuleComponent>("Hitbox");
	hitbox->SetupAttachment(Arootcomponent);
	hitbox->OnComponentBeginOverlap.AddDynamic(this, &AZipline::BeginOverlap);
	hitbox->OnComponentEndOverlap.AddDynamic(this, &AZipline::EndOverlap);
	Updatehitbox();

	zoffset = 110;
}

// Called when the game starts or when spawned
void AZipline::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AZipline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#if WITH_EDITOR
void AZipline::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	if (ReregisterComponentsWhenModified())
	{
		Updatecable();
		Updatehitbox();
	}
}
#endif

void AZipline::Updatecable()
{
	cable->SetWorldLocation(spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World));
	cable->bAttachEnd = true;
	cable->EndLocation = spline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::Local);
}

void AZipline::Updatehitbox()
{
	hitbox->SetCapsuleHalfHeight(spline->GetSplineLength() / 2);
	hitbox->SetWorldLocation(spline->GetLocationAtDistanceAlongSpline(spline->GetSplineLength() / 2, ESplineCoordinateSpace::World));
	FRotator rot = spline->GetRotationAtDistanceAlongSpline(spline->GetSplineLength() / 2, ESplineCoordinateSpace::World);
	hitbox->SetWorldRotation(FRotator{ rot.Pitch + 90, rot.Yaw, rot.Roll });
}

void AZipline::BeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	asrunchar = Cast<Arunchar>(OtherActor);
	if (asrunchar != nullptr)
	{
		asrunchar->isziplining = true;
		asrunchar->endoverlapdelegate.BindUObject(this, &AZipline::EndOverlap);
		FRotator lookrot = UKismetMathLibrary::FindLookAtRotation(spline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World), spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World));

		float distalongspline = (asrunchar->GetActorLocation() - spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World)).Size();
		asrunchar->SetActorLocation(spline->GetLocationAtDistanceAlongSpline(distalongspline, ESplineCoordinateSpace::World) - FVector{ 0,0,110 });
		asrunchar->sliderot = lookrot.Yaw;
		asrunchar->setgrav(false, 0.0f);
		lookrot.Yaw = UKismetMathLibrary::NormalizeAxis(lookrot.Yaw);
		asrunchar->yaw = lookrot.Yaw;
		asrunchar->calculaterootyawoffset();
		asrunchar->takeyaw = true;


		
		GetWorldTimerManager().SetTimer(updatezipmovementhandle, this, &AZipline::updatezipmovement, 0.01f, true, 0.0f);
	}
}
void AZipline::EndOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex)
{
	asrunchar = Cast<Arunchar>(OtherActor);
	if (asrunchar != nullptr && asrunchar->isziplining)
	{
		asrunchar->endoverlapdelegate.Unbind();
		asrunchar->setgrav(true);
		asrunchar->isziplining = false;
		GetWorldTimerManager().ClearTimer(updatezipmovementhandle);
	}
}

void AZipline::updatezipmovement()
{
	launchdirect = spline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World) - spline->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World);
	launchdirect.Normalize();
	launchdirect.Z += 0.01; //not sure why but this makes it so that the character doesn't slowly slide down
	asrunchar->LaunchCharacter(800*launchdirect, true, true);
}