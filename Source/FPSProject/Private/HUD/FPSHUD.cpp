#include "HUD/FPSHUD.h"
#include "Blueprint/UserWidget.h"

void AFPSHUD::BeginPlay()
{
    Super::BeginPlay();

    // Check if the Widget class is set in the editor
    if (HUDWidgetClass)
    {
        // Create the widget instance and add it to the screen
        HUDWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
        if (HUDWidgetInstance)
        {
            HUDWidgetInstance->AddToViewport();
        }
    }
}
