// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Enemy.h"
#include "BigRock.h"
#include "Blueprint/UserWidget.h"
#include "BRSS_Gamemode.generated.h"

/**
 * 
 */
UCLASS()
class BIGROCKS_SPACESHIPS_API ABRSS_Gamemode : public AGameMode
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
			TSubclassOf<AEnemy> Enemy;

		UPROPERTY(EditAnywhere)
		TSubclassOf<ABigRock> BigRockTemplate;

		UPROPERTY(EditAnywhere)
			UAudioComponent* MusicTrack;

		//constructor
		ABRSS_Gamemode();

		virtual void BeginPlay() override;
		virtual void Tick(float DeltaTime) override;

		TSubclassOf<ABigRock> GetBigRockTemplate();

public:

	int32 SpawnTimer;
	bool bPlayerDead;

	UFUNCTION()
		void RestartLevel(FName LevelName);

	bool bStart;

protected:

	FActorSpawnParameters AsteroidSpawnParameters;
	FVector AsteroidSpawnLoc;
	FRotator AsteroidSpawnRot;

	UWorld* ThisWorld;

	UPROPERTY(EditAnywhere, Category = "HUD Elements")
		TSubclassOf<UUserWidget> Score_Widget_Class;
	UUserWidget* Score_Widget;

	UPROPERTY(EditAnywhere, Category = "HUD Elements")
		TSubclassOf<UUserWidget> Health_And_Armor_Widget_Class;
	UUserWidget* Health_And_Armor_Widget;

	UPROPERTY(EditAnywhere, Category = "HUD Elements")
		TSubclassOf<UUserWidget> Restart_Widget_Class;
	UUserWidget* Restart_Widget;

	APlayerController* PC_Ref;
};
