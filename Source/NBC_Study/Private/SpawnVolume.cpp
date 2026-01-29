// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    // 박스 컴포넌트를 생성하고, 이 엑터의 루트로 설정
    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
    SetRootComponent(Scene);

    SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
    SpawningBox->SetupAttachment(Scene);
}

AActor* ASpawnVolume::SpawnRandomItem()
{
    if (FItemSpawnRow* SelectedRow = GetRandomItem())
    {
        if (UClass* ActualClass = SelectedRow->ItemClass.Get())
        {
            return SpawnItem(ActualClass);
        }
    }

    return nullptr;
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
    // 1) 박스 컴포넌트의 스케일된 Extent, 즉 x/y/z 방향으로 반지름 (절반 길이)을 구함
    // (200, 100, 50) (2, 1, 1) 스케일이라면 (400, 200, 100) 이 됨
    // 중심 -> 끝까지의 거리, 박스 컴포넌트의 실제 가로/세로/높이 절반 길이(Extent)를 반환
    FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
    // 2) 박스 중심 위치
    FVector BoxOrigin = SpawningBox->GetComponentLocation();
    // 3) 각 축별로 -Extent ~ +Extent 범위의 무작위 값 생성
    return BoxOrigin + FVector(
        FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
        FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
        FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
    );
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
    if (!ItemClass) return nullptr;

    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
        ItemClass,
        GetRandomPointInVolume(),
        FRotator::ZeroRotator
    );

    if (SpawnedActor)
    {
        SpawnedItems.Add(SpawnedActor);
    }

    return SpawnedActor;
}

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{ 
    if (!ItemDataTable) return nullptr;

    // 1 - 모든 행 가져오기
    TArray<FItemSpawnRow*> AllRows;
    static const FString ContextString(TEXT("ItemSpawnContext"));
    ItemDataTable->GetAllRows(ContextString, AllRows);

    if (AllRows.IsEmpty()) return nullptr;

    // 2 - 전체 확률 합 구하기 
    float TotalChance = 0.0f;
    for (const FItemSpawnRow* Row : AllRows) // 각 배열 row 순회
    {
        if (Row)
        {
            TotalChance += Row->SpawnChance;
        }
    }

    // 전체 확률과 0 사이 랜덤 값
    const float RandValue = FMath::FRandRange(0.0f, TotalChance);
    float AccumulateChance = 0.0f;

    // 3 - 누적 확률로 아이템 선택
    for (FItemSpawnRow* Row : AllRows)
    {
        AccumulateChance += Row->SpawnChance;
        if (RandValue <= AccumulateChance)
        {
            return Row;
        }
    }

    return nullptr;
}

void ASpawnVolume::ClearAllSpawnedItems()
{
    for (int32 i = SpawnedItems.Num() - 1; i >= 0; --i)
    {
        if (AActor* SpawnedItem = SpawnedItems[i])
        {
            if (IsValid(SpawnedItem))
            {
                SpawnedItem->Destroy();
            }
        }
    }

    SpawnedItems.Empty();
}
