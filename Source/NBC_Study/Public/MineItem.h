// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "MineItem.generated.h"

/**
 * 
 */
UCLASS()
class NBC_STUDY_API AMineItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
    AMineItem();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item|Component")
    USphereComponent* ExplosionCollision;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
    UParticleSystem* ExplosionParticle;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
    USoundBase* ExplosionSound;

    // 폭발까지 걸리는 시간 (5초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mine")
    float ExplosionDelay;

    // 폭발 범위
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mine")
    float ExplosionRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mine")
    float ExplosionDamage;

    bool bHasExploded;
    FTimerHandle ExplosionTimerHandle;

    virtual void ActivateItem(AActor* Activator) override;
    void Explode();
};
