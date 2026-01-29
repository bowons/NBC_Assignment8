// Fill out your copyright notice in the Description page of Project Settings.


#include "DebuffItem.h"
#include "SpartaCharacter.h"
#include "DebuffEffect.h"
#include "DebuffManagerComponent.h"

void ADebuffItem::ActivateItem(AActor* Activator)
{
    Super::ActivateItem(Activator);

    ASpartaCharacter* Character = Cast<ASpartaCharacter>(Activator);
    if (!Character) return;

    UDebuffManagerComponent* DebuffManager = Character->FindComponentByClass<UDebuffManagerComponent>();
    if (DebuffManager && DebuffToApply)
    {
        DebuffManager->ApplyDebuff(DebuffToApply, InitialStacks);
        UE_LOG(LogTemp, Log, TEXT("Debuff applied to player: %s"), *DebuffToApply->GetName());
    }

    if (bDestroyOnPickup)
    {
        DestroyItem();
    }
}
