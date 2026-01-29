// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DebuffEffect.h"
#include "ReverseControlDebuffEffect.generated.h"

/**
 * 
 */
UCLASS()
class NBC_STUDY_API UReverseControlDebuffEffect : public UDebuffEffect
{
	GENERATED_BODY()
	
public:
    UReverseControlDebuffEffect();

    virtual void OnApplied(ASpartaCharacter* Target) override;
    virtual void OnRemoved(ASpartaCharacter* Target) override;
};
