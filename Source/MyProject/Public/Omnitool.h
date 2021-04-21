// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Omnitool.generated.h"

UCLASS()
class MYPROJECT_API AOmnitool : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOmnitool();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Misc)
		class USceneComponent* Arootcomponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Mesh)
		class UStaticMeshComponent* omnitoolmesh;
	UMaterialInstanceDynamic* omnitoolmat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Misc)
		class UPointLightComponent* statuslight;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	enum struct Weaponmodes {Grapple, Shield};

	class Arunchar* asrunchar;

	FTimerHandle materializeomnitoolhandle;

	void materializeomnitool();

	float erode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	float eroderate;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
