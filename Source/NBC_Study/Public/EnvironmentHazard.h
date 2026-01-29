// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentHazard.generated.h"

UCLASS()
class NBC_STUDY_API AEnvironmentHazard : public AActor
{
    GENERATED_BODY()

public:
    AEnvironmentHazard();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Hazard")
    void StartWarning(FVector Location, float Radius, float Damage, float WarningDuration);

protected:
    FVector ExplosionLocation;
    float WarningTimeRemaining;
    bool bIsWarning;

    void UpdateWarning(float DeltaTime);

    float ExplosionRadius;
    float ExplosionDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    UParticleSystem* ExplosionEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    USoundBase* ExplosionSound;

    void TriggerExplosion();

    void CleanupAfterExplosion();

    FTimerHandle CleanupTimerHandle;
};