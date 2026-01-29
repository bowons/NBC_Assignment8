// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelManager.h"
#include "SpartaPlayerController.h"
#include "SpartaGameInstance.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "EnvironmentHazardManager.h"

void ULevelManager::LoadLevelWavesData()
{
    LevelWavesMap.Empty();

    if (LevelItemDataTable)
    {
        TArray<FName> RowNames = LevelItemDataTable->GetRowNames();

        for (const FName& RowName : RowNames)
        {
            // TEXT("") 는 로깅 용도로 빈 문자열 전달
            FLevelItemRow* LevelItemRow = LevelItemDataTable->FindRow<FLevelItemRow>(RowName, TEXT(""));
            
            if (LevelItemRow)
            {
                FString RowString = RowName.ToString();
                TArray<FString> Parts;
                RowString.ParseIntoArray(Parts, TEXT("-"), true); // '1-1' -> ['1', '1']

                if (Parts.Num() == 2)
                {
                    int32 Level = FCString::Atoi(*Parts[0]);
                    
                    if (!LevelWavesMap.Contains(Level))
                    {
                        LevelWavesMap.Add(Level, TArray<FLevelItemRow>());
                    }
                    // 레벨 1 기준, '1-1', '1-2', '1-3' ... 처럼 Wave 단위를 Array로 묶음
                    LevelWavesMap[Level].Add(*LevelItemRow);
                }
            }
        }
    }

}

void ULevelManager::OnWaveTimeUp()
{
    CompleteCurrentWave();
}

void ULevelManager::Initialize(UDataTable* InDataTable, ASpawnVolume* InSpawnVolume, UGameInstance* InGameInstance)
{
    LevelItemDataTable = InDataTable;
    SpawnVolume = InSpawnVolume;
    GameInstance = InGameInstance;

    LoadLevelWavesData();
}

void ULevelManager::StartWave(int32 WaveIndex)
{
    if (!LevelItemDataTable) return;

    if (!SpawnVolume) return;

    // CurrentLevelIndex + 1 + '-' + WaveIndex + 1를 기준으로 RowName로 Map에서 찾음
    FString RowNameString = FString::Printf(TEXT("%d-%d"), CurrentLevelIndex + 1, WaveIndex + 1);
    FName RowName(*RowNameString);

    FLevelItemRow* LevelItemRow = LevelItemDataTable->FindRow<FLevelItemRow>(RowName, TEXT(""));

    if (LevelItemRow)
    {
        int32 ItemSpawnCount = LevelItemRow->ItemCount;

        UE_LOG(LogTemp, Warning, TEXT("Wave %d Start!"), WaveIndex + 1);

        for (int32 i = 0; i < ItemSpawnCount; ++i)
        {
            if (SpawnVolume)
            {
                AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
                OnItemSpawned.Broadcast(SpawnedActor);
            }
        }

        TArray<AActor*> FoundManagers;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnvironmentHazardManager::StaticClass(), FoundManagers);
        if (FoundManagers.Num() > 0)
        {
            AEnvironmentHazardManager* HazardManager = Cast<AEnvironmentHazardManager>(FoundManagers[0]);
            if (HazardManager)
            {
                HazardManager->OnWaveStart(WaveIndex);
            }
        }

        if (GetWorld())
        {
            // 웨이브 타이머 설정
            GetWorld()->GetTimerManager().SetTimer(
                WaveTimerHandle,
                this,
                &ULevelManager::OnWaveTimeUp,
                LevelItemRow->LevelDuration,
                false
            );
        }
    } 
    else
    {
        UE_LOG(LogTemp, Error, TEXT("LevelManager::StartWave - Cannot find row: %s"), *RowNameString);
        return;
    }
}

void ULevelManager::StartLevel()
{

    if (GameInstance)
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
            CurrentWaveIndex = SpartaGameInstance->CurrentWaveIndex;

            StartWave(CurrentWaveIndex);
        }
    }

}

bool ULevelManager::EndLevel()
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
    }

    if (GameInstance)
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            // 레벨 인덱스 증가
            CurrentLevelIndex++;
            SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;

            // 웨이브 인덱스 초기화
            CurrentWaveIndex = 0;
            SpartaGameInstance->CurrentWaveIndex = CurrentWaveIndex;

            // 다음 레벨이 있는지 확인
            int32 MaxLevels = GetMaxLevels();
            if (CurrentLevelIndex < MaxLevels)
            {
                return true; // 다음 레벨 있음
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("LevelManager::EndLevel - Failed to cast GameInstance!"));
            return false;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("LevelManager::EndLevel - GameInstance is nullptr!"));
        return false;
    }
    
    return false; // 더 이상 레벨 없음
}

float ULevelManager::GetRemainingTime() const
{
    if (!GetWorld())
    {
        return 0.0f;
    }

    return GetWorld()->GetTimerManager().GetTimerRemaining(WaveTimerHandle);
}

bool ULevelManager::CompleteCurrentWave()
{
    TArray<AActor*> FoundManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnvironmentHazardManager::StaticClass(), FoundManagers);
    if (FoundManagers.Num() > 0)
    {
        AEnvironmentHazardManager* HazardManager = Cast<AEnvironmentHazardManager>(FoundManagers[0]);
        if (HazardManager)
        {
            HazardManager->OnWaveEnd();
        }
    }

    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(WaveTimerHandle);
    }

    if (!GameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("LevelManager::CompleteCurrentWave - GameInstance is nullptr!"));
        return false;
    }

    USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
    if (!SpartaGameInstance)
    {
        UE_LOG(LogTemp, Error, TEXT("LevelManager::CompleteCurrentWave - Failed to cast GameInstance!"));
        return false;
    }

    CurrentWaveIndex++;

    TArray<FLevelItemRow>* WavesPtr = LevelWavesMap.Find(CurrentLevelIndex + 1);
    if (!WavesPtr)
    {
        UE_LOG(LogTemp, Error, TEXT("LevelManager::CompleteCurrentWave - No waves found for level %d"), CurrentLevelIndex + 1);
        return false;
    }

    if (CurrentWaveIndex < WavesPtr->Num())
    {
        // 다음 웨이브 존재
        if (SpawnVolume)
        {
            SpawnVolume->ClearAllSpawnedItems();
        }

        StartWave(CurrentWaveIndex);
        SpartaGameInstance->CurrentWaveIndex = CurrentWaveIndex;
        return true;
    }

    // 마지막 웨이브 완료
    OnLevelComplete.Broadcast();
    return false;
}
