#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/Weapon.h"
#include "Item.generated.h"

UCLASS()
class FPSPROJECT_API AItem : public AActor
{
    GENERATED_BODY()

public:
    AItem();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup")
    class USphereComponent* PickupRadius;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
    FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
    TSubclassOf<class AWeapon> WeaponClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
    bool bCanBePickedUp;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
    bool bIsWeapon;  // Flag to indicate if this item is a weapon

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
    bool bIsAmmo;

    virtual void OnPickup(class AFPSCharacter* PlayerCharacter);

    UFUNCTION()
    void OnPickupRadiusEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPickupRadiusExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
