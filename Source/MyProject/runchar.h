// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BoxComponent.h"
#include "runchar.generated.h"



UCLASS()
class MYPROJECT_API Arunchar : public ACharacter
{
	GENERATED_BODY()
	DECLARE_DELEGATE_FourParams(Endziplinesignature, UPrimitiveComponent*, AActor*, UPrimitiveComponent*, int32);

	/** Follow camera */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Omnitool, meta = (AllowPrivateAccess = "true"))
	class UChildActorComponent* omnitool;
	UPROPERTY(EditDefaultsOnly, Category = Omnitool)
	TSubclassOf<class AOmnitool> omnitoolclass;
	class UCapsuleComponent* hitb;
		float hitbheight;

public:
	// Sets default values for this character's properties
	Arunchar();

//Keybinded functions
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	void Lookup(float Axisval);

	void Turn(float Axisval);

	void UnSpace();

	void Crouch();

	void StopCrouch();

	APlayerController* pc;

	void Interact();

	void UnInteract();

	public: 
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actions) bool interactbuttondown;
		void calculaterootyawoffset();

public:
	void Jump() override;

	void Allowalli();
	void Disablealli();
	bool takeyaw;
	bool takepitch;
	bool takews;
	bool takead;
	bool takecrouch;
	bool takejump;

//Sliding stuff
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	bool issliding;
	void Startslide();
	void Updateslide();
	void Endslide();
	FVector Setslidevector();
	FVector slidevector;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
	float sliderot;
	float origbrakedecel;
	float origfrict;
	float origcrouchspeed;
private:
	FTimerHandle slideupdatehandle;
	FVector slidefv;
	FVector sliderv;
//--------------

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	/** Returns FollowCamera subobject **/
	//FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cameracontrol)
	float sensitivity;

	void Landed(const FHitResult& Hit) override;
	void Falling() override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cameracontrol)
	float pitch;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cameracontrol)
	float yaw;
protected:
	float yawlastframe;
	float yawchangeoverframe;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cameracontrol)
	float turnchange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cameracontrol)
	float rootyawoffset;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cameracontrol)
		bool isturning;
	float curveval;
	float curvevall;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cameracontrol)
	float turnratio;

protected:
	float startheight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Misc)
		float hardfalltime;
	
	//Falling animation handling
	FOnMontageEnded landenddelegate;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		UAnimMontage* Hardlanding;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		UAnimMontage* Rolllanding;
	UFUNCTION()
	void OnFallAnimationEnded(UAnimMontage* mont, bool interupted);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		bool willroll;

	//-----------------------------------------------------------

protected:
	UFUNCTION(BlueprintCallable, Category = Getmovementstate)
	void setisMoving();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movementstate)
	bool ismoving;

	FTimerHandle getmovingtimerhandle;

	//For raycasting
	TArray<AActor*> actorsToIgnore;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
		UCurveFloat* accelcurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
		float flowtime;
	UFUNCTION(BlueprintCallable, Category = Movement)
		void updateaccel();
	FTimerHandle updateaccelhandle;

//wall run stuff
	protected:
		void delaywallrun1();
		void delaywallrun2();

		FTimerHandle wallrunstart;
		FTimerHandle wallruntime;
		FTimerHandle upwallrun;
		
		void updatewallrun();
		float oggravscale;

	public:
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
		float walldirectflip;
		bool canwallrun;
		FVector prevcross;
		FVector cross;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Movement)
		bool iswallrunning;

//Vault stuff
		bool canvault;
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations)
		bool vaultanim;
		bool vaulting;
		float vaulttime;
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animations)
		FVector vaultloc;
		AActor* remcollisonactor;
		void vaultupdate();
		FTimerHandle updatevaulthandle;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
			UAnimMontage* vault1;
		UFUNCTION(BlueprintCallable, Category = Movement)
		void endvault(UAnimMontage* mont, bool interupted);
		FOnMontageEnded vaultenddelegate;

//Ik stuff
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
			bool lefthandwallblock;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
			bool righthandwallblock;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
			FVector lefthandwallblockloc;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
			FVector righthandwallblockloc;
		FTimerHandle updatewallblockhandle;
		void updatewallblock();
//Omnitool stuff
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Omnitool)
			bool inrangeofinteractible;
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
			UAnimMontage* Interactanim;
//Wallclimb stuff
		bool canwallclimb;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		bool iswallclimbing;
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		float maxclimbtime;
		float wallclimbtime;
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		float wallclimbforce;
		void updatewallclimb();
		FTimerHandle updatewallclimbhandle;
//Quickturn stuff
		void quickturn();
		FTimerHandle delayfall; //setgravityscale not working????????
		float holdtime;
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
		float maxholdtime;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		bool isquickturning;
		void holdonwall();
		bool canquickturn;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		UAnimMontage* Wallturn;
		FOnMontageEnded wallturndelegate;

		void endquickturn(UAnimMontage* mont, bool interupted);

//Zipline stuff
		void setgrav(bool resetgrav = false, float newgrav = 0.0f);
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		bool isziplining;
		Endziplinesignature endoverlapdelegate;
//Coil jump stuff
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
		bool coiljump;
		void stopcoiljump();
};
