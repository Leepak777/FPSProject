#include "Animation/FPSAnimInstance.h"
#include "Characters/FPSCharacter.h"
#include "Camera/CameraComponent.h"

UFPSAnimInstance::UFPSAnimInstance(){

}

void UFPSAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();

    Character = Cast<AFPSCharacter>(TryGetPawnOwner());
    if(Character)
    {
        Mesh = Character->GetMesh();
        Character->CurrentWeaponChangeDelegate.AddDynamic(this, &UFPSAnimInstance::CurrentWeaponChanged);  // Fixed function signature
        CurrentWeaponChanged(Character->CurrentWeapon, nullptr);

    }
}

void UFPSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if(!Character) return;

    SetVars(DeltaTime);
    CalculateWeaponSway(DeltaTime);
}

void UFPSAnimInstance::SetVars(float DeltaTime)
{
    CameraTransform = FTransform(Character->GetBaseAimRotation(), Character->Camera->GetComponentLocation());

    const FTransform RootOffset = Mesh->GetSocketTransform(FName("root"), RTS_Component).Inverse() * Mesh->GetSocketTransform(FName("ik_hand_root"));
    RelativeCameraTransform = CameraTransform.GetRelativeTransform(RootOffset);
}

void UFPSAnimInstance::CalculateWeaponSway(float DeltaTime)
{

}

void UFPSAnimInstance::CurrentWeaponChanged(AWeapon* NewWeapon, const AWeapon* OldWeapon)  // Fixed function signature and typo
{
    CurrentWeapon = NewWeapon;
    if(CurrentWeapon)
    {
        IKProperties = CurrentWeapon->IKProperties;
    }
    else
    {
        // Handle case where CurrentWeapon is null, if needed
    }
}
