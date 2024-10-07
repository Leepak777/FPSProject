#include "Weapon/Weapon.h"
#include "Characters/FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon/TracerProjectile.h"
#include "Characters/Enemy.h"
#include "Animation/FPSAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"


AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Root);

    MuzzleSocketName = "Muzzle";

};

void AWeapon::BeginPlay()
{
    Super::BeginPlay();
    CurrentAmmo = MaxAmmo;
    if(!CurrentOwner)
        Mesh->SetVisibility(false);
};

void AWeapon::Shoot()
{
    if (GetWorld()->GetTimerManager().IsTimerActive(ReloadTimerHandle)) return;
    if (CurrentAmmo <= 0)
    {
        // Play dry fire sound or animation
        return;
    }

    if (CurrentOwner)
    {
        FVector EyeLocation;
        FRotator EyeRotation;

        AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(CurrentOwner);
        if (FPSCharacter)
        {
            if (FPSCharacter->IsPlayerControlled())
            {
                // Player controlled - use camera's location and rotation
                EyeLocation = FPSCharacter->Camera->GetComponentLocation();
                EyeRotation = FPSCharacter->Camera->GetComponentRotation();
            }
            else
            {
                // AI controlled - use the AI character's facing direction
                EyeLocation = FPSCharacter->GetActorLocation();
                EyeRotation = FPSCharacter->GetControlRotation(); // AI's control rotation
            }

            FVector ShotDirection = EyeRotation.Vector(); // The direction to shoot
            
            if (TracerProjectileClass)
            {
                FVector MuzzleLocation = Mesh->GetSocketLocation("MuzzleFlash");
                
                // Spawn the projectile at the muzzle location
                ATracerProjectile* Projectile = GetWorld()->SpawnActor<ATracerProjectile>(TracerProjectileClass, MuzzleLocation, EyeRotation);
                
                if (Projectile)
                {
                    // Set the direction and velocity using the ShotDirection
                    Projectile->SetActorRotation(ShotDirection.Rotation());
                    Projectile->ProjectileMovementComponent->Velocity = ShotDirection * Projectile->ProjectileMovementComponent->InitialSpeed;

                    // Debugging: Visualize the shot direction
                    DrawDebugLine(GetWorld(), MuzzleLocation, MuzzleLocation + (ShotDirection * 1000.0f), FColor::Blue, false, 2.0f);
                }
            }

            // Reduce current ammo and play animations
            CurrentAmmo--;
            Mesh->PlayAnimation(ShootAnim, false);
            //UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
            PlayMuzzleFlash();

            if (CurrentAmmo <= 0)
            {
                StartReload();
            }
        }
    }
}



void AWeapon::StartReload()
{
    if (CurrentAmmo < MaxAmmo)
    {
        // Play reload animation if available
        
        if (ReloadAnim)
        {
            ReloadTime = ReloadAnim->GetPlayLength();
            AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(CurrentOwner);
            UAnimInstance* AnimInstance = FPSCharacter->GetMesh()->GetAnimInstance();
            UFPSAnimInstance* FPSAnimInstance = Cast<UFPSAnimInstance>(AnimInstance);
            if(CurrentOwner->GetCharacterMovement()->MaxWalkSpeed == 600)
            {
                CharacterJogging = true;
                CurrentOwner->GetCharacterMovement()->MaxWalkSpeed = 300;
            }
            FPSAnimInstance->IsReloading= true;
            Mesh->PlayAnimation(ReloadAnim, false);
        }

        // Play reload sound if available
        if (ReloadSound)
        {
            UGameplayStatics::PlaySoundAtLocation(this, ReloadSound, GetActorLocation());
        }

        // Start reload timer
        GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &AWeapon::FinishReload, ReloadTime, false);
    }
}

void AWeapon::FinishReload()
{
    MaxAmmo -= (MegAmount - CurrentAmmo);
    CurrentAmmo = MegAmount;
    if (ReloadFinishSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ReloadFinishSound, GetActorLocation());
    }
    AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(CurrentOwner);
    UAnimInstance* AnimInstance = FPSCharacter->GetMesh()->GetAnimInstance();
    UFPSAnimInstance* FPSAnimInstance = Cast<UFPSAnimInstance>(AnimInstance);
    if(CurrentOwner->GetCharacterMovement()->MaxWalkSpeed == 300 && CharacterJogging == true)
    {
        CharacterJogging = false;
        CurrentOwner->GetCharacterMovement()->MaxWalkSpeed = 600;
    }
    FPSAnimInstance->IsReloading= false;
}

void AWeapon::MaxAmmoIncrease()
{
    MaxAmmo += 30;
}

void AWeapon::PlayMuzzleFlash()
{
    if (MuzzleFlash)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, Mesh->GetSocketTransform(FName("muzzle")));
    }
}