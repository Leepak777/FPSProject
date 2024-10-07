// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/EnAnimInstance.h"
#include <Kismet/KismetMathLibrary.h>
#include "Characters/Enemy.h"
#include "Camera/CameraComponent.h"


UEnAnimInstance::UEnAnimInstance(){

}

void UEnAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();

    /*Character = Cast<AFPSCharacter>(TryGetPawnOwner());
    if(Character)
    {
        Mesh = Character->GetMesh();
        Character->CurrentWeaponChangeDelegate.AddDynamic(this, &UEnAnimInstance::CurrentWeaponChanged);  // Fixed function signature
        CurrentWeaponChanged(Character->CurrentWeapon, nullptr);

    }*/
}

void UEnAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
    Super::NativeUpdateAnimation(DeltaTime);

    if(!Character)
    {
        Character = Cast<AEnemy>(TryGetPawnOwner());
        if(Character)
        {
            Mesh = Character->GetMesh();
            Character->CurrentWeaponChangeDelegate.AddDynamic(this, &UEnAnimInstance::CurrentWeaponChanged);
            CurrentWeaponChanged(Character->CurrentWeapon, nullptr);
        }
        else return;
    }

    SetVars(DeltaTime);
    CalculateWeaponSway(DeltaTime);

    LastRotation = CameraTransform.Rotator();
}

void UEnAnimInstance::SetVars(float DeltaTime)
{
    CameraTransform = FTransform(Character->GetBaseAimRotation(), Character->Camera->GetComponentLocation());

    const FTransform RootOffset = Mesh->GetSocketTransform(FName("root"), RTS_Component).Inverse() * Mesh->GetSocketTransform(FName("ik_hand_root"));
    RelativeCameraTransform = CameraTransform.GetRelativeTransform(RootOffset);

    ADSWeight = Character->ADSWeight;

    /*
    *OFFSETS
    */
    //Accumulative rotation
    constexpr float AngleClamp = 6.f;
    const FRotator& AddRotation = CameraTransform.Rotator() - LastRotation;
    FRotator AddRotationClamped = FRotator(FMath::ClampAngle(AddRotation.Pitch, -AngleClamp, AngleClamp)*1.5f, 
        FMath::ClampAngle(AddRotation.Yaw, -AngleClamp, AngleClamp), 0.f);
    AddRotationClamped.Roll = AddRotationClamped.Yaw *0.7f;

    AccumulativeRotation += AddRotationClamped;
    AccumulativeRotation = UKismetMathLibrary::RInterpTo(AccumulativeRotation, FRotator::ZeroRotator, DeltaTime, 30.f);
    AccumulativeRotationInterp = UKismetMathLibrary::RInterpTo(AccumulativeRotationInterp, AccumulativeRotation, DeltaTime, 5.f);
}

void UEnAnimInstance::CalculateWeaponSway(float DeltaTime)
{
    FVector LocationOffset = FVector::ZeroVector;
    FRotator RotationOffset = FRotator::ZeroRotator;

    const FRotator& AccumulativeRotationInterpInverse = AccumulativeRotationInterp.GetInverse();
    RotationOffset += AccumulativeRotationInterpInverse;

    LocationOffset += FVector(0.f, AccumulativeRotationInterpInverse.Yaw, AccumulativeRotationInterpInverse.Pitch)/6.f;

    OffsetTransform = FTransform(RotationOffset, LocationOffset);
}

void UEnAnimInstance::CurrentWeaponChanged(AWeapon* NewWeapon, const AWeapon* OldWeapon)  // Fixed function signature and typo
{
    CurrentWeapon = NewWeapon;
    if(CurrentWeapon)
    {
        IKProperties = CurrentWeapon->IKProperties;
        GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UEnAnimInstance::SetIKTransform);
    }
    else
    {
        // Handle case where CurrentWeapon is null, if needed
    }
}

void UEnAnimInstance::SetIKTransform()
{
    RHandToSightsTransform = CurrentWeapon->GetSightsWorldTransform().GetRelativeTransform(Mesh->GetSocketTransform(FName("hand_r")));
}

void UEnAnimInstance::PlayFireAnimation()
{
    if (FireMontage)
    {
        Montage_Play(FireMontage);  // Play the fire montage when shooting
    }
}
