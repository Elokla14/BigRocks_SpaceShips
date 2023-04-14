// Fill out your copyright notice in the Description page of Project Settings.


#include "BigRock.h"

// Sets default values
ABigRock::ABigRock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("RootComponent"));


	StaticMesh = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	CollisionComponent = CreateAbstractDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	AsteriodExplosionFX = CreateAbstractDefaultSubobject<UParticleSystemComponent>(TEXT("AsteriodExplosionFX"));
	AsteroidExplosionSound = CreateAbstractDefaultSubobject<UAudioComponent>(TEXT("AsteroidExplosionSound"));

	StaticMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CollisionComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	AsteriodExplosionFX->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	AsteroidExplosionSound->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Spawn_X_Velocity = -500.0f;
	Spawn_Y_Velocity = 0.0f;

	SelfDestructTimer = 1.0f;

	bHit = false;
}

// Called when the game starts or when spawned
void ABigRock::BeginPlay()
{
	Super::BeginPlay();

	Initial_Rotation = 0.0f;
	Initial_X_Location = 1500.0f;
	Initial_Y_Location = FMath::RandRange(-500, 500);

	RandSize = FMath::RandRange(3, 8);
	this->SetActorScale3D(FVector(RandSize, RandSize, RandSize));

	AsteroidExplosionSound->Deactivate();
	AsteroidExplosionSound->bStopWhenOwnerDestroyed = false;

	AsteriodExplosionFX->Deactivate();

	OnActorBeginOverlap.AddDynamic(this, &ABigRock::OnBeginOverlap);
}

// Called every frame
void ABigRock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	check(this != nullptr);

	Initial_Rotation += DeltaTime;

	Initial_X_Location += DeltaTime * Spawn_X_Velocity;
	Initial_Y_Location += DeltaTime * Spawn_Y_Velocity;

	this->SetActorRotation(FRotator(Initial_Rotation * 100.0f, Initial_Rotation * 50.0f, 0.0f));
	this->SetActorLocation(FVector(Initial_X_Location, Initial_Y_Location, 0.0f));

	if (SelfDestructTimer <= 0.0f)
		this->Destroy();

	if (bHit)
	{
		bHit = false;
		bStartDestroyTimer = true;

		AsteriodExplosionFX->Activate();
		AsteriodExplosionFX->SetWorldLocation(this->GetActorLocation());
		AsteriodExplosionFX->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

		AsteroidExplosionSound->Activate();
		AsteroidExplosionSound->Play();

		this->StaticMesh->SetVisibility(false);
		this->SetActorEnableCollision(false);

		if (this->GetActorScale3D().X > 6.0f)
		{
			SpawnChildren(3);
		}
	}

	if (bStartDestroyTimer)
	{
		SelfDestructTimer -= DeltaTime;
	}

} // tickkkk

void ABigRock::SpawnChildren(int32 NumChildren)
{
	FActorSpawnParameters Params = {};
	Params.Owner = this;

	for (int i = 0; i < NumChildren; i++)
	{
		if (ChildSpawn != nullptr)
		{
			ABigRock* NewBigRock = Cast<ABigRock>(GetWorld()->SpawnActor(ChildSpawn, new FVector(this->GetActorLocation()), new FRotator(this->GetActorRotation()), Params));

			NewBigRock->Initial_X_Location = this->GetActorLocation().X;
			NewBigRock->Initial_Y_Location = this->GetActorLocation().Y;

			NewBigRock->SetBigRockVelocity(FVector(FMath::RandRange(-250, 100), FMath::RandRange(-50, 50), 0.0f));

			float RandScale = FMath::RandRange(2, 5);

			NewBigRock->SetActorScale3D(FVector(RandScale, RandScale, RandScale));

			NewBigRock->SetActorEnableCollision(true);
			NewBigRock->StaticMesh->SetVisibility(true);
		}
	} // for loooooop

}

void ABigRock::SetBigRockVelocity(FVector NewVelocity)
{
	Spawn_X_Velocity = NewVelocity.X;
	Spawn_Y_Velocity = NewVelocity.Y;
}

void ABigRock::OnBeginOverlap(AActor* AsteroidActor, AActor* OtherActor)
{
	if (OtherActor->ActorHasTag("Bounds"))
	{
		SelfDestructTimer = 0.0f;
	}

	if (OtherActor->ActorHasTag("Projectile") || OtherActor->ActorHasTag("Player"))
	{
		bHit = true;
	}
		
}
