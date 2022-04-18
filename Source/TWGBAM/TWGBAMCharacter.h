// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Projectile.h"
#include "Fire.h"
#include "HotBar.h"
#include "HotBarSpell.h"
#include "TWGBAMCharacter.generated.h"

UCLASS(config = Game)
class ATWGBAMCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;
public:
	ATWGBAMCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	float getHealth() const { return Health; }
	void setHealth(float val) { Health = val; }

	float getMaxHealth() const { return MaxHealth; }
	void setMaxHealth(float val) { MaxHealth = val; }

	UFUNCTION()
		void Fire();

	UFUNCTION()
		AActor* GetClosestActor(FVector sourceLocation, TArray<AActor*> actors);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector MuzzleOffset;

protected:
	UPROPERTY(VisibleAnywhere)
		class UWidgetComponent* HealthWidgetComp;
	UPROPERTY(VisibleAnywhere)
		class UWidgetComponent* HotWidgetComp;

protected:

	/** Resets HMD orientation in VR. */
	void OnResetVR();

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	//Projectile class to spawn
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AProjectile> ProjectileClass;

	/*
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AFire> FireClass;
		*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float MaxHealth;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float Health;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool DEAD;

	UHotBar* HotBar;
	UHotBarSpell* Spell;

protected:
	bool ThunderOn = false;
	bool FireOn = false;
	bool WaterOn = false;

	void SwitchThunder();
	void SwitchFire();
	void SwitchWater();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};