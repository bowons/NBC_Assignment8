// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"

// "LogSparta" 카테고리 정의 (헤더에서 선언한 것을 실제로 구현)
DEFINE_LOG_CATEGORY(LogSparta);

// Sets default values
AItem::AItem()
{
    // Scene Component를 생성하고 루트로 설정
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    // Static Mesh Component를 생성하고 Scene Component에 Attach
    StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    StaticMeshComp->SetupAttachment(SceneRoot);

    // Static Mesh를 코드에서 설정
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("'/Game/Resources/Props/SM_Chair.SM_Chair'"));
    if (MeshAsset.Succeeded())
    {
        StaticMeshComp->SetStaticMesh(MeshAsset.Object);
    }

    // Material을 코드에서 설정
    static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("/Game/Resources/Materials/M_Metal_Gold.M_Metal_Gold"));
    if (MaterialAsset.Succeeded())
    {
        StaticMeshComp->SetMaterial(0, MaterialAsset.Object);
    }

    UE_LOG(LogSparta, Warning, TEXT("%s Constructor"), *GetName());
    
    // Tick 함수를 켜줍니다.
    PrimaryActorTick.bCanEverTick = true;
    // 기본 회전 속도 (1초에 90도 회전)
    RotationSpeed = 90.0f;
}

void AItem::BeginPlay()
{
    Super::BeginPlay();
    // 기존 LogTemp
    UE_LOG(LogTemp, Warning, TEXT("My Item appears!!"));
    // 새로 만든 LogSparta
    UE_LOG(LogSparta, Error, TEXT("My Log!!"));

    // 위치, 회전, 스케일 설정하기
    // (300, 200, 100) 위치로 이동
    SetActorLocation(FVector(300.f, 200.f, 100.f));
    // Yaw 방향으로 45도 회전
    SetActorRotation(FRotator(0.0f, 45.0f, 0.0f));
    // 모든 축을 2배로 스케일
    SetActorScale3D(FVector(2.0f, 2.0f, 2.0f));
}

void AItem::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    UE_LOG(LogSparta, Warning, TEXT("%s PostInitializeComponents"), *GetName());
}

void AItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Tick 함수는 매 프레임마다 호출되므로
    // 여기서 로그를 찍으면 방대한 양의 메시지가 쌓일 수 있음.
    // 필요시 디버깅용 코드만 간단히 작성하거나, 별도 조건을 걸어 사용.

    // RotationSpeed가 0이 아니라면 회전 처리
    if (!FMath::IsNearlyZero(RotationSpeed))
    {
        // 초당 RotationSpeed만큼, 한 프레임당 Yaw (RotationSpeed * DeltaTime)만큼 회전
        AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
    }
}

void AItem::Destroyed()
{
    UE_LOG(LogSparta, Warning, TEXT("%s Destroyed"), *GetName());

    Super::Destroyed();
}

void AItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UE_LOG(LogSparta, Warning, TEXT("%s EndPlay"), *GetName());

    Super::EndPlay(EndPlayReason);
}

void AItem::ResetActorPosition()
{
    SetActorLocation(FVector::ZeroVector);
}

float AItem::GetRotationSpeed() const
{
    return RotationSpeed;
}
