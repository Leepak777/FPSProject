// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Weapon/Weapon.h"
#include "FPSAnimInstance.generated.h"

UCLASS()  // Corrected macro
class FPSPROJECT_API UFPSAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UFPSAnimInstance();

protected:
    virtual void NativeBeginPlay() override;
    virtual void NativeUpdateAnimation(float DeltaTime) override;  // Fixed typo (Deltatime to DeltaTime)

    UFUNCTION()
    virtual void CurrentWeaponChanged(class AWeapon* NewWeapon, const class AWeapon* OldWeapon);
    
    virtual void SetVars(const float DeltaTime);
    virtual void CalculateWeaponSway(const float DeltaTime);

    virtual void SetIKTransform();

public:
    /*
    * References
    */
    UPROPERTY(BlueprintReadWrite, Category = "Anim")
    class AFPSCharacter* Character;

    UPROPERTY(BlueprintReadWrite, Category = "Anim")
    class USkeletalMeshComponent* Mesh;

    UPROPERTY(BlueprintReadWrite, Category = "Anim")
    class AWeapon* CurrentWeapon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
    FIKProperties IKProperties;

    /*
    *State
    */
    UPROPERTY(BlueprintREadOnly, Category = "Anim")
    FRotator LastRotation;
    
    /*
    *IKVariable
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
    FTransform CameraTransform;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
    FTransform RelativeCameraTransform;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
    FTransform RHandToSightsTransform;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim|IK")
    FTransform OffsetTransform;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
    float ADSWeight = 0.f;

    UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, Category = "Anim")
    bool IsWalking = false;

    UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, Category = "Anim")
    bool IsWalkingBackward = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Anim|IK")
    FTransform CurrentWeaponCustomOffsetTransform;
    /*
    * Accumulate Offset
    */
    UPROPERTY(BlueprintReadWrite, Category = "Anim")
    FRotator AccumulativeRotation;

    UPROPERTY(BlueprintReadWrite, Category = "Anim")
    FRotator AccumulativeRotationInterp; 

    UFUNCTION(BlueprintCallable, Category = "Animation")
    void PlayFireAnimation();
    
    // Animation montage for firing the weapon
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    UAnimMontage* FireMontage;
};
