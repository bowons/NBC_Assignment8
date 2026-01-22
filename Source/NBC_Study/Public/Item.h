// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

// 로그스파르타 로그 카테고리 선언
DECLARE_LOG_CATEGORY_EXTERN(LogSparta, Warning, All);

UCLASS()
class NBC_STUDY_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
    // Root Scene Component, 에디터에서 볼 수만 있고 수정 불가
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Components")
    USceneComponent* SceneRoot;
    // Static Mesh, 에디터와 Blueprint에서 수정 가능
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Components")
    UStaticMeshComponent* StaticMeshComp;

    // 회전 속도, 클래스 기본값만 수정 가능
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Properties")
    float RotationSpeed;

protected:
    // BeginPlay 함수를 다시 선언
    virtual void BeginPlay() override;

    // 라이프 사이클 함수들
    virtual void PostInitializeComponents() override;
    virtual void Tick(float DeltaTime) override;
    virtual void Destroyed() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;    

protected:
    // 함수를 블루프린트에서 호출 가능하도록 설정
    UFUNCTION(BlueprintCallable, Category = "Item|Actions")
    void ResetActorPosition();

    // 블루프린트에서 값만 반환하도록 설정 (Getter)
    UFUNCTION(BlueprintPure, Category = "Item|Properties")
    float GetRotationSpeed() const;

    // C++에서 호출되지만 구현은 블루프린트에서 수행
    UFUNCTION(BlueprintImplementableEvent, Category = "Item|Event")
    void OnItemPickedUp();
};
