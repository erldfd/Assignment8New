#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"

#include "SpecialStarItem.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAMYPROJECT_API ASpecialStarItem : public ABaseItem
{
	GENERATED_BODY()

public:

	ASpecialStarItem();

	virtual void ActivateItem(AActor* Activator) override;

	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;
protected:

	virtual void Tick(float DeltaSeconds) override;

protected:
	
	UPROPERTY(EditAnywhere, Category = "SpecialStar")
	float BuffTime = 10;
};
