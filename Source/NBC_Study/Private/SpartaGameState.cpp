// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"

ASpartaGameState::ASpartaGameState()
{
    Score = 0;
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;
    LevelDuration = 30.0f; // 한 레벨당 30초
    CurrentLevelIndex = 0;
    MaxLevels = 3;
}

void ASpartaGameState::BeginPlay()
{
    Super::BeginPlay();

    UpdateHUD();
    StartLevel();

    GetWorldTimerManager().SetTimer(
        HUDUpdateTimerHandle,
        this,
        &ASpartaGameState::UpdateHUD,
        0.1f,
        true
    );
}

int32 ASpartaGameState::GetScore() const
{
    return Score;
}

void ASpartaGameState::AddScore(int32 Amount)
{
    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            SpartaGameInstance->AddToScore(Amount);
        }
    }
}

void ASpartaGameState::OnGameOver()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
        {
            SpartaPlayerController->SetPause(true); // 로컬 진행 중 게임 정지 시도
            SpartaPlayerController->ShowMainMenu(true);
        }
    }
}

void ASpartaGameState::StartLevel()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
        if (SpartaPlayerController)
        {
            SpartaPlayerController->ShowGameHUD();
        }
    }   

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            CurrentLevelIndex = SpartaGameInstance->CurrentLevelIndex;
        }
    }

    // 레벨 시작 시, 코인 개수 초기화
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;

    // 현재 맵에 배치된 모든 SpawnVolume을 찾아 아이템 40개를 스폰
    TArray<AActor*> FoundVolumes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

    const int32 ItemsToSpawn = 40;

    for (int32 i = 0; i < ItemsToSpawn; ++i)
    {
        if (FoundVolumes.Num() > 0)
        {
            ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
            if (SpawnVolume)
            {
                AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
                // 만약 스폰된 엑터가 코인 타입이라면 SpawnedCoinCount 증가
                if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
                {
                    SpawnedCoinCount++;
                }
            }
        }
    }

    // 30초 후에 OnLevelTimeUp() 가 호출되도록 타이머 설정
    GetWorldTimerManager().SetTimer(
        LevelTimerHandle,
        this,
        &ASpartaGameState::OnLevelTimeUp,
        LevelDuration,
        false
    );

    UpdateHUD();

    UE_LOG(LogTemp, Warning, TEXT("Level %d Start!, Spawned %d coin"),
        CurrentLevelIndex + 1,
        SpawnedCoinCount
    );

}

void ASpartaGameState::OnLevelTimeUp()
{
    // 시간이 다 되면, 레벨을 종료
    EndLevel();
}

void ASpartaGameState::OnCoinCollected()
{
    CollectedCoinCount++;

    UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
        CollectedCoinCount,
        SpawnedCoinCount
    );

    // 현재 레벨에서 스폰된 코인을 전부 주웠다면 즉시 레벨 종료
    if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
    {
        EndLevel();
    }
}

void ASpartaGameState::EndLevel()
{
    // 타이머 해제
    GetWorldTimerManager().ClearTimer(LevelTimerHandle);

    if (UGameInstance* GameInstance = GetGameInstance())
    {
        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
        if (SpartaGameInstance)
        {
            AddScore(Score);
            CurrentLevelIndex++;
            SpartaGameInstance->CurrentLevelIndex = CurrentLevelIndex;
        }
    }

    if (CurrentLevelIndex >= MaxLevels)
    {
        OnGameOver();
        return;
    }

    // 레벨 맵 이름이 있다면 해당 맵 불러오기
    if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
    {
        FName NextLevelName = LevelMapNames[CurrentLevelIndex];
        UGameplayStatics::OpenLevel(GetWorld(), NextLevelName);
    }
    else
    {
        OnGameOver();
    }

}

void ASpartaGameState::UpdateHUD()
{
    if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
    {
        ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController);
        if (SpartaPlayerController)
        {
            if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
            {
                if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
                {
                    float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
                    TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
                }

                if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
                {
                    if (UGameInstance* GameInstance = GetGameInstance())
                    {
                        USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
                        if (SpartaGameInstance)
                        {
                            ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), SpartaGameInstance->TotalScore)));
                        }
                    }
                }

                if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
                {
                    LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
                }
            }
        }
    }
}
