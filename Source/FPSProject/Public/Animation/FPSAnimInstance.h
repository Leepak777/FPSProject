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

    virtual void CurrentWeaponChanged(class AWeapon* NewWeapon, const class AWeapon* OldWeapon);
    virtual void SetVars(const float DeltaTime);
    virtual void CalculateWeaponSway(const float DeltaTime);

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
    *IKVariable
    */
   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
   FTransform CameraTransform;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
   FTransform RelativeCameraTransform;
};