// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SpartaGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NBC_STUDY_API USpartaGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    USpartaGameInstance();

    // 게임 전체 누적 점수
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="GameData")
    int32 TotalScore;
    // 현재 레벨 인덱스 (GameState에서도 관리할 수 있지만, 맵 전환 후에도 살리고 싶다면 GameInstance에 복사 가능)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="GameData")
    int32 CurrentLevelIndex;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GameData")
    int32 CurrentWaveIndex;

    UFUNCTION(BlueprintCallable, Category = "GameData")
    void AddToScore(int32 Amount);
};
