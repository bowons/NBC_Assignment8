// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DebuffEffect.generated.h"

/**
 * 
 */
class ASpartaCharacter;

UCLASS()
class NBC_STUDY_API UDebuffEffect : public UObject
{
	GENERATED_BODY()
	
public:
#pragma region Field
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debuff")
    FName DebuffID;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debuff")
    FText DebuffName;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debuff")
    UTexture2D* DebuffIcon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debuff|Duration")
    float BaseDuration = 5.0f;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debuff|Stacking")
    bool bCanStack = false;
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debuff|Stacking")
    int32 MaxStacks = 5;

    UPROPERTY(BlueprintReadOnly, Category = "Debuff|State")
    float RemainingTime;
    UPROPERTY(BlueprintReadOnly, Category = "Debuff|State")
    int32 CurrentStacks;
#pragma endregion Field

public:
#pragma region Method
    virtual void OnApplied(ASpartaCharacter* Target);
    virtual void OnTick(float DeltaTime, ASpartaCharacter* Target);
    virtual void OnRemoved(ASpartaCharacter* Target);
    virtual void OnStackAdded(ASpartaCharacter* Target);

    UFUNCTION(BlueprintCallable, Category = "Debuff")
    float GetRemainingPercent() const;

    UFUNCTION(BlueprintCallable, Category = "Debuff")
    void AddTime(float AdditionalTime);
#pragma endregion Method
};
