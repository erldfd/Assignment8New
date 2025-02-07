#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Bomb.generated.h"

UCLASS()
class SPARTAMYPROJECT_API ABomb : public AActor
{
	GENERATED_BODY()
	
public:	

	ABomb();

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	UParticleSystem* ExplosionParticle = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	USoundBase* ExplosionSound = nullptr;

	UPROPERTY(EditAnywhere, Category = "Bomb")
	int Damage = 10;

};
