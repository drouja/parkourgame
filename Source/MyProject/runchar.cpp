// Fill out your copyright notice in the Description page of Project Settings.

#include "runchar.h"
#include "Omnitool.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"
#include "Math/Vector.h"
#include <string>
#include <algorithm>
#include <math.h>
#include "UEHelperfuncts.h"

//Helper functions


FVector Arunchar::Setslidevector()
{
	FHitResult Outhit{};
	UKismetSystemLibrary::LineTraceSingle(this, GetMesh()->GetComponentLocation(), GetMesh()->GetComponentLocation() + FVector{ 0,0,-200 }, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::None, Outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f);
	slidevector = UKismetMathLibrary::Cross_VectorVector(Outhit.Normal, GetActorRightVector()) * -1;
	return slidevector;
}

void Arunchar::Allowalli()
{
	takeyaw = true;
	takepitch = true;
	takews = true;
	takead = true;
	takecrouch = true;
	takejump = true;
}

void Arunchar::Disablealli()
{
	takeyaw = false;
	takepitch = false;
	takews = false;
	takead = false;
	takecrouch = false;
	takejump = false;
}

void Arunchar::setgrav(bool resetgrav, float newgrav)
{
	if (resetgrav)  GetCharacterMovement()->GravityScale = oggravscale;
	else
	{
		GetCharacterMovement()->GravityScale = newgrav;
	}
}


FGenericTeamId Arunchar::GetGenericTeamId() const
{
	return FGenericTeamId{ 0 };
}
//----------------

// Sets default values
Arunchar::Arunchar()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	currenthealth = maxhealth = 5;

	sensitivity=3;

	pitch = 0;

	yaw = GetActorRotation().Yaw;
	yawlastframe = yaw;
	yawchangeoverframe = 0;
	rootyawoffset = 0;
	curveval = 0;
	curvevall = 0;
	turnchange = 5;
	turnratio = 1;
	maxslidez = -2;
	isturning = false;
	inrangeofinteractible = false;
	
	issliding = false;

	//Input control
	takeyaw = true;
	takepitch = true;
	takews = true;
	takead = true;
	takecrouch = true;
	takejump = true;
	interactbuttondown = false;
	//

	ismoving = 0;
	hardfalltime=1.5;
	willroll = 0;
	landenddelegate.BindUObject(this, &Arunchar::OnFallAnimationEnded);
	
	//vault
	vaulting = false;
	vaultenddelegate.BindUObject(this, &Arunchar::endvault);
	//

	flowtime = 0;

	//Wallrun
	canwallrun=false;
	iswallrunning=false;
	oggravscale = GetCharacterMovement()->GravityScale;
	cross = FVector{ 0,0,0 };
	//

	//Component stuff
	FollowCamera = CreateDefaultSubobject<UCameraComponent>("FPCam");
	FollowCamera->SetupAttachment(GetMesh(), FName("Camsocket"));
	omnitool = CreateDefaultSubobject<UChildActorComponent>("Omnitool");
	omnitool->SetChildActorClass(omnitoolclass);
	omnitool->SetupAttachment(GetMesh(), FName("LeftForeArm"));
	hitb = GetCapsuleComponent();
	hitbheight = hitb->GetUnscaledCapsuleHalfHeight(); //May cause issues later, figure out to use unscaled or scaled?

	//Wallclimb
	maxclimbtime = 0.4;
	wallclimbtime = 0.0f;
	wallclimbforce = 300.0f;
	maxholdtime = 0.5f;
	isquickturning = false;
	wallturndelegate.BindUObject(this, &Arunchar::endquickturn);

	//Zipline
	isziplining = false;

	//Coiljump
	coiljump = false;

	//Ledgegrab/climb
	isledgeclimbing = false;
	endledgeclimbdelegate.BindUObject(this, &Arunchar::endledgeclimb);

	//Death stuff
	enddeathdelegate.BindUObject(this, &Arunchar::resetplayer);
	isdying = false;
}

// Called when the game starts or when spawned
void Arunchar::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(getmovingtimerhandle, this, &Arunchar::setisMoving,0.01f,true,0.0f);
	GetWorldTimerManager().SetTimer(slideupdatehandle, this, &Arunchar::Updateslide, 0.01f, true, 0.0f);
	GetWorldTimerManager().SetTimer(updateaccelhandle, this, &Arunchar::updateaccel, 0.01f, true, 0.0f);
	GetWorldTimerManager().SetTimer(upwallrun, this, &Arunchar::updatewallrun, 0.005f, true, 0.0f);
	GetWorldTimerManager().SetTimer(updatevaulthandle, this, &Arunchar::vaultupdate, 0.005f, true, 0.0f);
	GetWorldTimerManager().SetTimer(updatewallblockhandle, this, &Arunchar::updatewallblock, 0.001f, true, 0.0f);
	GetWorldTimerManager().SetTimer(updatewallclimbhandle, this, &Arunchar::updatewallclimb, 0.005f, true, 0.0f);

	origbrakedecel = GetCharacterMovement()->BrakingDecelerationWalking;
	origfrict = GetCharacterMovement()->GroundFriction;
	origcrouchspeed = GetCharacterMovement()->MaxWalkSpeedCrouched;
	pc=GEngine->GetFirstLocalPlayerController(GetWorld());
	Wallturn->bEnableAutoBlendOut = false;

	//Dynamic material setup
	Speedlines = UMaterialInstanceDynamic::Create(Speedlinesmat, this);
	FollowCamera->AddOrUpdateBlendable(Speedlines);
	Healtheffect = UMaterialInstanceDynamic::Create(Healtheffectmat, this);
	FollowCamera->AddOrUpdateBlendable(Healtheffect);

	respawnloc = GetActorLocation();

	cm = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
}

// Called every frame
void Arunchar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void Arunchar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &Arunchar::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &Arunchar::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &Arunchar::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &Arunchar::Lookup);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &Arunchar::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &Arunchar::UnSpace);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &Arunchar::Crouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &Arunchar::StopCrouch);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &Arunchar::Interact);
	PlayerInputComponent->BindAction("Quickturn", IE_Pressed, this, &Arunchar::quickturn);
}

void Arunchar::MoveForward(float Value)
{
	float basecalc = (GetVelocity().Size() - 600) / 200;
	Speedlines->SetScalarParameterValue(TEXT("Weight"), clamp(basecalc, 0.0f, 1.0f));
	Speedlines->SetScalarParameterValue(TEXT("Flickerspeed"), clamp(basecalc, 0.0f, 0.25f));
	if (takews && !issliding && (Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = GetActorRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void Arunchar::MoveRight(float Value)
{
	if (takead && !issliding && (Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = GetActorRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void Arunchar::Lookup(float Axisval)
{
	if (takepitch)
	{ //For now, add this inversion to lookup when quickturning until find better way?
		AddControllerPitchInput((isquickturning? -1 : 1)*sensitivity*Axisval* (GetWorld()->GetDeltaSeconds() * (1 / GetActorTimeDilation())));
	}
	pitch = UKismetMathLibrary::NormalizeAxis(GetControlRotation().Pitch - GetActorRotation().Pitch);
}

void Arunchar::Turn(float Axisval)
{
	if (UKismetMathLibrary::Abs(Axisval) > 0.5) turnratio = 1 / UKismetMathLibrary::Abs(Axisval);
	else turnratio = 1;
	if (takeyaw)
	{
		AddControllerYawInput(sensitivity * Axisval * (GetWorld()->GetDeltaSeconds() * (1 / GetActorTimeDilation())));
		if (issliding|| iswallrunning || isziplining)
		{
			
			FRotator rot{ pc->GetControlRotation().Pitch,clampangle(pc->GetControlRotation().Yaw,sliderot-89,sliderot+89),pc->GetControlRotation().Roll };
			pc->SetControlRotation(rot);
		}
		calculaterootyawoffset();
	}
}

void Arunchar::calculaterootyawoffset()
{
	yawlastframe = yaw;
	yaw = pc->GetControlRotation().Yaw;
	yawchangeoverframe = yaw - yawlastframe;

	if (ismoving && !issliding && !iswallrunning &&!isziplining)
	{
		rootyawoffset = 0;
		return;
	}
	else
	{
		rootyawoffset = UKismetMathLibrary::NormalizeAxis(yawchangeoverframe + rootyawoffset);
	}
	if (isturning && !issliding && !iswallrunning)
	{
		curvevall = curveval;
		curveval = GetMesh()->GetAnimInstance()->GetCurveValue(TEXT("Distancecurve"));
		rootyawoffset += curveval - curvevall;
	}

}

//Crouch related functions
void Arunchar::Crouch()
{
	if (!takecrouch) return;
	if (GetCharacterMovement()->IsCrouching() && !(slidevector.Z <= maxslidez)) ACharacter::UnCrouch();
	else if (isziplining)
	{
		endoverlapdelegate.ExecuteIfBound(nullptr,this,nullptr,0);
	}
	else if (GetCharacterMovement()->IsFalling())
	{

		coiljump = true;
		hitb->SetCapsuleHalfHeight(hitbheight/2);
		
		FHitResult Outhit{};
		if (UKismetSystemLibrary::LineTraceSingle(this, GetMesh()->GetComponentLocation(), GetMesh()->GetComponentLocation() + FVector{ 0,0,-200 }, UEngineTypes::ConvertToTraceType(ECC_Camera),false,actorsToIgnore, EDrawDebugTrace::None,Outhit,true, FLinearColor::Red, FLinearColor::Green, 0.0f))
		{
			willroll = true;
		}
		return;
	}
	FVector vel = GetVelocity();
	if (!GetCharacterMovement()->IsCrouching() && vel.Size() >300 && UKismetMathLibrary::Dot_VectorVector(vel,GetActorForwardVector())>0)
	{
		Setslidevector();
		Startslide();
	}
	
}

void Arunchar::StopCrouch()
{
	///for now this is only for coiljump, will later add so you can choose to hold or toggle crouch
	if (coiljump)stopcoiljump();

}

void Arunchar::Startslide()
{
	UCharacterMovementComponent* charmov=GetCharacterMovement();
	if (charmov->IsWalking() && slidevector.Z <= 0.02)
	{
		sliderot = pc->GetControlRotation().Yaw;
		issliding = true;
		charmov->GroundFriction = 0;
		charmov->MaxWalkSpeedCrouched = charmov->MaxWalkSpeed+20;
		slidefv = GetActorForwardVector();
		sliderv = GetActorRightVector();
		ACharacter::Crouch();
	}
}

void Arunchar::Updateslide()
{
	Setslidevector();
	UCharacterMovementComponent* charmov = GetCharacterMovement();
	if (!issliding && slidevector.Z <= maxslidez && slidevector.Z >= -1)
	{
		StopAnimMontage(Hardlanding);
		StopAnimMontage(Rolllanding);
		charmov->AddImpulse(slidevector, true);
		Startslide();
		return;
	}
	if (!issliding) return;
	if (GetVelocity().Size()<35 || !charmov->IsCrouching())
	{
		Endslide();
		return;
	}
	if (slidevector.Z <= -0.2)
	{
		charmov->BrakingDecelerationWalking = 0;
		charmov->AddImpulse(GetActorForwardVector(),true);
	}
	else
	{
		charmov->BrakingDecelerationWalking=origbrakedecel / 6;
	}
	FHitResult Outhit{};
	FHitResult Outhit2{};
	if (UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), GetActorLocation() + FVector{ 0,0,-100 }, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::None, Outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f)
		&&
		UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation()+100*slidefv, GetActorLocation() + 100 * slidefv + FVector{ 0,0,-200 }, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::None, Outhit2, true, FLinearColor::Green, FLinearColor::Green, 0.0f)
		)
	{
		SetActorRotation(UKismetMathLibrary::MakeRotFromZX(UKismetMathLibrary::Cross_VectorVector(Outhit2.Location-Outhit.Location,sliderv),GetActorForwardVector()));
	}
}

void Arunchar::Endslide()
{
	UCharacterMovementComponent* charmov = GetCharacterMovement();
	issliding = false;
	Allowalli();
	ACharacter::UnCrouch();
	charmov->GroundFriction = origfrict;
	charmov->BrakingDecelerationWalking = origbrakedecel;
	charmov->MaxWalkSpeedCrouched = origcrouchspeed;
	SetActorRotation(UKismetMathLibrary::MakeRotFromZX(UKismetMathLibrary::GetUpVector(FRotator{0,0,0}), GetActorForwardVector()));

}
//------------------------

void Arunchar::Jump()
{
	if (takejump && !GetCharacterMovement()->IsFalling())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), hitsound1, UKismetMathLibrary::RandomFloatInRange(0.2, 0.4),UKismetMathLibrary::RandomFloatInRange(1,1.2));
		Super::Jump();
		if (issliding)
		{
			Endslide();
			LaunchCharacter(FVector{ 0,0,350 },false,false);
		}

		if (FVector::DotProduct(GetActorForwardVector(),GetVelocity())>0.0)
		{
		sliderv = GetActorRightVector();
		canwallclimb = true;
		GetWorldTimerManager().SetTimer(wallrunstart, this, &Arunchar::delaywallrun1, 0.001f, true);
		}
	}
	else if (iswallrunning || isquickturning)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), hitsound1);
		if (isquickturning)
		{
			yaw = pc->GetControlRotation().Yaw + 180;
			pc->SetControlRotation(FRotator{ -1 * pitch,yaw,0 });
			calculaterootyawoffset();
		}
		StopAnimMontage(Wallturn);
		GetWorldTimerManager().ClearTimer(delayfall);
		isquickturning = false;
		ACharacter::LaunchCharacter(700*(FollowCamera->GetForwardVector())+400*FollowCamera->GetUpVector(),true,true);
		canwallrun = false;
	}
}

void Arunchar::UnSpace()
{
	canwallclimb = false;
	if (iswallclimbing)
	{
		LaunchCharacter(FVector{ 0,0,30 }, true, true);
	}
	
}

void Arunchar::Falling()
{
	if (issliding) Endslide();
	startheight = GetActorLocation().Z;
	takeyaw = false;
}

void Arunchar::setisMoving()
{
	ismoving = (GetCharacterMovement()->GetCurrentAcceleration().Size() > 0);
}

void Arunchar::Landed(const FHitResult& Hit)
{
	stopcoiljump();
	//Super::OnLanded(Hit); do I need? Can't find function definition
	if (GetActorLocation().Z-startheight<-900)
	{
		Disablealli();
		if (willroll)
		{
			PlayAnimMontage(Rolllanding);
			GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(landenddelegate, Rolllanding);
			FollowCamera->bUsePawnControlRotation = 0;
		}
		else
		{
			PlayAnimMontage(Hardlanding);
			GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(landenddelegate, Hardlanding);
		}
	}
	else takeyaw = true;
	willroll = false;

	GetWorldTimerManager().ClearTimer(wallrunstart);
	GetWorldTimerManager().ClearTimer(wallruntime);
	canwallrun = false;
	
}

void Arunchar::OnFallAnimationEnded(UAnimMontage* mont, bool interupted)
{
	FollowCamera->bUsePawnControlRotation = 1;
	Allowalli();
}

void Arunchar::updateaccel()
{
	flowtime += 0.01f;
	if (GetVelocity().Size() < 100) flowtime = 0;
	GetCharacterMovement()->MaxWalkSpeed=accelcurve->GetFloatValue(flowtime);
}

void Arunchar::delaywallrun1()
{
	FHitResult Outhit;
	if (vaulting)
	{
		GetWorldTimerManager().ClearTimer(wallrunstart);
	}
	else if (!vaulting && GetVelocity().Z<0 || !UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), GetActorLocation() - 130 * GetActorUpVector(), UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::Persistent, Outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f))
	{
		canwallrun = true;
		GetWorldTimerManager().SetTimer(wallruntime, this, &Arunchar::delaywallrun2, 1.2f, false);
		GetWorldTimerManager().ClearTimer(wallrunstart);
	}
	
	
}

void Arunchar::delaywallrun2()
{
	canwallrun = false;
}

void Arunchar::updatewallrun()
{
	
	if (canwallrun && !coiljump && GetCharacterMovement()->IsFalling())
	{
		FHitResult outhit;
		FVector wallrunloc{};
		FVector wallrunnorm{};

		//Just raycasts for walls
		if (UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), GetActorLocation() + -75 * sliderv, UEngineTypes::ConvertToTraceType(ECC_Camera), false, actorsToIgnore, EDrawDebugTrace::None, outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f))
		{
			wallrunloc = outhit.Location;
			wallrunnorm = outhit.Normal;
			wallrunnorm.Normalize();
			walldirectflip = 1;
		}
		else if (UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), GetActorLocation() + 75 * sliderv, UEngineTypes::ConvertToTraceType(ECC_Camera), false, actorsToIgnore, EDrawDebugTrace::None, outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f))
		{
			wallrunloc = outhit.Location;
			wallrunnorm =outhit.Normal;
			wallrunnorm.Normalize();
			walldirectflip = -1;
		}
		else
		{
			iswallrunning = false;
			GetCharacterMovement()->GravityScale = oggravscale;
			return;
		}
		//

		FVector playervel = GetVelocity();
		playervel.Z = 0;
		double wallcheck = abs(UKismetMathLibrary::Cross_VectorVector(playervel, wallrunnorm).Size());
		
		if (-0.52 <= wallrunnorm.Z && wallrunnorm.Z <= 0.52 && 300<wallcheck)
		{
			iswallrunning = true;
			takeyaw = true;
			if (walldirectflip == 1)
			{
				sliderot = normalize360(UKismetMathLibrary::MakeRotFromYZ(wallrunnorm, GetActorUpVector()).Yaw);
			}
			else
			{
				sliderot = normalize360(UKismetMathLibrary::MakeRotFromYZ(-1*wallrunnorm, GetActorUpVector()).Yaw);
			}
			
			LaunchCharacter(wallrunloc - GetActorLocation(),false,false);
			prevcross = cross;
			cross = UKismetMathLibrary::Cross_VectorVector(wallrunnorm, FVector{ 0,0,walldirectflip });
			LaunchCharacter(GetVelocity().Size()*cross, true, true);
			if (!prevcross.Equals(cross, 0))
			{
				
				yaw = (UKismetMathLibrary::MakeRotFromXZ(cross, FVector{ 0,0,1 })).Yaw;
				calculaterootyawoffset();
			}
			GetCharacterMovement()->GravityScale = 0;
		}
		else
		{
			iswallrunning = false;
			canwallrun = false;
		}
	}
	else
	{
		cross = FVector{0,0,300};
		iswallrunning = false;
		GetCharacterMovement()->GravityScale = oggravscale;
		return;
	}
	
	
}

void Arunchar::vaultupdate()
{
	//Logic while vaulting animation
	if (isledgeclimbing) return;
	if (vaulting)
	{
		stopcoiljump();

		FHitResult outhit;
		(UKismetSystemLibrary::BoxTraceSingle(this, GetActorLocation() - 37 * GetActorForwardVector() - FVector{ 0,0,20 }, GetActorLocation() - FVector{ 0,0,20 } + GetActorForwardVector() * 200, FVector(5, 5, 20), GetActorRotation(), UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::None, outhit, true, FLinearColor::Green, FLinearColor::Green, 0.0f));
		//Stop vaulting once cleared obstacle
		if (!outhit.bBlockingHit || outhit.Actor!=remcollisonactor) //Check if the actor we've hit with the trace is the actor we're vaulting over
		{
			vaulting = false; 
			MoveIgnoreActorRemove(remcollisonactor);
			takeyaw = true;
			takews = true;
			takead = true;
		}
		// If the vaulting object is below us, keep moving forward
		else 
		{
		vaulttime += 0.005f;
		SetActorLocation(FVector{GetActorLocation().X,GetActorLocation().Y, UKismetMathLibrary::FInterpTo(GetActorLocation().Z,vaultloc.Z,vaulttime,.8f) });
		}
		return;
	}

	//If we aren't in the air, wallrunning, have negative z velocity, or are too slow/at weird angle, don't vault
	if (!GetCharacterMovement()->IsFalling() || iswallrunning)
	{
		vaulting = false;
		return;
	}

	checkifledgeclimb();
	if (isledgeclimbing) return;

	checkfvault(); //Check if can do foward vault over obstacle
}

void Arunchar::checkifledgeclimb()
{
	FVector charvel = GetVelocity();
	FVector charloc = GetActorLocation();
	FVector FV = 50 * GetActorForwardVector();
	FRotator rot = GetActorRotation();
	FHitResult outhit;
	if (100>(abs(charvel.X) + abs(charvel.Y))
		&&
		!UKismetSystemLibrary::BoxTraceSingle(this, charloc + FVector{ 0,0,65 }, charloc + FVector{ 0,0,65 } + FV, FVector(20, 20, 10), rot, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::None, outhit, true, FLinearColor::Green, FLinearColor::Green, 0.0f)
		&&
		UKismetSystemLibrary::BoxTraceSingle(this, charloc + FVector{ 0,0,45 }, charloc + FVector{ 0,0,45 } + FV, FVector(20, 20, 10), rot, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::None, outhit, true, FLinearColor::Green, FLinearColor::Green, 0.0f)
		&&
		UKismetSystemLibrary::BoxTraceSingle(this, charloc + FVector{ 0,0,25 }, charloc + FVector{ 0,0,25 } + FV, FVector(20, 20, 10), rot, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::None, outhit, true, FLinearColor::Green, FLinearColor::Green, 0.0f)
		)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		PlayAnimMontage(Ledgeclimb);
		isledgeclimbing = true;
		remcollisonactor = outhit.GetActor();
		MoveIgnoreActorAdd(remcollisonactor);
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(endledgeclimbdelegate, Ledgeclimb);
	}
}

void Arunchar::checkfvault()
{
	//Check if object in front and if we can vault over it
	FHitResult outhit;
	FHitResult outhit2;
	FHitResult outhit3;
	if (!(GetVelocity().Z < -1) && !(UKismetMathLibrary::Dot_VectorVector(normalizevector(GetVelocity()), GetActorForwardVector()) < 0.4) &&
		UKismetSystemLibrary::BoxTraceSingle(this, GetActorLocation() + FVector{ 0,0,20 }, GetActorLocation() + FVector{ 0,0,20 } + GetActorForwardVector() * 200, FVector(10, 10, 20), FRotator{ 0,0,0 }, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::ForOneFrame, outhit, true, FLinearColor::Green, FLinearColor::Green, 0.0f) &&
		!UKismetSystemLibrary::BoxTraceSingle(this, GetActorLocation() + FVector{ 0,0,80 }, GetActorLocation() + FVector{ 0,0,80 } + GetActorForwardVector() * 200, FVector(10, 10, 40), FRotator{ 0,0,0 }, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::ForOneFrame, outhit2, true, FLinearColor::Red, FLinearColor::Red, 0.0f)
		)
	{
		if (UKismetSystemLibrary::LineTraceSingle(this, outhit.Location + FVector{ 0,0,50 } - 15 * outhit.Normal, outhit.Location + FVector{ 0,0,-30 } - 15 * outhit.Normal, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::None, outhit2, true, FLinearColor::Green, FLinearColor::Green, 0.0f)
			&&
			!UKismetSystemLibrary::BoxTraceSingle(this, outhit.Location + FVector{ 0,0,-50 } - 150 * outhit.Normal, outhit.Location + FVector{ 0,0,-50 } - 300 * outhit.Normal, FVector(10, 10, 40), FRotator{ 0,0,0 }, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::None, outhit3, true, FLinearColor::Red, FLinearColor::Red, 0.0f)
			)
		{

			StopJumping();
			vaultloc = outhit2.Location; //Used for hand Ik, and for determening where to move character
			vaulting = true;
			vaultanim = true;
			canwallrun = false;

			//Disable some mouse inputs
			takeyaw = false;
			takews = false;
			takead = false;
			//-------------------------
			vaulttime = 0;
			//Setupmontage end delegate
			PlayAnimMontage(vault1);
			GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(vaultenddelegate, vault1);
			remcollisonactor = outhit.GetActor();
			MoveIgnoreActorAdd(remcollisonactor);
			//-------------------------

		}
		else
		{
			vaulting = false;
		}
	}
	else
	{
		vaulting = false;
	}
}

void Arunchar::endledgeclimb(UAnimMontage* mont, bool interupted)
{
	MoveIgnoreActorRemove(remcollisonactor);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	isledgeclimbing = false;
	Allowalli();
}

void Arunchar::endvault(UAnimMontage* mont, bool interupted)
{
	vaultanim = false;
}

void Arunchar::updatewallblock()
{
	righthandwallblock = false;
	lefthandwallblock = false;

	if (!pc->IsInputKeyDown(EKeys::W) || vaultanim || iswallclimbing || isquickturning) return;

	FVector rightsocketloc = GetMesh() -> GetSocketLocation(FName("RightShoulderSocket"));
	FVector leftsocketloc = GetMesh()->GetSocketLocation(FName("LeftShoulderSocket"));

	FHitResult outhit;
	
	if (UKismetSystemLibrary::LineTraceSingle(this, rightsocketloc, rightsocketloc + 50 * GetActorForwardVector(), UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::None, outhit, true, FLinearColor::Green, FLinearColor::Green, 0.0f))
	{
		righthandwallblockloc = outhit.Location - 3*GetActorForwardVector();
		righthandwallblock = true;
	}
	if (UKismetSystemLibrary::LineTraceSingle(this, leftsocketloc, leftsocketloc + 50 * GetActorForwardVector(), UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::None, outhit, true, FLinearColor::Green, FLinearColor::Green, 0.0f))
	{
		lefthandwallblockloc = outhit.Location-3*GetActorForwardVector();
		lefthandwallblock = true;
	}
}

void Arunchar::Interact()
{
	if (inrangeofinteractible)
	{
		PlayAnimMontage(Interactanim);
	}
	interactbuttondown = true;
	FTimerHandle butresettime;
	GetWorldTimerManager().SetTimer(butresettime, this, &Arunchar::UnInteract, 0.5f, false);
	
}

void Arunchar::UnInteract()
{
	interactbuttondown = false;
}

void Arunchar::updatewallclimb()
{
	//Dont wallclimb if quickturning
	if (isquickturning || isledgeclimbing || isledgeclimbing) return;

	//Raycasting for wall
	FVector actorloc = GetActorLocation();
	FVector actorup = GetActorUpVector();
	FVector aimvector = 70 * normalizevector(FVector{ FollowCamera->GetForwardVector().X,FollowCamera->GetForwardVector().Y,0 });
	FHitResult Outhit{};
	FHitResult Outhit2{};
	bool wallcheck =UKismetSystemLibrary::LineTraceSingle(this, actorloc + 80 * actorup, actorloc + 80 * actorup + aimvector, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::None, Outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f) &&
	UKismetSystemLibrary::LineTraceSingle(this, actorloc - 100 * actorup, actorloc - 100 * actorup + aimvector, UEngineTypes::ConvertToTraceType(ECC_WorldStatic), false, actorsToIgnore, EDrawDebugTrace::None, Outhit2, true, FLinearColor::Red, FLinearColor::Green, 0.0f);
	//

	//Allowing quickturn
	if (!wallcheck)
	{
		iswallclimbing = false;
		canquickturn = false;
		return;
	}
	else canquickturn = true;
	//
	
	//Disable wallclimbing if either wallclimbing for too long, or doing some other action
	if (!canwallclimb || iswallrunning || coiljump)
	{
		iswallclimbing = false;
		wallclimbtime = 0.0f;
		return;
	}
	if (wallclimbtime >= maxclimbtime && iswallclimbing)
	{
		LaunchCharacter(FVector{ 0,0,10 }, true, true);
		canwallclimb = false;
		return;
	}
	//
	
	//Wallclimb
	wallclimbtime += 0.005f;
	iswallclimbing = true;
	FVector wallclimbloc = { Outhit.Location.X,Outhit.Location.Y,actorloc.Z };
	LaunchCharacter(wallclimbloc-actorloc,false,false);
	LaunchCharacter(FVector{ 0,0,wallclimbforce }, false, true);
	//
}

void Arunchar::stopcoiljump()
{
	coiljump = false;
	hitb->SetCapsuleHalfHeight(hitbheight);
}

void Arunchar::quickturn()
{
	if (iswallrunning)
	{
		FRotator targetrot = GetControlRotation();
		if (walldirectflip==1)
		{
			targetrot.Yaw = sliderot + 90;
			smoothrotdel.BindUObject(this, &Arunchar::smoothrot,targetrot,5.0f);
			GetWorld()->GetTimerManager().SetTimer(smoothrothandle, smoothrotdel, 0.005f, true);
		}
		else
		{
			targetrot.Yaw = sliderot - 90;
			smoothrotdel.BindUObject(this, &Arunchar::smoothrot, targetrot, 5.0f);
			GetWorld()->GetTimerManager().SetTimer(smoothrothandle, smoothrotdel, 0.005f, true);
		}
	}
	else if (canquickturn && !isquickturning)
	{
		canwallclimb = false;
		iswallclimbing = false;

		//Animation stuff
		pc->SetControlRotation(FRotator{0,yaw, 0 });
		PlayAnimMontage(Wallturn);
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(wallturndelegate, Wallturn);
		FollowCamera->bUsePawnControlRotation = 0;
		//

		isquickturning = true;
		holdtime = 0;
		GetWorldTimerManager().SetTimer(delayfall, this, &Arunchar::holdonwall, 0.005f, true);
	}
}

void Arunchar::holdonwall()
{
	if (holdtime >= maxholdtime)
	{
		yaw = pc->GetControlRotation().Yaw + 180;
		pc->SetControlRotation(FRotator{ -1 * pitch,yaw, 0});
		calculaterootyawoffset();
		StopAnimMontage(Wallturn);
		GetWorldTimerManager().ClearTimer(delayfall);
		isquickturning = false;
		return;
	}
	LaunchCharacter(FVector{ 0,0,-1 }, true, true);
	holdtime += 0.005f;
	
}

void Arunchar::endquickturn(UAnimMontage* mont, bool interupted)
{
	FollowCamera->bUsePawnControlRotation = 1;
	
}

void Arunchar::smoothrot(FRotator targetrot, float scale)
{
	targetrot.Pitch = GetControlRotation().Pitch;
	if (GetControlRotation().Equals(targetrot,3.0f))
	{
		GetWorldTimerManager().ClearTimer(smoothrothandle);
		return;
	}
	pc->SetControlRotation(FMath::RInterpTo(GetControlRotation(),targetrot,GetWorld()->GetDeltaSeconds(),scale));
	calculaterootyawoffset();
}

float Arunchar::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (isdying) return 0;
	currenthealth -= Damage;
	Healtheffect->SetScalarParameterValue(TEXT("Weight"), currenthealth / maxhealth);
	if (currenthealth <= 0)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), hitsound2);
		cm->StartCameraFade(0.0f, 1.0f, 2.0f, FLinearColor{0,0,0,1},true,true);
		isdying = true;
		PlayAnimMontage(deathmontage);
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(enddeathdelegate, deathmontage);
		FollowCamera->bUsePawnControlRotation = 0;
	}
	return currenthealth;
}

void Arunchar::resetplayer(UAnimMontage* mont, bool interupted)
{
	cm->StartCameraFade(1.0f, 0.0f, 1.0f, FLinearColor{ 0,0,0,1 }, true, true);
	isdying = false;
	FollowCamera->bUsePawnControlRotation = 1;
	currenthealth = maxhealth;
	SetActorLocation(respawnloc);
	Healtheffect->SetScalarParameterValue(TEXT("Weight"), 1);
}