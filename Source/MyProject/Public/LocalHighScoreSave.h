// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LocalHighScoreSave.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ULocalHighScoreSave : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SavedVars)
	float besttime;
};
