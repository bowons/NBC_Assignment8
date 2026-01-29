// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DebuffItem.h"
#include "SlowingPotion.generated.h"

/**
 * 
 */
UCLASS()
class NBC_STUDY_API ASlowingPotion : public ADebuffItem
{
	GENERATED_BODY()
	
public:
    ASlowingPotion();

    virtual void ActivateItem(AActor* Activator) override;
};
