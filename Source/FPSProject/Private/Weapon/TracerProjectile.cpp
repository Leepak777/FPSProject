// TracerProjectile.cpp

#include "Weapon/TracerProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "Characters/Enemy.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"

ATracerProjectile::ATracerProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->InitSphereRadius(5.0f);
    RootComponent = CollisionComponent;

    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    CollisionComponent->SetNotifyRigidBodyCollision(true);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
    CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

    CollisionComponent->OnComponentHit.AddDynamic(this, &ATracerProjectile::OnHit);

    // Create the projectile movement component
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovementComponent->bAutoActivate = true;
}

void ATracerProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Perform a raycast to visualize the projectile's path
    FVector Start = GetActorLocation();
    FVector ForwardVector = GetActorForwardVector();
    FVector End = Start + (ForwardVector * RaycastDistance); // Define how far you want to trace

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this); // Ignore the projectile itself

    // Perform the raycast
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
    {
        // Optionally, visualize the hit point
        DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Red, false, 5.0f, 0, 1.0f);
    }
    else
    {
        // Draw a line to the end point if no hit
        DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 5.0f, 0, 1.0f);
    }
}


void ATracerProjectile::BeginPlay()
{
    Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Tracer Projectile Spawned"));

    GetWorld()->GetTimerManager().SetTimer(LifetimeTimerHandle, this, &ATracerProjectile::DestroyProjectile, Lifetime, false);
    FVector ForwardDirection = GetActorForwardVector();
    ProjectileMovementComponent->Velocity = ForwardDirection * Speed;
}

void ATracerProjectile::DestroyProjectile()
{
    Destroy();
}

void ATracerProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this)
    {
		UE_LOG(LogTemp, Warning, TEXT("Hit detected with: %s"), *OtherActor->GetName());
        UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetActorForwardVector(), Hit, GetInstigatorController(), this, nullptr);
        Destroy();
    }
}
