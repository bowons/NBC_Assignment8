// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnRow.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class NBC_STUDY_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
    // Sets default values for this actor's properties
    ASpawnVolume();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
    USceneComponent* Scene;

    // 스폰영역을 담당할 박스 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
    UBoxComponent* SpawningBox;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
    UDataTable* ItemDataTable;

#pragma region Spawning
    // 스폰 볼륨 내에서 무작위 좌표를 얻어 오는 함수
    UFUNCTION(BlueprintCallable, Category = "Spawning")
    FVector GetRandomPointInVolume() const;

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    AActor* SpawnRandomItem();

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    AActor* SpawnItem(TSubclassOf<AActor> ItemClass);

    FItemSpawnRow* GetRandomItem() const;

    UFUNCTION(BlueprintCallable, Category = "Spawning")
    void ClearAllSpawnedItems();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
    TArray<AActor*> SpawnedItems;

#pragma endregion Spawning
};
