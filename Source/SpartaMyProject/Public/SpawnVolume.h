// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "SpawnVolume.generated.h"

class ASpike;
class ABomb;
class ASpecialStarItem;

UCLASS()
class SPARTAMYPROJECT_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	

	ASpawnVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	class UBoxComponent* SpawningBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	UDataTable* ItemDataTable;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AActor* SpawnRandomItem();

	struct FItemSpawnRow* GetRandomItem() const;
	AActor* SpawnItem(TSubclassOf<AActor> ItemClass);
	FVector GetRandomPointInVolume() const;

	ASpike* SpawnSpike(const FVector& Position);
	
	ASpike* SpawnSpikeAtRandomPosition(float Height);

	ABomb* SpawnBomb(const FVector& Position);
	ABomb* SpawnBombAtRandomPosition();

	ASpecialStarItem* SpawnSpecialStar(const FVector& Position);
	ASpecialStarItem* SpawnSpecialStarAtRandomPosition();

protected:

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<ASpike> SpikeClass;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<ABomb> BombClass;

	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<ASpecialStarItem> SpecialStarClass;

};
