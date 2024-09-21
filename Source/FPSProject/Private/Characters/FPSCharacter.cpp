#include "Characters/FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "Animation/FPSAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CurrentWeaponBlendSpaceIdleWalkJog = nullptr;
	CurrentWeaponBlendSpaceCrouch = nullptr;

	GetMesh()->SetTickGroup(ETickingGroup::TG_PostUpdateWork);
	GetMesh()->bVisibleInReflectionCaptures = true;
	GetMesh()->bCastHiddenShadow = true;
	GetCharacterMovement()->MaxWalkSpeed = 300;
	ClientMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ClientMesh"));
	ClientMesh->SetCastShadow(false);
	ClientMesh->bCastHiddenShadow = false;
	ClientMesh->bVisibleInReflectionCaptures = false;
	ClientMesh->SetTickGroup(ETickingGroup::TG_PostUpdateWork);
	ClientMesh->SetupAttachment(GetMesh());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = true;
	Camera->SetupAttachment(GetMesh(), FName("head"));
}

// Called when the game starts or when spawned
void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
	//SewtupADS timeline
	if(AimingCurve){	
	FOnTimelineFloat TimelineFloat;
	TimelineFloat.BindDynamic(this, &AFPSCharacter::TimelineProgress);

	AimingTimeline.AddInterpFloat(AimingCurve, TimelineFloat);
	}
	//Client Mesh Logic
	if(IsLocallyControlled()){
		ClientMesh->HideBoneByName(FName("neck_01"), EPhysBodyOp::PBO_None);
		GetMesh()->SetVisibility(false);
	}
	else{
		ClientMesh->DestroyComponent();
	}
	//Spawning weapons
	if(HasAuthority())
	{
		for(const TSubclassOf<AWeapon>& WeaponClass : DefaultWeapons)
		{
			if(!WeaponClass) continue;
			FActorSpawnParameters Params;
			Params.Owner = this;
			AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass, Params);
			const int32 Index = Weapons.Add(SpawnedWeapon);
			if(Index == CurrentIndex)
			{
				CurrentWeapon = SpawnedWeapon;
				CurrentWeaponBlendSpaceIdleWalkJog = CurrentWeapon->GetWeaponBlendSpaceIdleWalkJog();
				CurrentWeaponBlendSpaceCrouch = CurrentWeapon->GetWeaponBlendSpaceCrouch();
				CurrentWeaponReloadSequence = CurrentWeapon->GetWeaponReloadSequence();
				ProneReloadSequence = CurrentWeapon->GetWeaponProneReloadSequence();
				// Get the AnimInstance and update its blend space if needed
				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					// Assuming you have a custom animation instance class
					UFPSAnimInstance* FPSAnimInstance = Cast<UFPSAnimInstance>(AnimInstance);
					if (FPSAnimInstance)
					{
						FPSAnimInstance->CurrentWeaponBlendSpaceIdleWalkJog = CurrentWeaponBlendSpaceIdleWalkJog;
						FPSAnimInstance->CurrentWeaponBlendSpaceCrouch = CurrentWeaponBlendSpaceCrouch;
						FPSAnimInstance->CurrentWeaponReloadSequence = CurrentWeaponReloadSequence;
						FPSAnimInstance->ProneReloadSequence = ProneReloadSequence;
					}
				}
				OnRep_CurrentWeapon(nullptr);
			}
		}
	}
	GetCharacterMovement()->MaxWalkSpeed = 300;
}

void AFPSCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
	AimingTimeline.TickTimeline(DeltaTime);
	UpdateAnimationStatus();
}

// Called to bind functionality to input
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(FName("Aim"), EInputEvent::IE_Pressed, this, &AFPSCharacter::StartAiming);
	PlayerInputComponent->BindAction(FName("Aim"), EInputEvent::IE_Released, this, &AFPSCharacter::ReverseAiming);

	PlayerInputComponent->BindAction(FName("Reload"), EInputEvent::IE_Pressed, this, &AFPSCharacter::Reload);

	// Jogging
    PlayerInputComponent->BindAction(FName("Jog"), EInputEvent::IE_Pressed, this, &AFPSCharacter::StartJogging);
    PlayerInputComponent->BindAction(FName("Jog"), EInputEvent::IE_Released, this, &AFPSCharacter::StopJogging);

	// Jogging
    PlayerInputComponent->BindAction(FName("Jump"), EInputEvent::IE_Pressed, this, &AFPSCharacter::StartJump);

    // Crouching
    PlayerInputComponent->BindAction(FName("Crouch"), EInputEvent::IE_Pressed, this, &AFPSCharacter::StartCrouch);
    PlayerInputComponent->BindAction(FName("Crouch"), EInputEvent::IE_Released, this, &AFPSCharacter::StopCrouch);

	PlayerInputComponent->BindAction(FName("Prone"), EInputEvent::IE_Pressed, this, &AFPSCharacter::StartProne);
    PlayerInputComponent->BindAction(FName("Prone"), EInputEvent::IE_Released, this, &AFPSCharacter::StopProne);

	PlayerInputComponent->BindAction(FName("Fire"), EInputEvent::IE_Pressed, this, &AFPSCharacter::StartFiring);

	PlayerInputComponent->BindAction(FName("NextWeapon"), EInputEvent::IE_Pressed, this, &AFPSCharacter::NextWeapon);
	PlayerInputComponent->BindAction(FName("LastWeapon"), EInputEvent::IE_Pressed, this, &AFPSCharacter::LastWeapon);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AFPSCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Lookup"), this, &AFPSCharacter::Lookup);
	PlayerInputComponent->BindAxis(FName("Lookright"), this, &AFPSCharacter::Lookright);
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AFPSCharacter, Weapons, COND_None);
	DOREPLIFETIME_CONDITION(AFPSCharacter, CurrentWeapon, COND_None);
	DOREPLIFETIME_CONDITION(AFPSCharacter, ADSWeight, COND_None);
}

void AFPSCharacter::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	DOREPLIFETIME_ACTIVE_OVERRIDE(AFPSCharacter, ADSWeight, ADSWeight >= 1.f || ADSWeight <= 0.f);
}

void AFPSCharacter::OnRep_CurrentWeapon(const AWeapon* OldWeapon)
{
	if(CurrentWeapon)
	{
		if(!CurrentWeapon->CurrentOwner)
		{
			const FTransform& PlacementTransform = CurrentWeapon->PlacementTransform * GetMesh()->GetSocketTransform(FName("weaponsocket_r"));
			CurrentWeapon->SetActorTransform(PlacementTransform, false, nullptr, ETeleportType::TeleportPhysics);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, FName("weaponsocket_r"));

			CurrentWeapon->CurrentOwner = this;
			//PRINT(TEXT("%s: Visible"), *AUTH);
		}
		CurrentWeapon->Mesh->SetVisibility(true);
		CurrentWeaponName = CurrentWeapon->GetActorNameOrLabel();
	}
	if(OldWeapon)
	{
		OldWeapon->Mesh->SetVisibility(false);
	}
	CurrentWeaponChangeDelegate.Broadcast(CurrentWeapon, OldWeapon);
}

void AFPSCharacter::EquipWeapon(const int32 Index)
{
	if(!Weapons.IsValidIndex(Index) || CurrentWeapon == Weapons[Index]) return;
	if(IsLocallyControlled() || HasAuthority())
	{
		CurrentIndex = Index;

		const AWeapon* OldWeapon = CurrentWeapon;
		CurrentWeapon = Weapons[Index];
		// Update the current blend space from the new weapon's blend space
        CurrentWeaponBlendSpaceIdleWalkJog = CurrentWeapon->GetWeaponBlendSpaceIdleWalkJog();
		CurrentWeaponBlendSpaceCrouch = CurrentWeapon->GetWeaponBlendSpaceCrouch();
        // Get the AnimInstance and update its blend space if needed
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            // Assuming you have a custom animation instance class
            UFPSAnimInstance* FPSAnimInstance = Cast<UFPSAnimInstance>(AnimInstance);
            if (FPSAnimInstance)
            {
                FPSAnimInstance->CurrentWeaponBlendSpaceIdleWalkJog = CurrentWeaponBlendSpaceIdleWalkJog;
				FPSAnimInstance->CurrentWeaponBlendSpaceCrouch = CurrentWeaponBlendSpaceCrouch;
            }
        }
		OnRep_CurrentWeapon(OldWeapon); 
	}
	else if(!HasAuthority())
	{
		Server_SetCurrentWeapon(Weapons[Index]);
	}
}

void AFPSCharacter::Server_SetCurrentWeapon_Implementation(AWeapon* NewWeapon)
{
	const AWeapon* OldWeapon = CurrentWeapon;
	CurrentWeapon = NewWeapon;
	OnRep_CurrentWeapon(OldWeapon);
}

void AFPSCharacter::StartAiming()
{
	if(GetCharacterMovement()->MaxWalkSpeed == 600){
		StopJogging();
		bIsJogging = true;
	}
	if(IsLocallyControlled()|| HasAuthority()){
		Multi_Aim_Implementation(true);
	}
	if(!HasAuthority()){
		Server_Aim(true);
	}
}
void AFPSCharacter::ReverseAiming(){
	if(bIsJogging == true){
		StartJogging();
		bIsJogging = false;
	}
	if(IsLocallyControlled()|| HasAuthority()){
		Multi_Aim_Implementation(false);
	}
	if(!HasAuthority()){
		Server_Aim(false);
	}
}

void AFPSCharacter::Multi_Aim_Implementation(const bool bForward){
	if(bForward)
	{
		AimingTimeline.Play();
	}
	else
	{
		AimingTimeline.Reverse();
	}
}

void AFPSCharacter::StartFiring()
{
    if (CurrentWeapon)
    {
        CurrentWeapon->Shoot();  // Call the Shoot function on the currently equipped weapon
    }
}

void AFPSCharacter::TimelineProgress(const float Value)
{
	ADSWeight = Value;
}

void AFPSCharacter::NextWeapon()
{
	const int32 Index = Weapons.IsValidIndex(CurrentIndex + 1) ? CurrentIndex + 1 : 0;
	EquipWeapon(Index);
}

void AFPSCharacter::LastWeapon()
{	
	const int32 Index = Weapons.IsValidIndex(CurrentIndex - 1) ? CurrentIndex - 1 : Weapons.Num() - 1;
	EquipWeapon(Index);
}

void AFPSCharacter::MoveForward(float Value)
{
	if (!bIsProne)
	{
		const FVector& Direction = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}

}

void AFPSCharacter::MoveRight(float Value)
{
	if (!bIsProne)
	{
		const FVector& Direction = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);	
	}
}

void AFPSCharacter::Lookup(float Value)
{
	AddControllerPitchInput(Value);
}

void AFPSCharacter::Lookright(float Value)
{
	AddControllerYawInput(Value);
}


// Function to update the IsWalking and IsWalkingBackward variables based on movement status
void AFPSCharacter::UpdateAnimationStatus()
{
    // Get the AnimInstance
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        UFPSAnimInstance* FPSAnimInstance = Cast<UFPSAnimInstance>(AnimInstance);
        if (FPSAnimInstance)
        {
            // Get the current velocity
            FVector Velocity = GetVelocity();

            // Get the speed (ignoring the Z component, so we only check X and Y plane movement)
            float Speed = Velocity.Size2D();  // Size2D checks movement in X and Y only

            // Update the IsWalking boolean depending on the character's speed
            if (Speed > 0.0f)
            {
                FPSAnimInstance->IsWalking = true;

                // Get the forward vector of the character
                FVector ForwardVector = GetActorForwardVector();
                
                // Normalize the velocity and get the direction the character is moving
                FVector MovementDirection = Velocity.GetSafeNormal();

                // Use the dot product to check if the movement is forward or backward
                float ForwardDotProduct = FVector::DotProduct(MovementDirection, ForwardVector);

                if (ForwardDotProduct < 0.0f)  // Moving backward
                {
                    FPSAnimInstance->IsWalkingBackward = true;
                }
                else  // Moving forward
                {
                    FPSAnimInstance->IsWalkingBackward = false;
                }
            }
            else
            {
                // Character is not walking
                FPSAnimInstance->IsWalking = false;
                FPSAnimInstance->IsWalkingBackward = false;
            }
			FPSAnimInstance->IsCrouching = bIsCrouching;
            FPSAnimInstance->IsProne = bIsProne;
        }
    }
}

FString AFPSCharacter::GetCurrentWeaponName() const
{
    if (CurrentWeapon)
    {
        return CurrentWeapon->GetName();  // Return the weapon's name
    }
    return "No Weapon";
}

FString AFPSCharacter::GetAmmoText() const
{
    if (CurrentWeapon)
    {
        return FString::Printf(TEXT("%d / %d"), CurrentWeapon->GetCurrentAmmo(), CurrentWeapon->GetMaxAmmo());  // Format as "CurrentAmmo / MaxAmmo"
    }
    return "0 / 0";
}

void AFPSCharacter::StartJogging()
{
    // Set speed for jogging
    GetCharacterMovement()->MaxWalkSpeed = 600; // Define JogSpeed in your class
}

void AFPSCharacter::StopJogging()
{
    // Reset speed to normal walking speed
    GetCharacterMovement()->MaxWalkSpeed = 300; // Define WalkSpeed in your class
}

void AFPSCharacter::Reload()
{
	if(CurrentWeapon->GetCurrentAmmo() == CurrentWeapon->GetMaxAmmo()){return;}
	CurrentWeapon->StartReload();
}

void AFPSCharacter::StartJump()
{
	if (!GetCharacterMovement()->IsFalling())  // Check if the character is on the ground
    {
        Jump();  // Physically jump

        // Play the jump animation depending on the current movement speed
        if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
        {
            if (GetCharacterMovement()->MaxWalkSpeed == 300)
            {
                AnimInstance->Montage_Play(WalkJump);
            }
            else if (GetCharacterMovement()->MaxWalkSpeed == 600)
            {
                AnimInstance->Montage_Play(JogJump);
            }
        }

        bIsJump = true;  // Update the flag to track jumping
    }
}

void AFPSCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);

    // Reset jump flag when landing
    bIsJump = false;
}

void AFPSCharacter::StartCrouch()
{
	if (GetMesh()->GetAnimInstance())
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

        if (AnimInstance)
        {
            AnimInstance->Montage_Play(StandToCrouch);
        }
    }
    if (!bIsProne)
    {
        Crouch();
        bIsCrouching = true;
    }
}

void AFPSCharacter::StopCrouch()
{
    if (!bIsProne)
    {
		if (GetMesh()->GetAnimInstance())
        {
            UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

            if (AnimInstance)
            {
                AnimInstance->Montage_Play(CrouchToStand);
            }
        }
        UnCrouch();
        bIsCrouching = false;
    }
}

void AFPSCharacter::StartProne()
{
	
    if (!bIsProne)
    {
        // Exit crouch if already crouching
        if (bIsCrouching)
        {
            StopCrouch();
        }

        bIsProne = true;
		if (GetMesh()->GetAnimInstance())
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			if (AnimInstance)
			{
				AnimInstance->Montage_Play(StandToProne);
			}
		}
		 AdjustCameraToFloor();
    }
}

void AFPSCharacter::StopProne()
{
    if (bIsProne)
    {

        bIsProne = false;
		if (GetMesh()->GetAnimInstance())
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

			if (AnimInstance)
			{
				AnimInstance->Montage_Play(ProneToStand);
			}
		}
		 AdjustCameraToFloor();
    }
}

void AFPSCharacter::AdjustCameraToFloor()
{
    // Trace from the camera location downward
    FVector CameraLocation = Camera->GetComponentLocation();
    FVector TraceEnd = CameraLocation - FVector(0.f, 0.f, 0.f);  // Trace 100 units down

    FHitResult HitResult;
    FCollisionQueryParams TraceParams(FName(TEXT("CameraFloorTrace")), false, this);

    if (GetWorld()->LineTraceSingleByChannel(HitResult, CameraLocation, TraceEnd, ECC_Visibility, TraceParams))
    {
        // Adjust the camera height if the trace hits the floor
        if (HitResult.bBlockingHit)
        {
            float FloorZ = HitResult.ImpactPoint.Z;
            float CameraZ = CameraLocation.Z;
            if (CameraZ - FloorZ < 10.0f)  // If camera is within 10 units of the floor
            {
                // Raise the camera to stay above the floor
                Camera->SetRelativeLocation(FVector(0.f, 0.f, 10.f));  // Adjust this as necessary
            }
        }
    }
}