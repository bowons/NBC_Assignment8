#include "EnvironmentHazardManager.h"
#include "EnvironmentHazard.h"
#include "SpawnVolume.h"
#include "SpartaCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

// 웨이브 별 환경 변화 매니저 - AI 구현 도움
AEnvironmentHazardManager::AEnvironmentHazardManager()
{
	PrimaryActorTick.bCanEverTick = true;

	SafeZoneCenter = FVector::ZeroVector;
	InitialSafeZoneRadius = 2000.0f;
	FinalSafeZoneRadius = 800.0f;
	CurrentSafeZoneRadius = InitialSafeZoneRadius;
	OutOfZoneDamage = 10.0f;
	DamageTick = 0.5f;
	ShrinkStartDelay = 5.0f;
	ShrinkDuration = 30.0f;
	bSafeZoneActive = false;
	bSafeZoneShrinkStarted = false;
	SafeZoneElapsedTime = 0.0f;
	FireRingParticleCount = 24;
	FireRingUpdateInterval = 1.0f;
	LastUpdatedRadius = 0.0f;

	ExplosionInterval = 7.0f;
	ExplosionIntervalRandomRange = 2.0f;
	ExplosionWarningTime = 3.0f;
	ExplosionRadius = 300.0f;
	ExplosionDamage = 50.0f;
	SimultaneousExplosions = 2;
	bExplosionSystemActive = false;

	CurrentWaveIndex = 0;
}

void AEnvironmentHazardManager::BeginPlay()
{
	Super::BeginPlay();

    GetWorld()->GetTimerManager().ClearTimer(DamageTickTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(FireRingUpdateTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(ExplosionSpawnTimerHandle);
    bSafeZoneActive = false;
    bExplosionSystemActive = false;
    ClearFireParticles();
    CleanupExplosionSystem();

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	if (FoundVolumes.Num() > 0)
	{
		SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
		if (SpawnVolume)
		{
			SafeZoneCenter = SpawnVolume->GetActorLocation();
			UE_LOG(LogTemp, Log, TEXT("EnvironmentHazardManager initialized - Safe Zone Center: %s"), *SafeZoneCenter.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("EnvironmentHazardManager: Failed to cast SpawnVolume"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EnvironmentHazardManager: SpawnVolume not found in level!"));
	}
}

void AEnvironmentHazardManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSafeZoneActive)
	{
		UpdateSafeZoneShrink(DeltaTime);
	}
}

void AEnvironmentHazardManager::OnWaveStart(int32 WaveIndex)
{
	CurrentWaveIndex = WaveIndex;

	UE_LOG(LogTemp, Warning, TEXT("EnvironmentHazardManager::OnWaveStart - Wave %d"), WaveIndex + 1);

	if (WaveIndex == 1)
	{
		InitializeSafeZone();
		ShowNotification(TEXT("🔥 불길이 번지고 있습니다! 안전 지대로 이동하세요!"));
	}

	if (WaveIndex == 2)
	{
        InitializeSafeZone();
		InitializeExplosionSystem();
		ShowNotification(TEXT("⚠️ 무작위 폭발이 발생합니다! 경고 지역을 피하세요!"));
	}
}

void AEnvironmentHazardManager::OnWaveEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("EnvironmentHazardManager::OnWaveEnd"));

	if (bSafeZoneActive)
	{
		bSafeZoneActive = false;
		bSafeZoneShrinkStarted = false;
		SafeZoneElapsedTime = 0.0f;

		GetWorld()->GetTimerManager().ClearTimer(DamageTickTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(FireRingUpdateTimerHandle);

		ClearFireParticles();
	}

	if (bExplosionSystemActive)
	{
		CleanupExplosionSystem();
	}
}

void AEnvironmentHazardManager::InitializeSafeZone()
{
    GetWorld()->GetTimerManager().ClearTimer(DamageTickTimerHandle);
    GetWorld()->GetTimerManager().ClearTimer(FireRingUpdateTimerHandle);
    ClearFireParticles();

	bSafeZoneActive = true;
	bSafeZoneShrinkStarted = false;
	SafeZoneElapsedTime = 0.0f;
	CurrentSafeZoneRadius = InitialSafeZoneRadius;
	LastUpdatedRadius = CurrentSafeZoneRadius;

	GetWorld()->GetTimerManager().SetTimer(
		DamageTickTimerHandle,
		this,
		&AEnvironmentHazardManager::ApplyOutofZoneDamage,
		DamageTick,
		true
	);

	UpdateFireRing();

	GetWorld()->GetTimerManager().SetTimer(
		FireRingUpdateTimerHandle,
		this,
		&AEnvironmentHazardManager::UpdateFireRing,
		FireRingUpdateInterval,
		true
	);

	UE_LOG(LogTemp, Warning, TEXT("Safe Zone Initialized - Center: %s, Radius: %.1f"),
		*SafeZoneCenter.ToString(), CurrentSafeZoneRadius);
}

void AEnvironmentHazardManager::UpdateSafeZoneShrink(float DeltaTime)
{
	if (!bSafeZoneShrinkStarted)
	{
		SafeZoneElapsedTime += DeltaTime;
		if (SafeZoneElapsedTime >= ShrinkStartDelay)
		{
			bSafeZoneShrinkStarted = true;
			SafeZoneElapsedTime = 0.0f;
			UE_LOG(LogTemp, Warning, TEXT("Safe Zone shrinking started"));
		}
		return;
	}

	SafeZoneElapsedTime += DeltaTime;
	float Progress = FMath::Clamp(SafeZoneElapsedTime / ShrinkDuration, 0.0f, 1.0f);
	CurrentSafeZoneRadius = FMath::Lerp(InitialSafeZoneRadius, FinalSafeZoneRadius, Progress);
}

void AEnvironmentHazardManager::ApplyOutofZoneDamage()
{
	if (!bSafeZoneActive) return;

	TArray<AActor*> FoundCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpartaCharacter::StaticClass(), FoundCharacters);

	for (AActor* Actor : FoundCharacters)
	{
		APawn* PlayerPawn = Cast<APawn>(Actor);
		if (PlayerPawn && !IsInSafeZone(PlayerPawn))
		{
			float DamageAmount = OutOfZoneDamage * DamageTick;

			UGameplayStatics::ApplyDamage(
				PlayerPawn,
				DamageAmount,
				nullptr,
				this,
				UDamageType::StaticClass()
			);

			UE_LOG(LogTemp, Log, TEXT("Player outside Safe Zone - Applied %.1f damage"), DamageAmount);
		}
	}
}

bool AEnvironmentHazardManager::IsInSafeZone(APawn* Player) const
{
	if (!Player) return true;

	FVector PlayerLocation = Player->GetActorLocation();
	float Distance = FVector::Dist2D(PlayerLocation, SafeZoneCenter);

	return Distance <= CurrentSafeZoneRadius;
}

void AEnvironmentHazardManager::InitializeExplosionSystem()
{
	bExplosionSystemActive = true;

	SpawnRandomExplosions();

	float NextInterval = ExplosionInterval + FMath::FRandRange(-ExplosionIntervalRandomRange, ExplosionIntervalRandomRange);
	GetWorld()->GetTimerManager().SetTimer(
		ExplosionSpawnTimerHandle,
		this,
		&AEnvironmentHazardManager::SpawnRandomExplosions,
		NextInterval,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("Explosion System Initialized - Next explosion in %.1f seconds"), NextInterval);
}

void AEnvironmentHazardManager::SpawnRandomExplosions()
{
	if (!bExplosionSystemActive) return;

	if (!HazardClass)
	{
		UE_LOG(LogTemp, Error, TEXT("HazardClass is not set!"));
		return;
	}

	ShowNotification(TEXT("⚠️ 무작위 지점 폭발 예정!"));

	for (int32 i = 0; i < SimultaneousExplosions; ++i)
	{
		FVector SpawnLocation = GetRandomLocationInSafeZone();

		AEnvironmentHazard* Hazard = GetWorld()->SpawnActor<AEnvironmentHazard>(
			HazardClass,
			SpawnLocation,
			FRotator::ZeroRotator
		);

		if (Hazard)
		{
			Hazard->StartWarning(SpawnLocation, ExplosionRadius, ExplosionDamage, ExplosionWarningTime);
			ActiveHazards.Add(Hazard);

			UE_LOG(LogTemp, Log, TEXT("Spawned explosion hazard #%d at %s"), i + 1, *SpawnLocation.ToString());
		}
	}

	float NextInterval = ExplosionInterval + FMath::FRandRange(-ExplosionIntervalRandomRange, ExplosionIntervalRandomRange);
	GetWorld()->GetTimerManager().SetTimer(
		ExplosionSpawnTimerHandle,
		this,
		&AEnvironmentHazardManager::SpawnRandomExplosions,
		NextInterval,
		false
	);

	UE_LOG(LogTemp, Log, TEXT("Next explosion in %.1f seconds"), NextInterval);
}

FVector AEnvironmentHazardManager::GetRandomLocationInSafeZone() const
{
	if (!SpawnVolume)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpawnVolume is null, using SafeZoneCenter"));
		return SafeZoneCenter;
	}

	FVector SpawnLocation = SpawnVolume->GetRandomPointInVolume();

	if (bSafeZoneActive)
	{
		float Distance = FVector::Dist2D(SpawnLocation, SafeZoneCenter);
		if (Distance > CurrentSafeZoneRadius * 0.9f)
		{
			FVector Direction = (SpawnLocation - SafeZoneCenter).GetSafeNormal2D();
			SpawnLocation = SafeZoneCenter + Direction * (CurrentSafeZoneRadius * 0.8f);
			SpawnLocation.Z = SpawnVolume->GetActorLocation().Z;
		}
	}

	return SpawnLocation;
}

void AEnvironmentHazardManager::CleanupExplosionSystem()
{
	bExplosionSystemActive = false;

	GetWorld()->GetTimerManager().ClearTimer(ExplosionSpawnTimerHandle);

	for (AEnvironmentHazard* Hazard : ActiveHazards)
	{
		if (IsValid(Hazard))
		{
			Hazard->Destroy();
		}
	}
	ActiveHazards.Empty();

	UE_LOG(LogTemp, Warning, TEXT("Explosion System Cleaned Up"));
}

void AEnvironmentHazardManager::UpdateFireRing() // 불의 고리가 다가오도록 연출 - AI GEN
{
	if (!SafeZoneBoundaryEffect) return;

	float RadiusChange = FMath::Abs(CurrentSafeZoneRadius - LastUpdatedRadius);
	if (RadiusChange < 50.0f && FireParticles.Num() > 0)
	{
		return;
	}

	ClearFireParticles();

	LastUpdatedRadius = CurrentSafeZoneRadius;

	const float AngleStep = (2.0f * PI) / FireRingParticleCount;
	for (int32 i = 0; i < FireRingParticleCount; ++i)
	{
		float Angle = AngleStep * i;
		FVector Offset(
			FMath::Cos(Angle) * CurrentSafeZoneRadius,
			FMath::Sin(Angle) * CurrentSafeZoneRadius,
			0.0f
		);
		FVector FireLocation = SafeZoneCenter + Offset;

		UParticleSystemComponent* FireParticle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			SafeZoneBoundaryEffect,
			FireLocation,
			FRotator::ZeroRotator,
			FVector(1.0f),
			true,
			EPSCPoolMethod::None,
			true
		);

		if (FireParticle)
		{
			FireParticles.Add(FireParticle);
		}
	}
}

void AEnvironmentHazardManager::ClearFireParticles()
{
	for (UParticleSystemComponent* Fire : FireParticles)
	{
		if (IsValid(Fire))
		{
			Fire->DestroyComponent();
		}
	}
	FireParticles.Empty();
}

void AEnvironmentHazardManager::ShowNotification(const FString& Message)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, Message);
	}

	UE_LOG(LogTemp, Warning, TEXT("NOTIFICATION: %s"), *Message);
}
