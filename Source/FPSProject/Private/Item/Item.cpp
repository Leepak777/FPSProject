#include "Item/Item.h"
#include "Components/SphereComponent.h"
#include "Characters/FPSCharacter.h"

AItem::AItem()
{
    PickupRadius = CreateDefaultSubobject<USphereComponent>(TEXT("PickupRadius"));
    PickupRadius->InitSphereRadius(200.0f);
    PickupRadius->SetCollisionProfileName(TEXT("Trigger"));
    RootComponent = PickupRadius;

    PickupRadius->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnPickupRadiusEnter);
    PickupRadius->OnComponentEndOverlap.AddDynamic(this, &AItem::OnPickupRadiusExit);

    bCanBePickedUp = true;
    bIsWeapon = false;  // Default to not a weapon
}

void AItem::OnPickup(AFPSCharacter* PlayerCharacter)
{
    if (PlayerCharacter)
    {
		if (bIsWeapon && WeaponClass)
		{
			PlayerCharacter->PickupWeapon(WeaponClass);
		}
		else if(bIsAmmo && WeaponClass)
		{
			PlayerCharacter->IncreaseMaxAmmo(WeaponClass);
		}
		else
		{
			PlayerCharacter->AddToInventory(this);
		}
        // Optionally destroy the item after pickup
        Destroy();
    }
}

void AItem::OnPickupRadiusEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(OtherActor);
    if (PlayerCharacter)
    {
        PlayerCharacter->ShowPickupPrompt(this);
    }
}

void AItem::OnPickupRadiusExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AFPSCharacter* PlayerCharacter = Cast<AFPSCharacter>(OtherActor);
    if (PlayerCharacter)
    {
        PlayerCharacter->HidePickupPrompt();
    }
}
