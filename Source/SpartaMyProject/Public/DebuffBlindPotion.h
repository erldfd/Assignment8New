#pragma once

#include "CoreMinimal.h"
#include "DebuffItem.h"

#include "DebuffBlindPotion.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAMYPROJECT_API ADebuffBlindPotion : public ADebuffItem
{
	GENERATED_BODY()

public:

	ADebuffBlindPotion();

	virtual void ActivateItem(AActor* Activator) override;
};
