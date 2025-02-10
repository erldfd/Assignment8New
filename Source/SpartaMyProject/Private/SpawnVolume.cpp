#include "SpawnVolume.h"

#include "ItemSpawnRow.h"
#include "Spike.h"
#include "Bomb.h"
#include "SpecialStarItem.h"

#include "Components/BoxComponent.h"

ASpawnVolume::ASpawnVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);

	ItemDataTable = nullptr;
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

FItemSpawnRow* ASpawnVolume::GetRandomItem() const
{
	if (!ItemDataTable) return nullptr;

	TArray<FItemSpawnRow*> AllRows;
	static const FString ContextString(TEXT("ItemSpawnContext"));

	ItemDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty()) return nullptr;

	float TotalChance = 0.0f;
	for (const FItemSpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}

	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;

	for (FItemSpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}

	return nullptr;

	return nullptr;
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	// extent : 중심에서 끝까지의 거리.
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z, BoxExtent.Z)
		);
}

ASpike* ASpawnVolume::SpawnSpike(const FVector& Position)
{
	if (SpikeClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ASpawnVolume::SpawnSpike, SpikeClass == nullptr"));
		return nullptr;
	}

	ASpike* NewSpike = GetWorld()->SpawnActor<ASpike>(SpikeClass, Position, FRotator::ZeroRotator);

	return NewSpike;
}

ASpike* ASpawnVolume::SpawnSpikeAtRandomPosition(float Height)
{
	FVector RandomPosition = GetRandomPointInVolume();
	RandomPosition.Z = Height;

	return SpawnSpike(RandomPosition);
}

ABomb* ASpawnVolume::SpawnBomb(const FVector& Position)
{
	if (BombClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ASpawnVolume::SpawnBomb, BombClass == nullptr"));
		return nullptr;
	}

	return GetWorld()->SpawnActor<ABomb>(BombClass, Position, FRotator::ZeroRotator);
}

ABomb* ASpawnVolume::SpawnBombAtRandomPosition()
{
	return SpawnBomb(GetRandomPointInVolume());
}

ASpecialStarItem* ASpawnVolume::SpawnSpecialStar(const FVector& Position)
{
	if (SpecialStarClass == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("ASpawnVolume::SpawnSpecialStar, SpecialStarClass == nullptr"));
		return nullptr;
	}

	return GetWorld()->SpawnActor<ASpecialStarItem>(SpecialStarClass, Position, FRotator::ZeroRotator);;
}

ASpecialStarItem* ASpawnVolume::SpawnSpecialStarAtRandomPosition()
{
	return SpawnSpecialStar(GetRandomPointInVolume());
}

AActor* ASpawnVolume::SpawnItem(TSubclassOf<AActor> ItemClass)
{
	if (!ItemClass) return nullptr;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ItemClass,
		GetRandomPointInVolume(),
		FRotator::ZeroRotator
	);

	return SpawnedActor;
}

