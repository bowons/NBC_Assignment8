// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentHazardManager.generated.h"

class AEnvironmentHazard;
class ASpawnVolume;
class UParticleSystemComponent;

UCLASS()
class NBC_STUDY_API AEnvironmentHazardManager : public AActor
{
	GENERATED_BODY()
	
public:	
    AEnvironmentHazardManager();

protected:
    virtual void BeginPlay() override;

public:	
    virtual void Tick(float DeltaTime) override;

    UFUNCTION(BlueprintCallable, Category = "Environment")
    void OnWaveStart(int32 WaveIndex);
    UFUNCTION(BlueprintCallable, Category = "Environment")
    void OnWaveEnd();

protected:
    UPROPERTY()
    ASpawnVolume* SpawnVolume;

#pragma region SafeZone
    // Safe Zone 활성화 여부
    bool bSafeZoneActive;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
    FVector SafeZoneCenter;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
    float InitialSafeZoneRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
    float FinalSafeZoneRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
    float CurrentSafeZoneRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
    float OutOfZoneDamage;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
    float DamageTick;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
    float ShrinkStartDelay; // Safe Zone 축소 카운트 지연시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
    float ShrinkDuration;

    float SafeZoneElapsedTime;
    bool bSafeZoneShrinkStarted;
    FTimerHandle DamageTickTimerHandle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
    class UParticleSystem* SafeZoneBoundaryEffect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
    int32 FireRingParticleCount;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
    float FireRingUpdateInterval;

    UPROPERTY()
    TArray<UParticleSystemComponent*> FireParticles;

    FTimerHandle FireRingUpdateTimerHandle;
    float LastUpdatedRadius;

    void InitializeSafeZone();
    void UpdateSafeZoneShrink(float DeltaTime);
    void ApplyOutofZoneDamage();
    bool IsInSafeZone(APawn* Player) const;
    void UpdateFireRing();
    void ClearFireParticles();
#pragma endregion SafeZone

#pragma region ExplosionGimmik
    bool bExplosionSystemActive;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    float ExplosionInterval;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    float ExplosionIntervalRandomRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    float ExplosionWarningTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    float ExplosionRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    float ExplosionDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    int32 SimultaneousExplosions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
    TSubclassOf<AEnvironmentHazard> HazardClass;

    FTimerHandle ExplosionSpawnTimerHandle;

    UPROPERTY()
    TArray<AEnvironmentHazard*> ActiveHazards;

    void InitializeExplosionSystem();

    void SpawnRandomExplosions();

    FVector GetRandomLocationInSafeZone() const;

    void CleanupExplosionSystem();

#pragma endregion ExplosionGimmik

    int32 CurrentWaveIndex;

    void ShowNotification(const FString& Message);
};
