// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LevelItemRow.h"
#include "SpartaGameState.generated.h"

/**
 * 
 */
UCLASS()
class NBC_STUDY_API ASpartaGameState : public AGameState
{
	GENERATED_BODY()

public:
    ASpartaGameState();

    virtual void BeginPlay() override;

    UPROPERTY()
    class ULevelManager* LevelManager;

#pragma region Coin
    // 전역 점수를 저장하는 변수
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Score")
    int32 Score;
    // 현재 레벨에서 스폰된 코인 개수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    int32 SpawnedCoinCount;
    // 플레이어가 수집한 코인 개수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
    int32 CollectedCoinCount;
    // 코인을 주웠을 때 호출
    void OnCoinCollected();
#pragma endregion Coin

#pragma region Level
    // 현재 진행 중인 레벨 인덱스
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
    int32 CurrentLevelIndex;
    // 전체 레벨의 개수
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
    int32 MaxLevels;
    // 실제 레벨 맵 이름 배열. 여기 있는 인덱스를 차례대로 연동
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
    TArray<FName> LevelMapNames;

    // 웨이브와 난이도 테이블 데이터
    UPROPERTY(EditDefaultsOnly, BlueprintReadONly, Category = "Level");
    UDataTable* LevelItemDataTable;

    // 매 레벨이 끝나기 전까지 시간이 흐르도록 관리하는 타이머
    FTimerHandle HUDUpdateTimerHandle;

    // 게임이 완전히 끝났을 때 (모든 레벨 종료) 실행되는 함수
    UFUNCTION(BlueprintCallable, Category = "Level")
    void OnGameOver();
    UFUNCTION()
    void HandleLevelComplete();

    UFUNCTION()
    void HandleItemSpawned(AActor* SpawnedItem);

    void StartLevel();
#pragma endregion Level

    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const;
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);

    
    void UpdateHUD();
};
