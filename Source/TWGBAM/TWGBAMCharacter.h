// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Projectile.h"
#include "Fire.h"
#include "Water.h"
#include "HotBar.h"
#include "HotBarSpell.h"
#include "Animation/AnimMontage.h"
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

	UFUNCTION(BlueprintCallable)
		void Fire();

	UFUNCTION(BlueprintCallable)
		void SpellOnCooldown();

	UFUNCTION()
		AActor* GetClosestActor(FVector sourceLocation, TArray<AActor*> actors);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		bool Attacking;

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

	UPROPERTY(BlueprintReadWrite, EditAnywhere) float FirePickup;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float ThunderPickup;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float WaterPickup;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) float BeginningSpells;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) float MaxSpellUsage;
	float FireUsage;
	float ThunderUsage;
	float WaterUsage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere) float Cooldown;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool FireOnCooldown;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool ThunderOnCooldown;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool WaterOnCooldown;

	float MaxFirePickup = 0.0f;
	float MaxThunderPickup = 0.0f;
	float MaxWaterPickup = 0.0f;

	UHotBar* HotBar;
	UHotBarSpell* Spell;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool ThunderOn = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool FireOn = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) bool WaterOn = false;

	void SwitchThunder();
	void SwitchFire();
	void SwitchWater();

	void ScrollDown();
	void ScrollUp();

	int scrollLoc;

	void RunLock();
	void RunOff();

	bool runOn;

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