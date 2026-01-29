// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DebuffItem.h"
#include "ReversePotion.generated.h"

/**
 * 
 */
UCLASS()
class NBC_STUDY_API AReversePotion : public ADebuffItem
{
	GENERATED_BODY()

public:
    AReversePotion();

    virtual void ActivateItem(AActor* Activator) override;
	
};
