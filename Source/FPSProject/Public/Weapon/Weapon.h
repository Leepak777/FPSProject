#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

USTRUCT(BlueprintType)
struct FIKProperties
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class UAnimSequence* AnimPose;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AimOffset = 15.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FTransform CustomOffsetTransform;  
    
      
};

UCLASS(Abstract)
class FPSPROJECT_API AWeapon : public AActor
{
    GENERATED_BODY()
public:
    AWeapon();

protected:
    virtual void BeginPlay() override;

    FTimerHandle ReloadTimerHandle;
    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    int32 MaxAmmo; // Maximum ammo capacity

    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    int32 CurrentAmmo; // Current ammo count

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    float ReloadTime; // Time taken to reload

    // Animation properties
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimSequence* ReloadAnim;  // Reload animation montage

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimSequence* ProneReloadAnim;  // Reload animation montage

    // Sound properties
    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    USoundBase* ReloadSound;  // Reload sound

    UPROPERTY(EditDefaultsOnly, Category = "Sound")
    USoundBase* ReloadFinishSound;  // Sound when reload is complete

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    class UBlendSpace* WeaponBlendSpaceIdleWalkJog;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    class UBlendSpace* WeaponBlendSpaceCrouch;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    class UAnimSequence* WeaponReloadSequence;

private:
    // Animation properties
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimSequence* ShootAnim;  // Reload animation montage

    UPROPERTY(EditDefaultsOnly, Category = "Recoil")
    float RecoilAmount; // Amount of recoil to apply

    UPROPERTY(EditDefaultsOnly, Category = "Recoil")
    float RecoilRecoverySpeed; // Speed at which recoil recovers

    FVector RecoilOffset; // Current recoil offset

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    UParticleSystem* MuzzleFlash; // Muzzle flash particle system

    void PlayMuzzleFlash();
    
    bool CharacterJogging = false;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    UParticleSystem* HitEffect; // Particle system for hit effects

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    USoundBase* HitSound; // Sound effect for hits

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    class USceneComponent* Root;
    // Getter for CurrentAmmo
    int32 GetCurrentAmmo() const { return CurrentAmmo; }

    // Getter for MaxAmmo
    int32 GetMaxAmmo() const { return MaxAmmo; }
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    class USkeletalMeshComponent* Mesh;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName MuzzleSocketName;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
    class AFPSCharacter* CurrentOwner;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
    FIKProperties IKProperties;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
    FTransform PlacementTransform;

    UBlendSpace* GetWeaponBlendSpaceIdleWalkJog() const { return WeaponBlendSpaceIdleWalkJog; }
    UBlendSpace* GetWeaponBlendSpaceCrouch() const { return WeaponBlendSpaceCrouch; }
    UAnimSequence* GetWeaponReloadSequence() const { return WeaponReloadSequence; }
    UAnimSequence* GetWeaponProneReloadSequence() const { return ProneReloadAnim; }

    void Shoot();
    // Reload functionality
    void StartReload();
    void FinishReload();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IK")
    FTransform GetSightsWorldTransform() const;
    virtual FORCEINLINE FTransform GetSightsWorldTransform_Implementation() const { return Mesh->GetSocketTransform(FName("Sights"));}

};