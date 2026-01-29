#include "EnvironmentHazard.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "SpartaCharacter.h"
#include "TimerManager.h"

AEnvironmentHazard::AEnvironmentHazard()
{
    PrimaryActorTick.bCanEverTick = true;

    USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    SetRootComponent(RootComp);

    bIsWarning = false;
    WarningTimeRemaining = 0.0f;
    ExplosionRadius = 300.0f;
    ExplosionDamage = 50.0f;
    ExplosionLocation = FVector::ZeroVector;
}

void AEnvironmentHazard::BeginPlay()
{
    Super::BeginPlay();
}

void AEnvironmentHazard::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsWarning)
    {
        UpdateWarning(DeltaTime);
    }
}

void AEnvironmentHazard::StartWarning(FVector Location, float Radius, float Damage, float WarningDuration)
{
    ExplosionLocation = Location;
    SetActorLocation(Location);
    ExplosionRadius = Radius;
    ExplosionDamage = Damage;
    WarningTimeRemaining = WarningDuration;
    bIsWarning = true;

    UE_LOG(LogTemp, Warning, TEXT("Explosion warning started at %s - Explodes in %.1f seconds"),
        *Location.ToString(), WarningDuration);
}

void AEnvironmentHazard::UpdateWarning(float DeltaTime)
{
    // 이곳에 HUD 업데이트 나중에
    WarningTimeRemaining -= DeltaTime;

    if (WarningTimeRemaining <= 0.0f)
    {
        bIsWarning = false;
        TriggerExplosion();
    }
}

void AEnvironmentHazard::TriggerExplosion()
{
    // 이펙트 생성
    if (ExplosionEffect)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            ExplosionEffect,
            ExplosionLocation,
            FRotator::ZeroRotator,
            FVector(1.0f),
            true,
            EPSCPoolMethod::None,
            true
        );
    }

    // 사운드 생성
    if (ExplosionSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            ExplosionSound,
            ExplosionLocation
        );
    }

    TArray<AActor*> FoundCharacters;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpartaCharacter::StaticClass(), FoundCharacters);

    for (AActor* Actor : FoundCharacters)
    {
        if (!Actor) continue;

        float Distance = FVector::Dist(Actor->GetActorLocation(), ExplosionLocation);
        if (Distance <= ExplosionRadius)
        {
            UGameplayStatics::ApplyDamage(
                Actor,
                ExplosionDamage,
                nullptr,
                this,
                UDamageType::StaticClass()
            );
        }
    }

    GetWorld()->GetTimerManager().SetTimer(
        CleanupTimerHandle,
        this,
        &AEnvironmentHazard::CleanupAfterExplosion,
        2.0f,
        false
    );
}

void AEnvironmentHazard::CleanupAfterExplosion()
{
    Destroy();
}
