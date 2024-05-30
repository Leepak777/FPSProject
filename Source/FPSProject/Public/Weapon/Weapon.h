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

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    class USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
    class USkeletalMeshComponent* Mesh;


    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
    class AFPSCharacter* CurrentOwner;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
    FIKProperties IKProperties;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configurations")
    FTransform PlacementTransform;

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "IK")
    FTransform GetSightsWorldTransform() const;
    virtual FORCEINLINE FTransform GetSightsWorldTransform_Implementation() const { return Mesh->GetSocketTransform(FName("Sights"));}

};