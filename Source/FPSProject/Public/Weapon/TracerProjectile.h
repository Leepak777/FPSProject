#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "TracerProjectile.generated.h"

UCLASS()
class FPSPROJECT_API ATracerProjectile : public AActor
{
    GENERATED_BODY()

public:
    ATracerProjectile();
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
    float Damage = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Trace")
	float RaycastDistance = 1000.0f; // Adjust this distance as needed

protected:
    virtual void BeginPlay() override;
	virtual void Tick(const float DeltaTime) override;
private:
    UPROPERTY(VisibleAnywhere)
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere)
    
	

    FTimerHandle LifetimeTimerHandle;
    float Lifetime = 3.0f;
    float Speed = 3000.0f;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    void DestroyProjectile();
};
