// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpartaPlayerController.generated.h"

class UInputMappingContext; // IMC 관련 전방 선언
class UInputAction; // IA 관련 전방 선언

UCLASS()
class NBC_STUDY_API ASpartaPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    ASpartaPlayerController();

    // 에디터에서 세팅할 IMC
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputMappingContext* InputMappingContext;
    // IA_Move를 지정할 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* MoveAction;
    // IA_Jump를 지정할 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* JumpAction;
    // IA_Look를 지정할 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* LookAction;
    // IA_Sprint를 지정할 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
    UInputAction* SprintAction;

    // UMG 위젯 클래스를 에디터에서 할당받을 변수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
    TSubclassOf<UUserWidget> HUDWidgetClass;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HUD")
    UUserWidget* HUDWidgetInstance;

    UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Menu")
    TSubclassOf<UUserWidget> MainMenuWidgetClass;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu")
    UUserWidget* MainMenuWidgetInstance;

    UFUNCTION(BlueprintCallable, Category = "HUD")
    void ShowGameHUD();

    UFUNCTION(BlueprintPure, Category = "HUD")
    UUserWidget* GetHUDWidget() const;

    UFUNCTION(BlueprintCallable, Category = "Menu")
    void ShowMainMenu(bool bIsRestart);

    UFUNCTION(BlueprintCallable, Category = "Menu")
    void StartGame();

protected:
    virtual void BeginPlay() override;
};
