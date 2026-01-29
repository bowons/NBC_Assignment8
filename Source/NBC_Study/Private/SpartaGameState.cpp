// Fill out your copyright notice in the Description page of Project Settings.


#include "SpartaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "LevelManager.h"

ASpartaGameState::ASpartaGameState()
{
    Score = 0;
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;
    CurrentLevelIndex = 0;
    MaxLevels = 0;
}

void ASpartaGameState::BeginPlay()
{
    Super::BeginPlay();

    if (LevelItemDataTable)
    {
        LevelManager = NewObject<ULevelManager>(this);
        if (!LevelManager) 
        {
            UE_LOG(LogTemp, Error, TEXT("SpartaGameState::BeginPlay - Failed to create LevelManager!"));
            return;
        }

        TArray<AActor*> FoundVolumes;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
        
        ASpawnVolume* SpawnVolume = FoundVolumes.Num() > 0 ? Cast<ASpawnVolume>(FoundVolumes[0]) : nullptr;
        if (!SpawnVolume)
        {
            UE_LOG(LogTemp, Warning, TEXT("SpartaGameState::BeginPlay - No SpawnVolume found in level!"));
        }

        LevelManager->Initialize(LevelItemDataTable, SpawnVolume, GetGameInstance());

        LevelManager->OnLevelComplete.AddDynamic(this, &ASpartaGameState::HandleLevelComplete);
        LevelManager->OnItemSpawned.AddDynamic(this, &ASpartaGameState::HandleItemSpawned);

        StartLevel();

        GetWorldTimerManager().SetTimer(
            HUDUpdateTimerHandle,
            this,
            &ASpartaGameState::UpdateHUD,
            0.1f,
            true
        );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("SpartaGameState::BeginPlay - LevelItemDataTable is not assigned!"));
    }

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

void ASpartaGameState::HandleLevelComplete()
{
    bool bHasNextLevel = false;

    if (LevelManager)
    {
        bHasNextLevel = LevelManager->EndLevel();
    }

    AddScore(Score);

    UpdateHUD();

    if (!bHasNextLevel)
    {
        // 게임 오버
        OnGameOver();
        return;
    }

    CurrentLevelIndex = LevelManager->GetCurrentLevelIndex();
    if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
    {
        FName NextLevelName = LevelMapNames[CurrentLevelIndex];
        UE_LOG(LogTemp, Warning, TEXT("Loading next level: %s"), *NextLevelName.ToString());
        UGameplayStatics::OpenLevel(GetWorld(), NextLevelName);
    }
    else
    {
        OnGameOver();
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


    // 레벨 시작 시, 코인 개수 초기화
    SpawnedCoinCount = 0;
    CollectedCoinCount = 0;

    if (LevelManager)
    {
        LevelManager->StartLevel();
        // LevelManager가 GameInstance에서 로드한 인덱스를 가져옴
        CurrentLevelIndex = LevelManager->GetCurrentLevelIndex();
    }

    UpdateHUD();

    UE_LOG(LogTemp, Warning, TEXT("Level %d Start!, Spawned %d coin"),
        CurrentLevelIndex + 1,
        SpawnedCoinCount
    );

}

void ASpartaGameState::HandleItemSpawned(AActor* SpawnedItem)
{
    if (SpawnedItem)
    {
        if (SpawnedItem->GetClass()->IsChildOf(ACoinItem::StaticClass()))
        {
            SpawnedCoinCount++;
        }
    }
    else 
    {
        UE_LOG(LogTemp, Warning, TEXT("SpartaGameState::HandleItemSpawned - SpawnedItem is null!"));
    }
}

void ASpartaGameState::OnCoinCollected()
{
    CollectedCoinCount++;

    UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"),
        CollectedCoinCount,
        SpawnedCoinCount
    );

    if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
    {
        SpawnedCoinCount = 0;
        CollectedCoinCount = 0;

        if (LevelManager)
        {
            bool bHasNextWave = LevelManager->CompleteCurrentWave();

            if (!bHasNextWave)
            {
                UE_LOG(LogTemp, Warning, TEXT("Level completed by collecting all coins!"));
            }
        }
        //HandleLevelComplete();
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
                    float RemainingTime = LevelManager ? LevelManager->GetRemainingTime() : 0.0f;
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
