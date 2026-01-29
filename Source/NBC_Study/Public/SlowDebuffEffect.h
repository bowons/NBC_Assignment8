// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DebuffEffect.h"
#include "SlowDebuffEffect.generated.h"

/**
 * 
 */
UCLASS()
class NBC_STUDY_API USlowDebuffEffect : public UDebuffEffect
{
	GENERATED_BODY()
	
public:
    USlowDebuffEffect();
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadONly, Category = "Slow")
    float SpeedReductionPercent = 30.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Slow")
    float StackMultiplier = 0.1f;

    virtual void OnApplied(ASpartaCharacter* Target) override;
    virtual void OnRemoved(ASpartaCharacter* Target) override;
    virtual void OnStackAdded(ASpartaCharacter* Target) override;

private:
    float OriginalSpeed;
    void ApplySpeedModification(ASpartaCharacter* Target);
};
