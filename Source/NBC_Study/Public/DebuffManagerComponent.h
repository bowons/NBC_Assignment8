// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DebuffManagerComponent.generated.h"

class UDebuffEffect;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDebuffChanged, UDebuffEffect*, Debuff);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NBC_STUDY_API UDebuffManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDebuffManagerComponent();

    UFUNCTION(BlueprintCallable, Category = "Debuff")
    void ApplyDebuff(TSubclassOf<UDebuffEffect> DebuffClass, int32 Stacks = 1);
    UFUNCTION(BlueprintCallable, Category = "Debuff")
    void RemoveDebuff(FName DebuffID);
    UFUNCTION(BlueprintCallable, Category = "Debuff")
    bool HasDebuff(FName DebuffID) const;
    
    UFUNCTION(BlueprintCallable, Category = "Debuff")
    UDebuffEffect* GetDebuff(FName DebuffID) const;
    UFUNCTION(BlueprintCallable, Category = "Debuff")
    TArray<UDebuffEffect*> GetActiveDebuffs() const;

    UPROPERTY(BlueprintAssignable, Category = "Debuff|Events")
    FOnDebuffChanged OnDebuffApplied;

    UPROPERTY(BlueprintAssignable, Category = "Debuff|Events")
    FOnDebuffChanged OnDebuffRemoved;

    UPROPERTY(BlueprintAssignable, Category = "Debuff|Events")
    FOnDebuffChanged OnDebuffUpdated;

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
    UPROPERTY()
    TMap<FName, UDebuffEffect*> ActiveDebuffs;

    void UpdateDebuff(UDebuffEffect* Debuff, float DeltaTime);
    void CleanupExpiredDebuffs();
};
