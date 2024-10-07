#include "Characters/FPSCharacterAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionTypes.h"
#include "NavigationSystem.h"
#include "Characters/FPSCharacter.h"

AFPSCharacterAIController::AFPSCharacterAIController()
{
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = 1000.0f;
    SightConfig->LoseSightRadius = 1200.0f;
    SightConfig->PeripheralVisionAngleDegrees = 90.0f;

    AIPerceptionComponent->ConfigureSense(*SightConfig);
    AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AFPSCharacterAIController::OnTargetDetected);
}

void AFPSCharacterAIController::BeginPlay()
{
    Super::BeginPlay();
    
    PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    EnemyDetectionRadius = 10000.0f;  // Example detection range
    ControlledCharacter = Cast<AFPSCharacter>(GetPawn()); // Initialize ControlledCharacter
    
    if (!ControlledCharacter) {
        UE_LOG(LogTemp, Error, TEXT("ControlledCharacter is not valid! Check if the pawn is set correctly."));
    }
}

void AFPSCharacterAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (ControlledCharacter && ControlledCharacter->GetTargetActor())
    {
        FVector TargetLocation = ControlledCharacter->GetTargetActor()->GetActorLocation();
        FRotator LookAtRotation = (TargetLocation - ControlledCharacter->GetActorLocation()).Rotation();

        // Debugging
        UE_LOG(LogTemp, Warning, TEXT("Aiming at target: %s"), *ControlledCharacter->GetTargetActor()->GetName());

        ControlledCharacter->SetActorRotation(FMath::RInterpTo(ControlledCharacter->GetActorRotation(), LookAtRotation, DeltaTime, 5.0f));

        // Start firing if the target is within a certain range
        float DistanceToTarget = FVector::Dist(ControlledCharacter->GetActorLocation(), TargetLocation);
        if (DistanceToTarget < EnemyDetectionRadius)
        {
            ControlledCharacter->StartFiring(); // Call the firing function
        }

        // Move toward the target if it's farther than a certain distance
        float MoveDistanceThreshold = 300.0f; // Threshold to start moving
        if (DistanceToTarget > MoveDistanceThreshold)
        {
            MoveToActor(ControlledCharacter->GetTargetActor(), 5.0f); // Move towards the target
        }
    }
}

void AFPSCharacterAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
    // Check if the detected actor is the player character
    if (Actor->IsA<AFPSCharacter>() && Actor == PlayerPawn && Stimulus.WasSuccessfullySensed())
    {
        UE_LOG(LogTemp, Warning, TEXT("Target detected: %s"), *Actor->GetName());
        
        MoveToActor(Actor); // Move towards the detected player
        ControlledCharacter->SetTargetActor(Actor);
    }
    else
    {
        // Target is lost; handle accordingly
        ControlledCharacter->SetTargetActor(nullptr);
    }
}

void AFPSCharacterAIController::MoveToTarget(AActor* TargetActor)
{
    if (TargetActor) {
        MoveToActor(TargetActor, 25.0f); // 5.0f is the acceptable radius
    } else {
        UE_LOG(LogTemp, Warning, TEXT("Target actor is null, cannot move to target."));
    }
}
