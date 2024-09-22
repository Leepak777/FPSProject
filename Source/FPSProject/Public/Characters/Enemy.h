#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/SphereComponent.h"
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Damage handling function
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    // Enemy health
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Stats")
    float MaxHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
    float CurrentHealth;

    // Death handling function
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

	// Sphere component for potential extra collision (optional)
	UPROPERTY(VisibleAnywhere)
    USphereComponent* CollisionComponent;
};
