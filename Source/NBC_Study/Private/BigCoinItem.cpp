// Fill out your copyright notice in the Description page of Project Settings.


#include "BigCoinItem.h"

ABigCoinItem::ABigCoinItem()
{
    PointValue = 50;
    ItemType = "BigCoin";
}

void ABigCoinItem::ActivateItem(AActor* Activator)
{
    // 부모의 기본 점수 획득 로직 사용
    Super::ActivateItem(Activator);
}
