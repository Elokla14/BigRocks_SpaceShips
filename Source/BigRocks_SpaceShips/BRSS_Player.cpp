// Fill out your copyright notice in the Description page of Project Settings.


#include "BRSS_Player.h"
#include "BRSS_Gamemode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABRSS_Player::ABRSS_Player()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	RootComponent = CreateAbstractDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	ShipMesh = CreateAbstractDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	ParticleSystems = CreateAbstractDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystems"));
	CollisionComponent = CreateAbstractDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	ExplosionFX = CreateAbstractDefaultSubobject<UParticleSystemComponent>(TEXT("ExplosionFX"));
	DeathExplosionSound = CreateAbstractDefaultSubobject<UAudioComponent>(TEXT("DeathExplosionSound"));

	ShipMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ParticleSystems->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CollisionComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	ExplosionFX->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	DeathExplosionSound->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	MaxVelocity = 250.0f;
	Current_X_Velocity = 0.0f;
	Current_Y_Velocity = 0.0f;

	bIsFiring = false;
	WeaponFireRate = 0.25f;
	TimeSinceLastShot = 0.0f;

	PlayerScore = 0.0f;
}

// Called when the game starts or when spawned
void ABRSS_Player::BeginPlay()
{
	Super::BeginPlay();

	Current_Location = this->GetActorLocation();
	Current_Rotation = this->GetActorRotation();

	bHit = false;
	bDead = false;

	ExplosionFX->Deactivate();
	DeathExplosionSound->Deactivate();

	Max_Health = 100.0f;
	Current_Health = 100.0f;

	Max_Armor = 100.0f;
	Current_Armor = 100.0f;
	
	OnActorBeginOverlap.AddDynamic(this, &ABRSS_Player::OnBeginOverLap);
	
}

// Called every frame
void ABRSS_Player::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Current_X_Velocity != 0.0f || Current_Y_Velocity != 0.0f)
	{
		New_Location = FVector(Current_Location.X + (Current_X_Velocity * DeltaTime), Current_Location.Y + (Current_Y_Velocity * DeltaTime), 0);

		this->SetActorLocation(New_Location);

		Current_Location = New_Location;
	}

	if (Current_Y_Velocity > 0.1f)
	{
		this->SetActorRotation(FRotator(Current_Rotation.Pitch + 45.0f, Current_Rotation.Yaw, Current_Rotation.Roll));
	}
	else if (Current_Y_Velocity < -0.1f)
	{
		this->SetActorRotation(FRotator(Current_Rotation.Pitch - 45.0f, Current_Rotation.Yaw, Current_Rotation.Roll));
	}
	else
	{
		this->SetActorRotation(FRotator(Current_Rotation));
	}

	if (this->GetActorLocation().X > Field_Width)
	{
		Current_Location = FVector(Field_Width - 1, Current_Location.Y, Current_Location.Z);
	}

	if (this->GetActorLocation().X < -Field_Width)
	{
		Current_Location = FVector(-Field_Width + 1, Current_Location.Y, Current_Location.Z);
	}

	if (this->GetActorLocation().Y > Field_Height)
	{
		Current_Location = FVector(Current_Location.X, Field_Height - 1, Current_Location.Z);
	}

	if (this->GetActorLocation().Y < -Field_Height)
	{
		Current_Location = FVector(Current_Location.X, -Field_Height + 1, Current_Location.Z);
	}


	if (bIsFiring)
	{
		if (TimeSinceLastShot > WeaponFireRate)
		{
			FireWeapon();
			TimeSinceLastShot = 0.0f;
		}
	}//Weapon projectile firing logic

	TimeSinceLastShot += DeltaTime;

	if (bHit)
	{
		bDead = true;

		this->ShipMesh->SetVisibility(false);
		this->ParticleSystems->SetVisibility(false);
		this->DeathExplosionSound->Activate();
		this->DeathExplosionSound->Play();

		this->SetActorTickEnabled(false);

		ABRSS_Gamemode* GameModeREF = Cast<ABRSS_Gamemode>(UGameplayStatics::GetGameMode(GetWorld()));
		GameModeREF->bPlayerDead = true;

	}

} // tick


// Called to bind functionality to input
void ABRSS_Player::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ABRSS_Player::MoveRight);
	PlayerInputComponent->BindAxis(FName("MoveUp"), this, &ABRSS_Player::MoveUp);

	PlayerInputComponent->BindAction(FName("Fire"), IE_Pressed, this, &ABRSS_Player::StartFiring);
	PlayerInputComponent->BindAction(FName("Fire"), IE_Released, this, &ABRSS_Player::StopFiring);
}

void ABRSS_Player::MoveRight(float AxisValue)
{
	Current_X_Velocity = MaxVelocity * AxisValue;
}

void ABRSS_Player::MoveUp(float AxisValue)
{
	Current_Y_Velocity = MaxVelocity * AxisValue;
}

void ABRSS_Player::StartFiring()
{
	bIsFiring = true;
}

void ABRSS_Player::StopFiring()
{
	bIsFiring = false;
}

void ABRSS_Player::FireWeapon()
{
	FActorSpawnParameters Params = {};
	Params.Owner = this;

	AActor* SpawnedProjectile = GetWorld()->SpawnActor(WeaponProjectile_BP, &Current_Location, &Current_Rotation, Params);
}

void ABRSS_Player::CollectablePickup()
{
	if (Current_Armor < 100.0f && Current_Health == 100.0f)
	{
		Current_Armor += 10.0f;

		if (Current_Armor > 100.0f)
			Current_Armor = 100.0f;
	}
	else if (Current_Health < 100.0f)
	{
		Current_Health += 10.0f;

		if (Current_Health > 100.0f)
			Current_Health = 100.0f;
	}
}

void ABRSS_Player::OnBeginOverLap(AActor* PlayerActor, AActor* OtherActor)
{
	if (OtherActor->ActorHasTag("Asteroid") || OtherActor->ActorHasTag("EnemyShip"))
	{
		if (Current_Armor > 0.0f) {

			Current_Armor -= 10.0f;

			if (Current_Armor < 0.0f)
				Current_Armor = 0.0f;
		} 
		else if (Current_Health > 0.0f) {

			Current_Health -= 10.0f;

			if (Current_Health <= 0.0f)
			{
				Current_Health = 0.0f;

				bHit = true;
			}
		}
	}

	if (OtherActor->ActorHasTag("EnemyProjectile"))
	{
		if (Current_Armor > 0.0f)
		{
			Current_Armor -= 25.0f;

			if (Current_Armor < 0.0f)
				Current_Armor = 0.0f;
		}
		else if (Current_Health > 0.0f)
		{
			Current_Health -= 25.0f;

			if (Current_Health <= 0.0f)
			{
				Current_Health = 0.0f;

				bHit = true;
			}
		}
	}
}
