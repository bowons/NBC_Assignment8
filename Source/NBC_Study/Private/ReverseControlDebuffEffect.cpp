// Fill out your copyright notice in the Description page of Project Settings.


#include "ReverseControlDebuffEffect.h"
#include "SpartaCharacter.h"

UReverseControlDebuffEffect::UReverseControlDebuffEffect()
{
    DebuffID = FName("ReverseControl");
    DebuffName = FText::FromString("조작 반전");
    BaseDuration = 5.0f;
    bCanStack = false;
    MaxStacks = 1;
}

void UReverseControlDebuffEffect::OnApplied(ASpartaCharacter* Target)
{
    Super::OnApplied(Target);

    if (Target)
    {
        Target->SetInputReversed(true);
        UE_LOG(LogTemp, Warning, TEXT("Controls reversed!"));
    }
}

void UReverseControlDebuffEffect::OnRemoved(ASpartaCharacter* Target)
{
    Super::OnRemoved(Target);

    if (Target)
    {
        Target->SetInputReversed(false);
        UE_LOG(LogTemp, Log, TEXT("Controls restored!"));
    }
}
