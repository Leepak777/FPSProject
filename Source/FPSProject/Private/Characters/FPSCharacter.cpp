#include "Characters/FPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Weapon.h"
#include "Animation/FPSAnimInstance.h"

// Sets default values
AFPSCharacter::AFPSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetTickGroup(ETickingGroup::TG_PostUpdateWork);
	GetMesh()->bVisibleInReflectionCaptures = true;
	GetMesh()->bCastHiddenShadow = true;

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
				OnRep_CurrentWeapon(nullptr);
			}
		}
	}
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
	if(IsLocallyControlled()|| HasAuthority()){
		Multi_Aim_Implementation(true);
	}
	if(!HasAuthority()){
		Server_Aim(true);
	}
}
void AFPSCharacter::ReverseAiming(){
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

void AFPSCharacter::Reload()
{
	//ClientMesh->PlayAnimation(ReloadAnim, false);
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
	const FVector& Direction = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::X);
	AddMovementInput(Direction, Value);

}

void AFPSCharacter::MoveRight(float Value)
{
	const FVector& Direction = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f)).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Value);
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
        }
    }
}


