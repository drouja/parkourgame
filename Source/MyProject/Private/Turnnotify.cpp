// Fill out your copyright notice in the Description page of Project Settings.
#include "Turnnotify.h"
#include "../runchar.h"

void UTurnnotify::NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration)
{
	Arunchar* player = Cast<Arunchar> (MeshComp->GetOwner());
	if (player != nullptr)
	{
		player->isturning = true;
		player->curveval = 0;
	}
}

void UTurnnotify::NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, "Bruh");
	Arunchar* player = Cast<Arunchar>(MeshComp->GetOwner());
	if (player != nullptr)
	{
		player->isturning = false;
	}
}