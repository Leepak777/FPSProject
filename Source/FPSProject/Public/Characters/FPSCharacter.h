// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <Components/TimelineComponent.h>
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCurrentWeaponChangeDelegate, class AWeapon*, CurrentWeapon, const class AWeapon*, OldWeapon);


UCLASS()
class FPSPROJECT_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

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

    void StartJogging();

    void StopJogging();

    bool bIsProne;
	bool bIsCrouching;

    void StartCrouch();

    void StopCrouch();

    void StartProne();

    void StopProne();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Configurations")
	TArray<TSubclassOf<class AWeapon>> DefaultWeapons;

public:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Replicated, Category = "State")
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

	void Reload();

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

protected:
	virtual void NextWeapon();
	virtual void LastWeapon();
	virtual void StartFiring();
	void MoveForward(const float Value);
	void MoveRight(const float Value);
	void Lookup(const float Value);
	void Lookright(const float Value);
};
