#pragma once

#include "CoreMinimal.h"
#include "DebuffItem.h"

#include "DebuffControlReversalPotion.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAMYPROJECT_API ADebuffControlReversalPotion : public ADebuffItem
{
	GENERATED_BODY()
	
public:

	ADebuffControlReversalPotion();

	virtual void ActivateItem(AActor* Activator) override;
};
