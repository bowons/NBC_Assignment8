// Fill out your copyright notice in the Description page of Project Settings.


#include "DebuffEffect.h"

void UDebuffEffect::OnApplied(ASpartaCharacter* Target)
{
    RemainingTime = BaseDuration;
    CurrentStacks = 1;
    // 추가 구현은 상속
}

void UDebuffEffect::OnTick(float DeltaTime, ASpartaCharacter* Target)
{
    RemainingTime -= DeltaTime;
    // 추가 내용은 상속
}

void UDebuffEffect::OnRemoved(ASpartaCharacter* Target)
{
    // 상속 시 구현
}

void UDebuffEffect::OnStackAdded(ASpartaCharacter* Target)
{
    CurrentStacks = FMath::Min(CurrentStacks + 1, MaxStacks);
}

float UDebuffEffect::GetRemainingPercent() const
{
    return BaseDuration > 0.0f ? (RemainingTime / BaseDuration) : 0.0f;
}

void UDebuffEffect::AddTime(float AdditionalTime)
{
    RemainingTime += AdditionalTime;
}
