// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.generated.h"

UCLASS()
class MYPROJECT_API ACheckpoint : public AActor
{
	GENERATED_BODY()

public:
	enum Checkpointtype
	{
		Start=0,
		Middle,
		End
	};
protected:
	DECLARE_DELEGATE_OneParam(Checkreachedsignature, Checkpointtype);

	class USceneComponent* Aroot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Components, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* hitbox;
public:	
	// Sets default values for this actor's properties
	ACheckpoint();
	Checkreachedsignature checkreachdelegate;
	Checkpointtype checktype;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
