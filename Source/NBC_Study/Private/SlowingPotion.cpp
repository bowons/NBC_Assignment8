// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowingPotion.h"
#include "SlowDebuffEffect.h"

ASlowingPotion::ASlowingPotion()
{
    ItemType = "Slowing";
    DebuffToApply = USlowDebuffEffect::StaticClass();
    InitialStacks = 1;
}

void ASlowingPotion::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);
}
