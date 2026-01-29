// Fill out your copyright notice in the Description page of Project Settings.


#include "ReversePotion.h"
#include "ReverseControlDebuffEffect.h"

AReversePotion::AReversePotion()
{
    DebuffToApply = UReverseControlDebuffEffect::StaticClass();
    InitialStacks = 1;
}

void AReversePotion::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);
}
