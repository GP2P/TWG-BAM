// Fill out your copyright notice in the Description page of Project Settings.


#include "Water.h"
#include "Kismet/GameplayStatics.h"
#include "CollisionShape.h"
#include "DrawDebugHelpers.h"

// Sets default values
AWater::AWater()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> MyParticleSystem(TEXT("'/Game/Spells/P_ky_aquaStorm.P_ky_aquaStorm'")); 
	if (MyParticleSystem.Succeeded()) {
		ParticleExplosion = MyParticleSystem.Object;
	}

	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void AWater::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWater::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWater::Trigger(FVector location) {
	FCollisionShape MySphere = FCollisionShape::MakeSphere(200.0f); 
	//DrawDebugSphere(GetWorld(), GetActorLocation(), MySphere.GetSphereRadius(), 50, FColor::Purple, true);
	TArray<FHitResult> OutResults;
	TArray<AActor*> MarkedEnemies;
	GetWorld()->SweepMultiByChannel(OutResults, location, location, FQuat::Identity, ECC_WorldStatic, MySphere);
	for (int i = 0; i < OutResults.Num(); i++) {
		if (OutResults[i].Actor->ActorHasTag("Enemy")) {
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit Result: %s"), *OutResults[i].Actor->GetName()));
			MarkedEnemies.Add(OutResults[i].Actor.Get());
		}
	}
	if (ParticleExplosion) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleExplosion, location);
	}
	MarkToDestroy(MarkedEnemies);
}

void AWater::MarkToDestroy(TArray<AActor*> Destructibles) {
	for (int i = 0; i < Destructibles.Num(); i++) {
		Destructibles[i]->Tags.RemoveAt(0);
	}
}