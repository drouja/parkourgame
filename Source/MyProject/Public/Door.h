// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class MYPROJECT_API ADoor : public AActor
{
	GENERATED_BODY()
	

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Misc)
	bool Actorinrange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Misc)
	bool openingdoor;
protected:
	class Arunchar* asrunchar;
	bool open;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Misc)
	float desiredrot;
	float rotationprogress;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Misc)
		float openspeed;

public:	
	// Sets default values for this actor's properties
	ADoor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Misc)
	class USceneComponent* Arootcomponent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Misc)
	class USceneComponent* doorpivot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMeshComponent* doormesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMeshComponent* lockmesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
	class UBoxComponent* Hitbox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	class UWidgetComponent* HUD;
	UMaterialInstanceDynamic* ledmat;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FTimerHandle getcharacterinrangehandle;
	FTimerHandle smoothopendoorhandle;

	void Getcharacterinrange();
	void Smoothopendoor();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
