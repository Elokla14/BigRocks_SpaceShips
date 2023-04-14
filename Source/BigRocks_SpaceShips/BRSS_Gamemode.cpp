// Fill out your copyright notice in the Description page of Project Settings.


#include "BRSS_Gamemode.h"

ABRSS_Gamemode::ABRSS_Gamemode()
{
	SpawnTimer = 0.0f;

	MusicTrack = CreateDefaultSubobject<UAudioComponent>(TEXT("Music"));
	MusicTrack->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	AsteroidSpawnLoc = FVector(1000.0f, 1000.0f, 1000.0f);
	AsteroidSpawnRot = FRotator(0.0f, 0.0f, 0.0f);

	ThisWorld = GetWorld();
}

void ABRSS_Gamemode::BeginPlay()
{
	Super::BeginPlay();

	MusicTrack->Play();

	bPlayerDead = false;

	if (Score_Widget_Class != nullptr)
	{
		Score_Widget = CreateWidget<UUserWidget>(GetWorld(), Score_Widget_Class);
		Score_Widget->AddToViewport();
	}

	if (Health_And_Armor_Widget_Class != nullptr)
	{
		Health_And_Armor_Widget = CreateWidget<UUserWidget>(GetWorld(), Health_And_Armor_Widget_Class);
		Health_And_Armor_Widget->AddToViewport();
	}

	if (Restart_Widget_Class != nullptr)
	{
		Restart_Widget = CreateWidget<UUserWidget>(GetWorld(), Restart_Widget_Class);
		Restart_Widget->AddToViewport();

		Restart_Widget->SetVisibility(ESlateVisibility::Hidden);
	}

	PC_Ref = GetWorld()->GetFirstPlayerController();
}

void ABRSS_Gamemode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpawnTimer = FMath::RandRange(0, 1000);

	if (Enemy)
	{
		if (SpawnTimer > 990)
		{
			FVector EnemySpawnLoc(0.0f, 0.0f, 0.0f);
			FRotator EnemySpawnRot(0.0f, 0.0f, 0.0f);

			AActor* NewEnemy = GetWorld()->SpawnActor(Enemy, &EnemySpawnLoc, &EnemySpawnRot, AsteroidSpawnParameters);

			if (NewEnemy)
				NewEnemy->SetActorLocation(FVector(1000.0f, FMath::RandRange(-500, 500), 0.0f));
		}
	}// Enemy spawn logic

	if (BigRockTemplate)
	{
		if (SpawnTimer > 995)
		{
			AActor* NewBigRock = GetWorld()->SpawnActor(BigRockTemplate, &AsteroidSpawnLoc, &AsteroidSpawnRot, AsteroidSpawnParameters);

			if (NewBigRock)
			{
				NewBigRock->SetActorLocation(FVector(1000.0f, 1000.0f, 1000.0f));
			}
		}
	}

	if (bPlayerDead)
	{
		Restart_Widget->SetVisibility(ESlateVisibility::Visible);

		PC_Ref->bShowMouseCursor = true;

		bPlayerDead = false;
	}

} // tick

void ABRSS_Gamemode::RestartLevel(FName LevelName)
{

}
