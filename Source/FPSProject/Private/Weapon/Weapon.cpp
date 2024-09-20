#include "Weapon/Weapon.h"
#include "Characters/FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
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

        // Cast CurrentOwner to AFPSCharacter and ensure it's valid
        AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(CurrentOwner);
        if (FPSCharacter)
        {
            // Get camera location and rotation
            EyeLocation = FPSCharacter->Camera->GetComponentLocation();
            EyeRotation = FPSCharacter->Camera->GetComponentRotation();

            FVector ShotDirection = EyeRotation.Vector();
            FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

            FHitResult HitResult;
            FCollisionQueryParams QueryParams;
            QueryParams.AddIgnoredActor(this);
            QueryParams.AddIgnoredActor(FPSCharacter);  // Use AddIgnoredActor properly

            // Raycast for hit detection
            if (GetWorld()->LineTraceSingleByChannel(HitResult, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
            {
                AActor* HitActor = HitResult.GetActor();

                if (HitActor)
                {
                    if (HitEffect)
                    {
                        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitEffect, HitResult.ImpactPoint);
                    }
                    
                    if (HitSound)
                    {
                        UGameplayStatics::PlaySoundAtLocation(GetWorld(), HitSound, HitResult.ImpactPoint);
                    }
                    UGameplayStatics::ApplyPointDamage(HitActor, 20.f, ShotDirection, HitResult, CurrentOwner->GetInstigatorController(), this, nullptr);  // Use ApplyPointDamage
                }
            }
            APlayerController* PlayerController = Cast<APlayerController>(FPSCharacter->GetController());
            if (PlayerController)
            {
                // Get the current control rotation and apply the recoil to it
                FRotator CurrentRotation = PlayerController->GetControlRotation();
                CurrentRotation.Pitch -= RecoilAmount;  // Apply recoil by adjusting the pitch
                PlayerController->SetControlRotation(CurrentRotation);
            }
        }
        
        RecoilOffset = FMath::VInterpTo(RecoilOffset, FVector::ZeroVector, GetWorld()->GetDeltaSeconds(), RecoilRecoverySpeed);
        CurrentAmmo--;
        Mesh->PlayAnimation(ShootAnim, false);
        PlayMuzzleFlash();
    }
     if (CurrentAmmo <= 0)
    {
        StartReload();
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
            FPSCharacter->Reload();
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
    CurrentAmmo = MaxAmmo;
    if (ReloadFinishSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ReloadFinishSound, GetActorLocation());
    }
}

void AWeapon::PlayMuzzleFlash()
{
    if (MuzzleFlash)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, Mesh->GetSocketTransform(FName("muzzle")));
    }
}