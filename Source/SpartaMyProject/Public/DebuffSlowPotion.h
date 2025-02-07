#pragma once

#include "CoreMinimal.h"
#include "DebuffItem.h"
#include "DebuffSlowPotion.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAMYPROJECT_API ADebuffSlowPotion : public ADebuffItem
{
	GENERATED_BODY()
	
public:

	ADebuffSlowPotion();

	virtual void ActivateItem(AActor* Activator) override;
};
