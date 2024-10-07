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
}

void AFPSCharacterAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (ControlledCharacter && ControlledCharacter->GetTargetActor())
    {
        FVector TargetLocation = ControlledCharacter->GetTargetActor()->GetActorLocation();
        FRotator LookAtRotation = (TargetLocation - ControlledCharacter->GetActorLocation()).Rotation();

        // Debugging
        UE_LOG(LogTemp, Warning, TEXT("Target Location: %s"), *TargetLocation.ToString());
        UE_LOG(LogTemp, Warning, TEXT("Current Rotation: %s"), *ControlledCharacter->GetActorRotation().ToString());

        ControlledCharacter->SetActorRotation(FMath::RInterpTo(ControlledCharacter->GetActorRotation(), LookAtRotation, DeltaTime, 5.0f));
        ControlledCharacter->StartFiring();
    }
}

void AFPSCharacterAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
   if (Actor->IsA<AFPSCharacter>())
    {
        if (Stimulus.WasSuccessfullySensed()) // Check if the actor was sensed successfully
        {
            MoveToActor(Actor);
            ControlledCharacter->SetTargetActor(Actor);
        }
        else
        {
            // Target is lost; handle accordingly
            ControlledCharacter->SetTargetActor(nullptr);
        }
    }
}

void AFPSCharacterAIController::MoveToTarget(AActor* TargetActor)
{
    MoveToActor(TargetActor, 5.0f); // 5.0f is the acceptable radius
}
