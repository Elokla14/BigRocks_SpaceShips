// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "BRSS_Player.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	BulletComponent = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("BulletComponent"));
	CapsuleCollision = CreateAbstractDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));

	BulletComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CapsuleCollision->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	bHit = false;
	bDestroy = false;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentLocation = this->GetActorLocation();

	OnActorBeginOverlap.AddDynamic(this, &AProjectile::OnBeginOverlap);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewLocation = FVector(CurrentLocation.X + (ProjectileSpeed * DeltaTime), CurrentLocation.Y, CurrentLocation.Z);

	this->SetActorLocation(NewLocation);
	CurrentLocation = NewLocation;

	if (bHit)
	{
		this->Destroy();
	}
}

void AProjectile::OnBeginOverlap(AActor* ProjectileActor, AActor* OtherActor)
{
	if (OtherActor)
	{
		ABRSS_Player* OwningPlayer = Cast<ABRSS_Player>(this->GetOwner());

		if (OtherActor->ActorHasTag(FName("Bounds")))
		{
			bHit = true;
		}

		if (this->ActorHasTag(FName("EnemyProjectile"))  && OtherActor->ActorHasTag(FName("Player")))
		{
			bHit = true;
		}

		if (OtherActor->ActorHasTag(FName("EnemyShip")))
		{
			if (OwningPlayer)
			{
				OwningPlayer->PlayerScore += 50.0f;

				bHit = true;
			}
		}

		if (OtherActor->ActorHasTag(FName("Asteroid")))
		{
			if (OwningPlayer)
			{
				OwningPlayer->PlayerScore += 10.0f;

				bHit = true;
			}
		}

	}

}

