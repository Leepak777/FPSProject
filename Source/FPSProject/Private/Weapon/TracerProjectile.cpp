#include "Weapon/TracerProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "DrawDebugHelpers.h" 
#include "Components/SphereComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"


ATracerProjectile::ATracerProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
    CollisionComponent->InitSphereRadius(3.0f);
    CollisionComponent -> SetCollisionResponseToAllChannels(ECR_Block);
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
    RootComponent = CollisionComponent;

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
    ProjectileMovementComponent->InitialSpeed = 9000.0f; 
    ProjectileMovementComponent->MaxSpeed = 9000.0f; 
    ProjectileMovementComponent->bRotationFollowsVelocity = true; 
    ProjectileMovementComponent->bShouldBounce = false; 
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f; 

    CollisionComponent->OnComponentHit.AddDynamic(this, &ATracerProjectile::OnHit);
}

void ATracerProjectile::BeginPlay()
{
    Super::BeginPlay();

    GetWorld()->GetTimerManager().SetTimer(LifetimeTimerHandle, this, &ATracerProjectile::DestroyProjectile, Lifetime, false);

}

void ATracerProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
     // Perform a raycast to visualize the projectile's path
    FVector Start = GetActorLocation();
    FVector ForwardVector = GetActorForwardVector();
    FVector End = Start + (ForwardVector * 1000.0f); // Adjust the distance as needed

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this); // Ignore the projectile itself

    // Perform the raycast
    if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams))
    {
        // Optionally, visualize the hit point
        DrawDebugLine(GetWorld(), Start, HitResult.Location, FColor::Green, false, 5.0f, 0, 1.0f);
    }
    else
    {
        // Draw a line to the end point if no hit
        DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 5.0f, 0, 1.0f);
    }

}

void ATracerProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor != this)
    {
        UGeometryCollectionComponent* GCComponent = Cast<UGeometryCollectionComponent>(OtherComp);
        if (GCComponent)
        {
            GCComponent->AddImpulseAtLocation(Hit.ImpactNormal * 100000.0f, Hit.ImpactPoint);
        }
        UGameplayStatics::ApplyPointDamage(OtherActor, Damage, GetActorForwardVector(), Hit, GetInstigatorController(), this, nullptr);
        DestroyProjectile();
    }
    
}

void ATracerProjectile::DestroyProjectile()
{
    Destroy();
}
