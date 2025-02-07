#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ItemInterface.generated.h"

UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPARTAMYPROJECT_API IItemInterface
{
	GENERATED_BODY()

public:
	// 인터페이스에서 UFUNCTION 붙여주면 구현해준 것들은 붙여줄 필요가 없다!
	UFUNCTION()
	virtual void OnItemOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) = 0;

	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) = 0;

	virtual void ActivateItem(AActor* Activator) = 0;
	virtual FName GetItemType() const = 0;
};
