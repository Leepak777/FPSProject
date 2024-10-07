// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Weapon/Weapon.h"
#include "EnAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class FPSPROJECT_API UEnAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
    UEnAnimInstance();

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
    class AEnemy* Character;

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

    UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, Category = "Movement")
    bool IsCrouching = false;

    UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, Category = "Movement")
    bool IsReloading = false;

    UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, Category = "Movement")
    bool IsProne = false;

    UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, Category = "Movement")
    bool IsInAir = false;

    UPROPERTY(BlueprintReadWrite, BlueprintReadWrite, Category = "Movement")
    bool IsDead = false;

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


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    class UBlendSpace* CurrentWeaponBlendSpaceIdleWalkJog;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    class UBlendSpace* CurrentWeaponBlendSpaceCrouch;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    class UAnimSequence* CurrentWeaponReloadSequence;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    class UAnimSequence* ProneReloadSequence;
};

