// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpointmanager.h"
#include "../UEHelperfuncts.h"
#include "../runchar.h"


FString formatsecs(float timepassed)
{
	int timepassedint = (int)timepassed;
	int milliseconds = (timepassed - timepassedint) * 1000;
	int minutes = timepassedint / 60;
	int seconds = timepassedint % 60;
	return (AddLeadingZeroes(minutes, 2) + ":" + AddLeadingZeroes(seconds, 2) + ":" + AddLeadingZeroes(milliseconds, 3));
}

// Sets default values
ACheckpointmanager::ACheckpointmanager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	timepassed = 0;
	timepassedstring = "00:00:000";
	threestarpercent = 1;
	twostarpercent = 1;
	onestarpercent = 1;
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
		checkpointarray[i]->checkreachdelegate.BindUObject(this, &ACheckpointmanager::Checkpointreached);
	}

}

// Called every frame
void ACheckpointmanager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACheckpointmanager::Checkpointreached(ACheckpoint::Checkpointtype checktype)
{
	if (checktype == ACheckpoint::Start) GetWorldTimerManager().SetTimer(timerhandle, this, &ACheckpointmanager::timetrial, 0.001f, true);
	else if (checktype == ACheckpoint::End)
	{
		GetWorldTimerManager().ClearTimer(timerhandle);
		ACheckpointmanager::endtimer();
	}
	if (Arunchar* playerpawn = Cast<Arunchar>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn()))
	{
		playerpawn->respawnloc = playerpawn->GetActorLocation();
	}
}

void ACheckpointmanager::timetrial()
{
	timepassed += 0.001f;
	timepassedstring = formatsecs(timepassed);
	threestarpercent = (threestartime - timepassed) / threestartime;
	twostarpercent = (twostartime - timepassed) / (twostartime - threestartime);
	onestarpercent = (onestartime - timepassed) / (onestartime - twostartime);
	return;
}

void ACheckpointmanager::endtimer_Implementation()
{
	if (threestarpercent >= 0) threestarpercent = 1;
	else if (twostarpercent >= 0) twostarpercent = 1;
	else if (onestarpercent >= 0) onestarpercent = 1;

	if (ULocalHighScoreSave* savedscore = Cast<ULocalHighScoreSave>(UGameplayStatics::LoadGameFromSlot(GetWorld()->GetMapName(), 0)))
	{
		currentbest=savedscore->besttime;
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));
		if (currentbest > timepassed)
		{
			endscreenflavourtext = "New Best!";
			differenceintime = "-" + formatsecs(-1 * (timepassed - currentbest));
			if (ULocalHighScoreSave* savescore = Cast<ULocalHighScoreSave>(UGameplayStatics::CreateSaveGameObject(ULocalHighScoreSave::StaticClass())))
			{
				savescore->besttime = timepassed;
				UGameplayStatics::SaveGameToSlot(savescore, GetWorld()->GetMapName(), 0);
			}
		}
		else
		{
			endscreenflavourtext = "Good Job!";
			differenceintime = "+" + formatsecs((timepassed - currentbest));
		}
	}
	else
	{
		endscreenflavourtext = "New Best!";
		differenceintime = "";
		if (ULocalHighScoreSave* savescore = Cast<ULocalHighScoreSave>(UGameplayStatics::CreateSaveGameObject(ULocalHighScoreSave::StaticClass())))
		{
			savescore->besttime = timepassed;
			UGameplayStatics::SaveGameToSlot(savescore, GetWorld()->GetMapName(), 0);
		}
	}
	UGameplayStatics::SetGamePaused(GetWorld(),true);
}

