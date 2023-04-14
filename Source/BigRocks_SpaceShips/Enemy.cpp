// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	StaticMesh = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	CollisionComponent = CreateAbstractDefaultSubobject<UCapsuleComponent>(TEXT("Collision Component"));
	ExplosionFX = CreateAbstractDefaultSubobject<UParticleSystemComponent>(TEXT("Explosion FX"));
	ExplosionSound = CreateAbstractDefaultSubobject<UAudioComponent>(TEXT("Explosion Sound"));

	StaticMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CollisionComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ExplosionFX->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ExplosionSound->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	fBurstDelay = 0.15f;

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	TotalTime = 0.0f;
	TimeSinceLastShot = 0.0f;

	bHit = false;
	bDestroy = false;

	fDestroyTimer = 1.0f;

	ExplosionFX->Deactivate();
	ExplosionSound->Deactivate();

	ThisWorld = GetWorld();

	RandomStart = FMath::Rand();

	OnActorBeginOverlap.AddDynamic(this, &AEnemy::OnBeginOverlap);

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TotalTime += DeltaTime;
	TimeSinceLastShot += DeltaTime;

	CurrentLocation = this->GetActorLocation();
	CurrentRotation = this->GetActorRotation();

	CurrentLocation.Y += FMath::Sin(TotalTime + RandomStart);

	this->SetActorLocation(CurrentLocation - (CurrentVelocity * DeltaTime));

	if (TimeSinceLastShot >= 1.0f && !bHit)
	{
		if (fBurstDelay >= 0.15f)
		{
			FActorSpawnParameters Params = {};
			Params.Owner = this;

			GetWorld()->SpawnActor(EnemyProjectile, &CurrentLocation, &CurrentRotation, Params);

			fBurstDelay = 0.0f;
		}
		TimeSinceLastShot = 0.0f;
		fBurstDelay += DeltaTime;
	}// Weapon shooting logic

	if (bDestroy)
	{
		this->Destroy();
	}

	if (bHit)
	{
		StaticMesh->SetVisibility(false);
		this->SetActorEnableCollision(false);

		ExplosionFX->Activate();
		ExplosionSound->Activate();

		fDestroyTimer -= DeltaTime;
	}
	
	if (fDestroyTimer <= 0.0f)
	{
		this->Destroy();
	}
	

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::OnBeginOverlap(AActor* EnemyActor, AActor* OtherActor)
{
	if (OtherActor->ActorHasTag("Bounds"))
	{
		bDestroy = true;
	}

	if (OtherActor->ActorHasTag("Asteroid") || OtherActor->ActorHasTag("Player"))
	{
		bHit = true;
	}

	if (OtherActor->ActorHasTag("Projectile"))
	{
		bHit = true;

		if (ThisWorld)
		{
			if (FMath::RandRange(0, 10) > 7)
			{
				FVector Current_Location = this->GetActorLocation();
				FRotator Current_Rotation = this->GetActorRotation();
				FActorSpawnParameters Params = {};

				ThisWorld->SpawnActor(PickupCan, &Current_Location, &Current_Rotation, Params);

			}

			
		}
	}


} // on begin overlapp