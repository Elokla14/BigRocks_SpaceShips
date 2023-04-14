// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUp.h"
#include "BRSS_Player.h"

// Sets default values
APickUp::APickUp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	StaticMesh = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	CollisionComponent = CreateAbstractDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));

	StaticMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CollisionComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

}

// Called when the game starts or when spawned
void APickUp::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &APickUp::OnBeginOverlap);
	
}

// Called every frame
void APickUp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->SetActorLocation(FVector(this->GetActorLocation().X - (DeltaTime * 100.0f), this->GetActorLocation().Y, 0.0f));

}

void APickUp::OnBeginOverlap(AActor* PickupActor, AActor* OtherActor)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		ABRSS_Player* PlayerActor = Cast<ABRSS_Player>(OtherActor);
		PlayerActor->CollectablePickup();

		this->Destroy();
	}
	
	if (OtherActor->ActorHasTag("Bounds"))
	{
		this->Destroy();
	}
}