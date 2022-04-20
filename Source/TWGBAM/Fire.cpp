// Fill out your copyright notice in the Description page of Project Settings.


#include "Fire.h"
#include "Kismet/GameplayStatics.h"
#include "CollisionShape.h"
#include "DrawDebugHelpers.h"

// Sets default values
AFire::AFire()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!RootComponent) {
		RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSceneComponent"));
	}

	if (!CollisionComponent) {
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		CollisionComponent->OnComponentHit.AddDynamic(this, &AFire::OnHit);
		CollisionComponent->InitSphereRadius(30.0f);
		RootComponent = CollisionComponent;
	}

	if (!ProjectileMovementComponent) {
		// Use this component to drive this projectile's movement.
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 750.0f;
		ProjectileMovementComponent->MaxSpeed = 750.0f;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = true;
		ProjectileMovementComponent->Bounciness = 0.3f;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}
	if (!ProjectileMeshComponent) {
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		static ConstructorHelpers::FObjectFinder<UStaticMesh>Mesh(TEXT("'/Game/Spells/Sphere.Sphere'"));
		if (Mesh.Succeeded()) {
			ProjectileMeshComponent->SetStaticMesh(Mesh.Object);
		}
	}
	//Material'/Game/Spells/FireMaterial.FireMaterial'
	static ConstructorHelpers::FObjectFinder<UMaterial>Material(TEXT("'/Game/Spells/M_ky_storm02.M_ky_storm02'"));
	if (Material.Succeeded()) {
		ProjectileMaterialInstance = UMaterialInstanceDynamic::Create(Material.Object, ProjectileMeshComponent);
	}
	ProjectileMeshComponent->SetMaterial(0, ProjectileMaterialInstance);
	ProjectileMeshComponent->SetRelativeScale3D(FVector(0.09f, 0.09f, 0.09f));
	ProjectileMeshComponent->SetupAttachment(RootComponent);

	InitialLifeSpan = 3.0f;
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> MyParticleSystem(TEXT("'/Game/Spells/P_ky_explosion.P_ky_explosion'"));
	if (MyParticleSystem.Succeeded()) {
		ParticleExplosion = MyParticleSystem.Object;
	}
}


// Called when the game starts or when spawned
void AFire::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AFire::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFire::FireInDirection(const FVector& ShootDirection) {
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
}

void AFire::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) {
	if (OtherActor != this && OtherComponent->IsSimulatingPhysics())
	{
		OtherComponent->AddImpulseAtLocation(ProjectileMovementComponent->Velocity * 100.0f, Hit.ImpactPoint);
	}
	else if (OtherActor->ActorHasTag("Enemy")) {
		FCollisionShape MySphere = FCollisionShape::MakeSphere(400.0f); // 5M Radius
		//DrawDebugSphere(GetWorld(), GetActorLocation(), MySphere.GetSphereRadius(), 50, FColor::Purple, true);
		TArray<FHitResult> OutResults;
		TArray<AActor*> MarkedEnemies;
		GetWorld()->SweepMultiByChannel(OutResults, OtherActor->GetActorLocation(), OtherActor->GetActorLocation(), FQuat::Identity, ECC_WorldStatic, MySphere);
		for (int i = 0; i < OutResults.Num(); i++) {
			if (OutResults[i].Actor->ActorHasTag("Enemy")) {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit Result: %s"), *OutResults[i].Actor->GetName()));
				//OutResults[i].Actor->Destroy();
				MarkedEnemies.Add(OutResults[i].Actor.Get());
			}
		}
		MarkToDestroy(MarkedEnemies);
	}
	FVector SpellLocation = this->GetActorLocation();
	if (ParticleExplosion) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleExplosion, SpellLocation);
	}
	Destroy();

}

void AFire::MarkToDestroy(TArray<AActor*> Destructibles) {
	for (int i = 0; i < Destructibles.Num(); i++) {
		Destructibles[i]->Destroy();
	}
}