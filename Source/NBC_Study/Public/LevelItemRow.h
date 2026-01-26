// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" // FTableRowBase 정의가 들어있는 헤더
#include "LevelItemRow.generated.h"

USTRUCT(BlueprintType)
struct FLevelItemRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 LevelIndex;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 WaveNumber;
 
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ItemCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float LevelDuration = 0.f;
};
