// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Checkpoint.h"
#include "GameFramework/Actor.h"
#include "LocalHighScoreSave.h"
#include "Kismet/GameplayStatics.h"
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
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void endtimer();

	virtual void endtimer_Implementation();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TimeTrialProperties)
	FString endscreenflavourtext;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TimeTrialProperties)
	float currentbest;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TimeTrialProperties)
	FString differenceintime;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TimeTrialProperties)
	float timepassed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TimeTrialProperties, meta = (ClampMin = "1.0", UIMin = "1.0"))
	float threestartime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TimeTrialProperties, meta = (ClampMin = "1.0", UIMin = "1.0"))
	float twostartime;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = TimeTrialProperties, meta = (ClampMin = "1.0", UIMin = "1.0"))
	float onestartime;
	FTimerHandle timerhandle;
//Hud stars variables:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TimeTrialProperties)
		float threestarpercent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TimeTrialProperties)
		float twostarpercent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = TimeTrialProperties)
		float onestarpercent;
};
