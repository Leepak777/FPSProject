// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/TimelineComponent.h>
#include "CoreMinimal.h"
#include "Characters/FPSCharacter.h"
#include "GameFramework/Character.h"
#include "Animation/EnAnimInstance.h"
#include "Item/Item.h"
#include "Enemy.generated.h"


UCLASS()
class FPSPROJECT_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float MaxHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    float CurrentHealth;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class USkeletalMeshComponent* ClientMesh;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
    FString GetCurrentWeaponName() const;

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    FString GetAmmoText() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* StandToCrouch;  

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* CrouchToStand; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* StandToProne;  

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* ProneToStand; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* WalkJump;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation")
    UAnimMontage* JogJump;   


    UFUNCTION(BlueprintCallable)
    void Heal(float HealAmount);

    void StartJogging();

    void StopJogging();

    bool bIsProne = false;
	bool bIsCrouching = false;
	bool bIsJogging = false;
	bool bIsJump = false;
	bool bIsDead = false;

    void StartCrouch();

    void StopCrouch();

    void StartProne();

    void StopProne();

	void StartJump();

    void Landed(const FHitResult &Hit);

    void Reload();

	void AdjustCameraToFloor();

	void Die();

    // Death animation
    UPROPERTY(EditAnywhere, Category = "Animation")
    UAnimMontage* DeathAnim;

	// Death particle effect
    UPROPERTY(EditAnywhere, Category = "Effects")
    UParticleSystem* DeathEffect;

    // Sound effect for death
    UPROPERTY(EditAnywhere, Category = "Audio")
    USoundBase* DeathSound;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Configurations")
	TArray<TSubclassOf<class AWeapon>> DefaultWeapons;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Inventory")
    TArray<AItem*> Inventory;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category = "Inventory|State")
	TArray<class AWeapon*> Weapons;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_CurrentWeapon, Category = "State")
	class AWeapon* CurrentWeapon;

	UPROPERTY(BlueprintReadWrite)
	FString CurrentWeaponName;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FCurrentWeaponChangeDelegate CurrentWeaponChangeDelegate;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "State")
	int32 CurrentIndex = 0;

	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual void EquipWeapon(const int32 Index);

protected:
	UFUNCTION()
	virtual void OnRep_CurrentWeapon(const class AWeapon* OldWeapon);

	UFUNCTION(Server, Reliable)
	void Server_SetCurrentWeapon(class AWeapon* weapon);
	virtual void Server_SetCurrentWeapon_Implementation(class AWeapon* weapon);
	

public:
	UFUNCTION(BluePrintCallable, Category = "Anim")
	virtual void StartAiming();

	UFUNCTION(BlueprintCallable, Category = "Anim")
	virtual void ReverseAiming();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, REplicated, Category = "Anim")
	float ADSWeight = 0.f;

	virtual float TakeDamage(
        float DamageAmount, 
        struct FDamageEvent const& DamageEvent, 
        class AController* EventInstigator, 
        AActor* DamageCauser
    ) override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Configureations|Anim")
	class UCurveFloat* AimingCurve;

	FTimeline AimingTimeline;

	UFUNCTION(Server, Reliable)
	void Server_Aim(const bool bForward = true);
	virtual FORCEINLINE void Server_Aim_Implementation(const bool bForward)
	{
		Multi_Aim(bForward);
		Multi_Aim_Implementation(bForward);
	}

	UFUNCTION(NetMulticast, Reliable)
	void Multi_Aim(const bool bForward);
	virtual void Multi_Aim_Implementation(const bool bForward);

	UFUNCTION()
	virtual void TimelineProgress(const float value);

	void UpdateAnimationStatus();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    class UBlendSpace* CurrentWeaponBlendSpaceIdleWalkJog;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
    class UBlendSpace* CurrentWeaponBlendSpaceCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    class UAnimSequence* CurrentWeaponReloadSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    class UAnimSequence* ProneReloadSequence;

protected:
	virtual void NextWeapon();
	virtual void LastWeapon();
	virtual void StartFiring();
	void MoveForward(const float Value);
	void MoveRight(const float Value);
	void Lookup(const float Value);
	void Lookright(const float Value);
};
