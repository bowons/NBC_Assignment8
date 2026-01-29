// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "DebuffItem.generated.h"

class UDebuffEffect;

UCLASS()
class NBC_STUDY_API ADebuffItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debuff")
    TSubclassOf<UDebuffEffect> DebuffToApply;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debuff")
    int32 InitialStacks = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debuff")
    bool bDestroyOnPickup = true;

    virtual void ActivateItem(AActor* Activator) override;
};
