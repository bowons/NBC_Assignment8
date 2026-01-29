// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SpartaCharacter.generated.h"

// 전방선언
class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
class UDebuffManagerComponent;

// Enhanced Input에서 액션 값 받을 때 사용하는 구조체
struct FInputActionValue;

UCLASS()
class NBC_STUDY_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpartaCharacter();

public:	
    // 스프링 암 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    USpringArmComponent* SpringArmComp;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
    UCameraComponent* CameraComp;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
    UWidgetComponent* OverheadWidget;

    // 현재 체력을 가져오는 함수
    UFUNCTION(BlueprintPure, Category = "Health")
    int32 GetHealth() const;
    // 체력을 회복시키는 함수
    UFUNCTION(BlueprintCallable, Category = "Health")
    void AddHealth(float Amount);

protected:
    virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // IA_Move와 IA_Jump 등을 처리할 함수 원형
    // EnhancedInput에서 액션 값은 FInputActionValue로 전달됨
    UFUNCTION()
    void Move(const FInputActionValue& value);
    UFUNCTION()
    void StartJump(const FInputActionValue& value);
    UFUNCTION()
    void StopJump(const FInputActionValue& value);
    UFUNCTION()
    void Look(const FInputActionValue& value);
    UFUNCTION()
    void StartSprint(const FInputActionValue& value);
    UFUNCTION()
    void StopSprint(const FInputActionValue& value);

protected:
    // 최대 체력
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    float MaxHealth;
    // 현재 체력
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    float Health;
    // 사망 처리 함수 (체력이 0 이하가 되었을 때 호출)
    UFUNCTION(BlueprintCallable, Category = "Health")
    virtual void OnDeath();

    void UpdateOverheadHP();

    // 데미지 처리 함수 - 외부로부터 데미지를 받을 때 호출
    // AActor의 TakeDamage 함수를 오버라이드
    virtual float TakeDamage(
        float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

private:
    float NormalSpeed;
    float SprintSpeedMultiplier;
    float SprintSpeed;
    bool bIsSprinting;

#pragma region Debuff
    bool bIsInputReversed = false;
public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debuff")
    UDebuffManagerComponent* DebuffManager;

    UFUNCTION(BlueprintCallable, Category = "Debuff")
    void SetInputReversed(bool bReversed) { bIsInputReversed = bReversed; }

    UFUNCTION(BlueprintPure, Category = "Debuff")
    bool IsInputReversed() const { return bIsInputReversed; }

    UFUNCTION(BlueprintPure, Category = "Movement")
    bool IsSprinting() const { return bIsSprinting; }

    UFUNCTION(BlueprintPure, Category = "Movement")
    float GetNormalSpeed() const { return NormalSpeed; }
#pragma endregion Debuff
};
