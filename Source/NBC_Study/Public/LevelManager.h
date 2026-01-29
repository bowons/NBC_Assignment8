// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LevelItemRow.h"
#include "LevelManager.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelCompleteSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSpawnedSignature, AActor*, SpawnedItem);

/**
 * 
 */
UCLASS()
class NBC_STUDY_API ULevelManager : public UObject
{
	GENERATED_BODY()
	
private:
    // 단순히 GameState에서 DataTable을 전달받는 용도로 사용됨
    UPROPERTY()
    UDataTable* LevelItemDataTable;

    UPROPERTY()
    class ASpawnVolume* SpawnVolume;

    UPROPERTY()
    UGameInstance* GameInstance;

    // 1레벨 기준, '1-1' , '1-2' , '1-3' ... 처럼 Wave 단위를 Array로 묶음
    TMap<int32, TArray<FLevelItemRow>> LevelWavesMap;

    int32 CurrentLevelIndex = 0;
    int32 CurrentWaveIndex = 0;

    FTimerHandle WaveTimerHandle;

    void LoadLevelWavesData();
    void OnWaveTimeUp();

public:
    UPROPERTY(BlueprintAssignable, Category = "Level")
    FOnLevelCompleteSignature OnLevelComplete;

    UPROPERTY(BlueprintAssignable, Category = "Level")
    FOnItemSpawnedSignature OnItemSpawned;

    void Initialize(UDataTable* InDataTable, class ASpawnVolume* InSpawnVolume, UGameInstance* InGameInstance);

    void StartWave(int32 WaveIndex);

    void StartLevel();
    bool EndLevel();

    int32 GetCurrentLevelIndex() const { return CurrentLevelIndex; }
    int32 GetCurrentWaveIndex() const { return CurrentWaveIndex; }
    int GetMaxLevels() const { return LevelWavesMap.Num(); }
    float GetRemainingTime() const;
    
    bool CompleteCurrentWave();
};
