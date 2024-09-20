#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FPSHUD.generated.h"

UCLASS()
class FPSPROJECT_API AFPSHUD : public AHUD
{
    GENERATED_BODY()

public:
    // Reference to the Widget Blueprint class
    UPROPERTY(EditDefaultsOnly, Category = "HUD")
    TSubclassOf<class UUserWidget> HUDWidgetClass;

    // The instance of the widget
    UPROPERTY()
    class UUserWidget* HUDWidgetInstance;

    virtual void BeginPlay() override;
};
