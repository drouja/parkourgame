// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Checkpoint.h"
#include "GameFramework/Actor.h"
#include "Checkpointmanager.generated.h"

UCLASS()
class MYPROJECT_API ACheckpointmanager : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ACheckpointmanager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Checkpoints)
	TArray<ACheckpoint*> checkpointarray;
	void Checkpointreached(ACheckpoint::Checkpointtype checktype);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TimeTrialProperties)
	FString timepassedstring;
	void timetrial();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TimeTrialProperties)
	float timepassed;
	FTimerHandle timerhandle;
};
