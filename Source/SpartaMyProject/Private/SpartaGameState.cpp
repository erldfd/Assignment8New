#include "SpartaGameState.h"

#include "SpawnVolume.h"
#include "CoinItem.h"
#include "SpartaGameInstance.h"
#include "SpartaPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "Runtime/AdvancedWidgets/Public/Components/RadialSlider.h"

ASpartaGameState::ASpartaGameState()
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	WaveDuration = 30.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
}

void ASpartaGameState::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();

	GetWorldTimerManager().SetTimer(
		HUDUpdateTimerHandle,
		this,
		&ASpartaGameState::UpdateHUD,
		0.02f,
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

void ASpartaGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
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

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	StartWave();
}

void ASpartaGameState::OnLevelTimeUp()
{
	EndLevel();
}

void ASpartaGameState::OnCoinCollected()
{
	CollectedCoinCount++;

	if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
	{
		EndLevel();
	}
}

void ASpartaGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);
	GetWorldTimerManager().ClearTimer(HUDUpdateTimerHandle);
	GetWorldTimerManager().ClearTimer(BombSpawnTimerHandle);

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

	if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
	}
	else
	{
		OnGameOver();
	}
}

void ASpartaGameState::OnGameOver()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			SpartaPlayerController->SetPause(true);
			SpartaPlayerController->ShowMainMenu(true);
		}
	}
}

void ASpartaGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = SpartaPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					URadialSlider* TimeRadialSlider = Cast<URadialSlider>(HUDWidget->GetWidgetFromName(TEXT("TimeRadialSlider")));

					if (TimeRadialSlider == nullptr)
					{
						UE_LOG(LogTemp, Error, TEXT("TimeRadialSlider is nullptr"));
						return;
					}

					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(WaveTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time : %.1f"), RemainingTime)));

					float Ratio = (1 - RemainingTime / WaveDuration) * 360.0f;
					TimeRadialSlider->SetSliderHandleStartAngle(Ratio);
				}

				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(GameInstance);
						if (SpartaGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score : %d"), SpartaGameInstance->TotalScore)));
						}
					}
				}

				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level : %d"), CurrentLevelIndex + 1)));
				}

				if (UTextBlock* WaveNumberText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveNumberText->SetText(FText::FromString(FString::Printf(TEXT("Wave : %d"), CurrentWaveNumber)));
				}
			}
		}
	}
}

void ASpartaGameState::StartWave()
{
	ASpartaPlayerController* SpartaPlayerController = Cast<ASpartaPlayerController>(GetWorld()->GetFirstPlayerController());
	if (SpartaPlayerController == nullptr)
	{
		return;
	}


	CurrentWaveNumber++;

	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 40 * CurrentWaveNumber;

	for (int32 i = 0; i < ItemToSpawn; ++i)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}
			}
		}
	}

	const int32 SpikeSpawnCount = 10 * (CurrentWaveNumber - 1) * (CurrentLevelIndex + 1);
	float Height = -100.0f;

	if (CurrentWaveNumber == 2)
	{
		SpartaPlayerController->ActivateTrapText(TEXT("Spike Trap Is Activated!"));
	}

	for (int32 i = 0; i < SpikeSpawnCount; ++i)
	{
		if (FoundVolumes.Num() <= 0)
		{
			break;
		}

		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
		if (SpawnVolume)
		{
			SpawnVolume->SpawnSpikeAtRandomPosition(Height);
		}
	}

	const int32 StarSpawnCount = 1;
	for (int32 i = 0; i < StarSpawnCount; ++i)
	{
		if (FoundVolumes.Num() <= 0)
		{
			break;
		}

		ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
		if (SpawnVolume)
		{
			SpawnVolume->SpawnSpecialStarAtRandomPosition();
		}
	}

	if (CurrentWaveNumber < MaxWaveNumber)
	{
		GetWorldTimerManager().SetTimer(
			WaveTimerHandle,
			this,
			&ASpartaGameState::StartWave,
			WaveDuration,
			false
		);
	}
	else
	{
		GetWorldTimerManager().SetTimer(
			WaveTimerHandle,
			this,
			&ASpartaGameState::OnLevelTimeUp,
			WaveDuration,
			false
		);

		SpartaPlayerController->ActivateTrapText(TEXT("Bomb Trap Is Activated!"));

		GetWorldTimerManager().SetTimer(
			BombSpawnTimerHandle,
			[this]()
			{
				if (::IsValid(this) == false)
				{
					return;
				}

				UWorld* World = GetWorld();

				if (::IsValid(World) == false)
				{
					return;
				}

				TArray<AActor*> Volumes;
				UGameplayStatics::GetAllActorsOfClass(World, ASpawnVolume::StaticClass(), Volumes);

				if (Volumes.Num() <= 0)
				{
					return;
				}

				ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(Volumes[0]);
				if (SpawnVolume)
				{
					SpawnVolume->SpawnBombAtRandomPosition();
				}
			},
			BombSpawnInterval,
			true
			);
	}

	UE_LOG(LogTemp, Warning, TEXT("Start Wave %d, Spawned %d coin"), CurrentWaveNumber, SpawnedCoinCount);
}
