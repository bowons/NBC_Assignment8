// Fill out your copyright notice in the Description page of Project Settings.


#include "DebuffManagerComponent.h"
#include "SpartaCharacter.h"
#include "DebuffEffect.h"

// Sets default values for this component's properties
UDebuffManagerComponent::UDebuffManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDebuffManagerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UDebuffManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 모든 디버프 업데이트
    for (auto& Pair : ActiveDebuffs)
    {
        UpdateDebuff(Pair.Value, DeltaTime);
    }

    CleanupExpiredDebuffs();
}

void UDebuffManagerComponent::ApplyDebuff(TSubclassOf<UDebuffEffect> DebuffClass, int32 Stacks)
{
    if (!DebuffClass) return;

    UDebuffEffect* DebuffCDO = DebuffClass->GetDefaultObject<UDebuffEffect>();
    if (!DebuffCDO) return;

    ASpartaCharacter* OwnerCharacter = Cast<ASpartaCharacter>(GetOwner());
    if (!OwnerCharacter) return;

    FName DebuffID = DebuffCDO->DebuffID; // 고유 ID 가져오기

    if (ActiveDebuffs.Contains(DebuffID))
    {
        UDebuffEffect* ExistingDebuff = ActiveDebuffs[DebuffID];

        if (ExistingDebuff->bCanStack)
        {
            for (int32 i = 0; i < Stacks; ++i)
            {
                ExistingDebuff->OnStackAdded(OwnerCharacter);
            }
            OnDebuffUpdated.Broadcast(ExistingDebuff);
        }
        else
        {
            // 중첩 불가시 갱신만
            ExistingDebuff->RemainingTime = ExistingDebuff->BaseDuration;
            OnDebuffUpdated.Broadcast(ExistingDebuff);
        }
    }
    else // 없으면 새 디버프 생성
    {
        UDebuffEffect* NewDebuff = NewObject<UDebuffEffect>(this, DebuffClass);
        NewDebuff->OnApplied(OwnerCharacter);

        for (int32 i = 1; i < Stacks; ++i)
        {
            NewDebuff->OnStackAdded(OwnerCharacter);
        }

        ActiveDebuffs.Add(DebuffID, NewDebuff);
        OnDebuffApplied.Broadcast(NewDebuff);
    }
}

void UDebuffManagerComponent::RemoveDebuff(FName DebuffID)
{
    if (UDebuffEffect** FoundDebuff = ActiveDebuffs.Find(DebuffID)) // 포인터 형태로 반환되니까 2중 포인터 해준다
    {
        ASpartaCharacter* OwnerCharacter = Cast<ASpartaCharacter>(GetOwner());
        if (OwnerCharacter)
        {
            // 두번 참조해야 한다 (* 갯수를 참조 횟수로 생각)
            (*FoundDebuff)->OnRemoved(OwnerCharacter);
        }

        OnDebuffRemoved.Broadcast(*FoundDebuff);
        ActiveDebuffs.Remove(DebuffID);
    }
}

bool UDebuffManagerComponent::HasDebuff(FName DebuffID) const
{
    return ActiveDebuffs.Contains(DebuffID);
}

UDebuffEffect* UDebuffManagerComponent::GetDebuff(FName DebuffID) const
{
    if (UDebuffEffect* const* FoundDebuff = ActiveDebuffs.Find(DebuffID))
    {
        return *FoundDebuff;
    }
    return nullptr;
}

TArray<UDebuffEffect*> UDebuffManagerComponent::GetActiveDebuffs() const
{
    TArray<UDebuffEffect*> Result;
    ActiveDebuffs.GenerateValueArray(Result); // 맵의 값들만 배열로 복사해주는 함수
    
    return Result;
}

void UDebuffManagerComponent::UpdateDebuff(UDebuffEffect* Debuff, float DeltaTime)
{
    if (!Debuff) return;

    ASpartaCharacter* OwnerCharacter = Cast<ASpartaCharacter>(GetOwner());
    if (!OwnerCharacter) return;

    Debuff->OnTick(DeltaTime, OwnerCharacter);
}

void UDebuffManagerComponent::CleanupExpiredDebuffs()
{
    TArray<FName> ExpiredDebuffs;

    for (auto& Pair : ActiveDebuffs)
    {
        if (Pair.Value->RemainingTime <= 0.0f)
        {
            ExpiredDebuffs.Add(Pair.Key);
        }
    }

    for (const FName& DebuffID : ExpiredDebuffs)
    {
        RemoveDebuff(DebuffID);
    }
}

