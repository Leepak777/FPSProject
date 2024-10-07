#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Characters/FPSCharacter.h"
#include "FPSCharacterAIController.generated.h"

class AFPSCharacter;

UCLASS()
class FPSPROJECT_API AFPSCharacterAIController : public AAIController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

private:
    UAIPerceptionComponent* AIPerceptionComponent;
    UAISenseConfig_Sight* SightConfig;
    APawn* PlayerPawn;
    float EnemyDetectionRadius;

    // Pointer to the controlled character (AI)
    AFPSCharacter* ControlledCharacter;

public:
    AFPSCharacterAIController();
    
    // Called when the perception system detects an actor
    void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);
    
    // Moves the AI character towards the target
    void MoveToTarget(AActor* TargetActor);
};