// Copyright Epic Games, Inc. All Rights Reserved.

#include "TWGBAMCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "HealthBar.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

//////////////////////////////////////////////////////////////////////////
// ATWGBAMCharacter

ATWGBAMCharacter::ATWGBAMCharacter(const FObjectInitializer& ObjectInitializer)
{
	setMaxHealth(100);
	setHealth(getMaxHealth());

	MaxSpellUsage = 30.0f;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	if (RootComponent == nullptr) {
		RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	}

	HealthWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("HealthBar"));
	HealthWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	HotWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("HotBar"));
	HotWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	scrollLoc = 0;
	runOn = false;
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATWGBAMCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATWGBAMCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATWGBAMCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	//PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("TurnRate", this, &ATWGBAMCharacter::TurnAtRate);
	//PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	//PlayerInputComponent->BindAxis("LookUpRate", this, &ATWGBAMCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATWGBAMCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATWGBAMCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ATWGBAMCharacter::OnResetVR);

	//Spell key bindings
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATWGBAMCharacter::Fire);
	PlayerInputComponent->BindAction("ThunderOn", IE_Pressed, this, &ATWGBAMCharacter::SwitchThunder);
	PlayerInputComponent->BindAction("FireOn", IE_Pressed, this, &ATWGBAMCharacter::SwitchFire);
	PlayerInputComponent->BindAction("WaterOn", IE_Pressed, this, &ATWGBAMCharacter::SwitchWater);
	PlayerInputComponent->BindAction("ScrollingDown", IE_Pressed, this, &ATWGBAMCharacter::ScrollDown);
	PlayerInputComponent->BindAction("ScrollingUp", IE_Pressed, this, &ATWGBAMCharacter::ScrollUp);
	PlayerInputComponent->BindAction("AutoRun", IE_Pressed, this, &ATWGBAMCharacter::RunLock);

	HealthWidgetComp->InitWidget();
	UHealthBar* HealthBar = Cast<UHealthBar>(HealthWidgetComp->GetUserWidgetObject());
	HealthBar->setOwner(this);

	HotWidgetComp->InitWidget();
	HotBar = Cast<UHotBar>(HotWidgetComp->GetUserWidgetObject());

	//set up mouse control
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC) {
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;
	}
}


void ATWGBAMCharacter::OnResetVR()
{
	// If TWGBAM is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in TWGBAM.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ATWGBAMCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ATWGBAMCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

/*
void ATWGBAMCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}
void ATWGBAMCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
*/

void ATWGBAMCharacter::RunLock() {
	if (!runOn) {
		runOn = true;
	}
	else {
		runOn = false;
	}
}

void ATWGBAMCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATWGBAMCharacter::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ATWGBAMCharacter::SwitchThunder() {
	ThunderOn = true;
	FireOn = false;
	WaterOn = false;
	HotBar->OnThunder();
}

void ATWGBAMCharacter::SwitchFire() {
	ThunderOn = false;
	FireOn = true;
	WaterOn = false;
	HotBar->OnFire();
}

void ATWGBAMCharacter::SwitchWater() {
	ThunderOn = false;
	FireOn = false;
	WaterOn = true;
	HotBar->OnWater();
}

void ATWGBAMCharacter::ScrollDown() {
	if (scrollLoc < 2) {
		scrollLoc++;
	}
	if (scrollLoc == 0) {
		SwitchThunder();
	}
	else if (scrollLoc == 1) {
		SwitchFire();
	}
	if (scrollLoc == 2) {
		SwitchWater();
	}
}

void ATWGBAMCharacter::ScrollUp() {
	if (scrollLoc > 0) {
		scrollLoc--;
	}
	if (scrollLoc == 0) {
		SwitchThunder();
	}
	else if (scrollLoc == 1) {
		SwitchFire();
	}
	if (scrollLoc == 2) {
		SwitchWater();
	}
}

void ATWGBAMCharacter::Fire() {
	Attacking = true;
	if (ThunderOn && ThunderPickup > 0) {
		//ThunderUsage = ThunderUsage - 10.0f;
		//HotBar->UseThunder(ThunderUsage/MaxSpellUsage);
		//if (ThunderUsage == 0.0f) {
			ThunderPickup--;
		//}
		if (ProjectileClass) {
			FVector CameraLocation;
			FRotator CameraRotation;
			GetActorEyesViewPoint(CameraLocation, CameraRotation);

			MuzzleOffset.Set(100.0f, 0.0f, 0.0f);


			APlayerController* PC = Cast<APlayerController>(GetController());
			FVector MouseLocation;
			FVector MouseDirection;

			PC->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);
			FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
			//DrawDebugLine(GetWorld(), MouseLocation, MouseLocation + MouseDirection * 100.0f, FColor::Red, true);

			FVector OV = MouseLocation + MouseDirection * 100.0f;
			FVector Test;
			Test.Set((CameraLocation.X - OV.X) + CameraLocation.X, OV.Y, OV.Z - 1000.0f);
			TArray<AActor*> ActorsToFind;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), ActorsToFind);
			AActor* ClosestToMouse = GetClosestActor(Test, ActorsToFind);

			if (ClosestToMouse != nullptr) {
				FVector closestRotator = ClosestToMouse->GetActorLocation();
				UE_LOG(LogTemp, Warning, TEXT("MouseWorldX: %f, MouseWorldY: %f, MouseWorldZ: %f"), Test.X, Test.Y, Test.Z);
				UE_LOG(LogTemp, Warning, TEXT("X: %f, Y: %f, Z: %f"), closestRotator.X, closestRotator.Y, closestRotator.Z);
				FRotator MouseRotator = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, closestRotator);
				//UE_LOG(LogTemp, Warning, TEXT("MouseWorldX: %f, MouseWorldY: %f, MouseWorldZ: %f"), OV.X, OV.Y, OV.Z);
				//UE_LOG(LogTemp, Warning, TEXT("CameraX: %f"), CameraLocation.X);

				// Skew the aim to be slightly upwards.
				FRotator MuzzleRotation = MouseRotator;
				//MuzzleRotation.Pitch += 10.0f;

				UWorld* World = GetWorld();
				if (World)
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = this;
					SpawnParams.Instigator = GetInstigator();

					// Spawn the projectile at the muzzle.
					AProjectile* Projectile = World->SpawnActor<AProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, SpawnParams);
					if (Projectile)
					{
						// Set the projectile's initial trajectory.
						FVector LaunchDirection = MuzzleRotation.Vector();
						Projectile->FireInDirection(LaunchDirection);
					}
				}
			}
		}
	}
	else if (FireOn && FirePickup > 0) {
		//FireUsage = FireUsage - 10.0f;
		//HotBar->UseFire(FireUsage/MaxSpellUsage);
		//if (FireUsage == 0.0f) {
			FirePickup--;
		//}
		//if (FireClass) {
			FVector CameraLocation;
			FRotator CameraRotation;
			GetActorEyesViewPoint(CameraLocation, CameraRotation);

			MuzzleOffset.Set(100.0f, 0.0f, 0.0f);


			APlayerController* PC = Cast<APlayerController>(GetController());
			FVector MouseLocation;
			FVector MouseDirection;

			PC->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);
			FVector MuzzleLocation = CameraLocation + FTransform(CameraRotation).TransformVector(MuzzleOffset);
			//DrawDebugLine(GetWorld(), MouseLocation, MouseLocation + MouseDirection * 100.0f, FColor::Red, true);

			FVector OV = MouseLocation + MouseDirection * 100.0f;
			FVector Test;
			//(CameraLocation.X - OV.X) + CameraLocation.X
			Test.Set(OV.X + 700.0f, OV.Y, OV.Z - 1000.0f);
			TArray<AActor*> ActorsToFind;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), ActorsToFind);
			AActor* ClosestToMouse = GetClosestActor(Test, ActorsToFind);

			if (ClosestToMouse != nullptr) {
				FVector closestRotator = ClosestToMouse->GetActorLocation();
				UE_LOG(LogTemp, Warning, TEXT("MouseWorldX: %f, MouseWorldY: %f, MouseWorldZ: %f"), Test.X, Test.Y, Test.Z);
				UE_LOG(LogTemp, Warning, TEXT("X: %f, Y: %f, Z: %f"), closestRotator.X, closestRotator.Y, closestRotator.Z);
				FRotator MouseRotator = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, closestRotator);
				//UE_LOG(LogTemp, Warning, TEXT("MouseWorldX: %f, MouseWorldY: %f, MouseWorldZ: %f"), OV.X, OV.Y, OV.Z);
				//UE_LOG(LogTemp, Warning, TEXT("CameraX: %f"), CameraLocation.X);

				// Skew the aim to be slightly upwards.
				FRotator MuzzleRotation = MouseRotator;
				//MuzzleRotation.Pitch += 10.0f;

				UWorld* World = GetWorld();
				if (World)
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = this;
					SpawnParams.Instigator = GetInstigator();

					// Spawn the projectile at the muzzle.
					AFire* Fire = World->SpawnActor<AFire>(AFire::StaticClass(), MuzzleLocation, MuzzleRotation, SpawnParams);
					if (Fire)
					{
						// Set the projectile's initial trajectory.
						FVector LaunchDirection = MuzzleRotation.Vector();
						Fire->FireInDirection(LaunchDirection);
					}
				}
			}
		//}
	}

	else if (WaterOn && WaterPickup > 0) {
		//WaterUsage = WaterUsage - 10.0f;
		//HotBar->UseWater(WaterUsage/MaxSpellUsage);
		//if (WaterUsage == 0.0f) {
			WaterPickup--;
		//}
		FVector WaterPos;
		WaterPos.Set(this->GetActorLocation().X, this->GetActorLocation().Y, 0.0f);
		UWorld* World = GetWorld();
		if (World) {
			AWater* Water = World->SpawnActor<AWater>(AWater::StaticClass());
			if (Water) {
				Water->Trigger(WaterPos);
			}
		}	 
	}
}

AActor* ATWGBAMCharacter::GetClosestActor(FVector sourceLocation, TArray<AActor*> actors)
{
	if (actors.Num() <= 0)
	{
		return nullptr;
	}

	AActor* closestActor = nullptr;
	float currentClosestDistance = TNumericLimits<float>::Max();

	for (int i = 0; i < actors.Num(); i++)
	{
		if (actors[i]->ActorHasTag("Enemy")) {
			float distance = FVector::DistSquared(sourceLocation, actors[i]->GetActorLocation());
			if (distance < currentClosestDistance)
			{
				currentClosestDistance = distance;
				closestActor = actors[i];
			}
		}
	}

	return closestActor;
}

void ATWGBAMCharacter::SpellOnCooldown() {
	HotBar->SpellsCooling(FireOnCooldown, ThunderOnCooldown, WaterOnCooldown);
}

void ATWGBAMCharacter::Tick(float DeltaTime) {
	if (FirePickup != MaxFirePickup) {
		HotBar->AddFire(FirePickup);
		//HotBar->UseFire(MaxSpellUsage);
		//FireUsage = MaxSpellUsage;
		MaxFirePickup = FirePickup;
	}
	if (FirePickup == 0.0f) {
		HotBar->UseFire(0.0f);
	}
	if (ThunderPickup != MaxThunderPickup) {
		HotBar->AddThunder(ThunderPickup);
		//HotBar->UseThunder(MaxSpellUsage);
		//ThunderUsage = MaxSpellUsage;
		MaxThunderPickup = ThunderPickup;
	}
	if (ThunderPickup == 0.0f) {
		HotBar->UseThunder(0.0f);
	}
	if (WaterPickup != MaxWaterPickup) {
		HotBar->AddWater(WaterPickup);
		//HotBar->UseWater(MaxSpellUsage);
		//WaterUsage = MaxSpellUsage;
		MaxWaterPickup = WaterPickup;
	}
	if (WaterPickup == 0.0f) {
		HotBar->UseWater(0.0f);
	}
	if (runOn) {
		MoveForward(1.0f);
	}
	if (getHealth() <= 0) {
		Destroy();
	}
}