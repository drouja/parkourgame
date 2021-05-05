// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpointmanager.h"
#include "../UEHelperfuncts.h"

// Sets default values
ACheckpointmanager::ACheckpointmanager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	timepassed = 0;
	timepassedstring = "00:00:000";
}

// Called when the game starts or when spawned
void ACheckpointmanager::BeginPlay()
{
	if (checkpointarray.Num() < 2)
	{
		if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("You must place at least 2 checkpoints for the checkpoint system to work"));
		return;
	}
	Super::BeginPlay();
	checkpointarray[0]->checktype = ACheckpoint::Start;
	checkpointarray[0]->checkreachdelegate.BindUObject(this, &ACheckpointmanager::Checkpointreached);
	checkpointarray.Last()->checktype = ACheckpoint::End;
	checkpointarray.Last()->checkreachdelegate.BindUObject(this, &ACheckpointmanager::Checkpointreached);
	for (int i = 1; i < checkpointarray.Num() - 1; i++)
	{
		checkpointarray[i]->checktype = ACheckpoint::Middle;
	}
	
}

// Called every frame
void ACheckpointmanager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACheckpointmanager::Checkpointreached(ACheckpoint::Checkpointtype checktype)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Checkpoint reached"));
	if (checktype == ACheckpoint::Start) GetWorldTimerManager().SetTimer(timerhandle, this, &ACheckpointmanager::timetrial, 0.001f, true);
	else if (checktype == ACheckpoint::End) GetWorldTimerManager().ClearTimer(timerhandle);

}

void ACheckpointmanager::timetrial()
{
	timepassed += 0.001f;
	int timepassedint = (int)timepassed;
	int milliseconds = (timepassed - timepassedint) * 1000;
	int minutes = timepassedint / 60;
	int seconds = timepassedint % 60;
	timepassedstring = AddLeadingZeroes(minutes,2) + ":" + AddLeadingZeroes(seconds, 2) + ":" + AddLeadingZeroes(milliseconds, 3);
	return;
}