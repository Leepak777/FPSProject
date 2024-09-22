#include "Characters/Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"  // Ensure you include this for movement settings

// Sets default values
AEnemy::AEnemy()
{
    // Set this character to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    MaxHealth = 100.0f;  // Default health
    CurrentHealth = MaxHealth;  // Initialize current health
    
    // Initialize Capsule Component to ensure proper collision
    GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);  // For projectile

    // Ensure movement is enabled and the enemy stays grounded
    GetCharacterMovement()->bOrientRotationToMovement = true;  // Rotate in the direction of movement
    GetCharacterMovement()->GravityScale = 1.0f;  // Use normal gravity
    GetCharacterMovement()->MaxWalkSpeed = 200.0f;  // Set a reasonable speed for the enemy
    GetCharacterMovement()->AirControl = 0.2f;  // Allow some control while in the air

    // Optionally initialize Sphere Component if needed
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    CollisionComponent->SetupAttachment(RootComponent);
    CollisionComponent->SetCollisionProfileName(TEXT("Enemy"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
    Super::BeginPlay();
    CurrentHealth = MaxHealth;  // Ensure health is set on BeginPlay
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}


// TakeDamage function for applying damage
float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    if (CurrentHealth <= 0.0f)
    {
        return 0.0f;  // Already dead
    }
    
    UE_LOG(LogTemp, Warning, TEXT("Enemy took damage: %f, Remaining Health: %f"), DamageAmount, CurrentHealth);
    UE_LOG(LogTemp, Warning, TEXT("Damage taken by %s: %f"), *GetName(), DamageAmount);
    
    // Apply damage to current health
    CurrentHealth -= DamageAmount;
    
    if (CurrentHealth <= 0.0f)
    {
        Die();  // Handle death if health reaches 0
    }

    return DamageAmount;  // Return the amount of damage applied
}

// Handle the enemy's death
void AEnemy::Die()
{
    // Disable enemy collision
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Play death animation if available
    if (DeathAnim)
    {
        UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
        if (AnimInstance)
        {
            AnimInstance->Montage_Play(DeathAnim);
        }
    }

    // Play death sound effect if available
    if (DeathSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
    }

    // Play death particle effect if available
    if (DeathEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DeathEffect, GetActorLocation());
    }

    // Destroy the enemy actor after a short delay
    SetLifeSpan(5.0f);  // Destroy the enemy after 5 seconds
}
