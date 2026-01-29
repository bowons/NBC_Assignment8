// Fill out your copyright notice in the Description page of Project Settings.


#include "SlowDebuffEffect.h"
#include "SpartaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

USlowDebuffEffect::USlowDebuffEffect()
{
    DebuffID = FName("SlowDebuff");
    DebuffName = FText::FromString("둔화");
    BaseDuration = 3.0f;
    bCanStack = true;
    MaxStacks = 5;
}

void USlowDebuffEffect::OnApplied(ASpartaCharacter* Target)
{
    Super::OnApplied(Target);

    if (Target && Target->GetCharacterMovement())
    {
        if (Target->IsSprinting())
        {
            OriginalSpeed = Target->GetNormalSpeed();
        }
        else
        {
            OriginalSpeed = Target->GetCharacterMovement()->MaxWalkSpeed;
        }

        ApplySpeedModification(Target);
    }
}

void USlowDebuffEffect::OnRemoved(ASpartaCharacter* Target)
{
    Super::OnRemoved(Target);
    if (Target && Target->GetCharacterMovement())
    {
        Target->GetCharacterMovement()->MaxWalkSpeed = OriginalSpeed;
    }
}

void USlowDebuffEffect::OnStackAdded(ASpartaCharacter* Target)
{
    Super::OnStackAdded(Target);
    ApplySpeedModification(Target);
}

void USlowDebuffEffect::ApplySpeedModification(ASpartaCharacter* Target)
{
    if (!Target || !Target->GetCharacterMovement()) return;

    float TotalReduction = SpeedReductionPercent + (StackMultiplier * 100.0f * (CurrentStacks - 1));
    TotalReduction = FMath::Clamp(TotalReduction, 0.0f, 90.0f); // 최대 90% 감소

    float NewSpeed = OriginalSpeed * (1.0f - TotalReduction / 100.0f); // ex 1.0 - (30.0f / 100.0f) = 0.7
    Target->GetCharacterMovement()->MaxWalkSpeed = NewSpeed;

    UE_LOG(LogTemp, Log, TEXT("Slow Debuff: Speed reduced to %.1f (%.1f%% reduction, %d stacks)"),
        NewSpeed, TotalReduction, CurrentStacks);
}
