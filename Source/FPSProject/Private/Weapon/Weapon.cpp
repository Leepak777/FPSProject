#include "Weapon/Weapon.h"


AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(Root);

};

void AWeapon::BeginPlay()
{
    Super::BeginPlay();

    if(!CurrentOwner)
        Mesh->SetVisibility(false);
};