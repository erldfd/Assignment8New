#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"

#include "DebuffItem.generated.h"

/**
 * 
 */
UCLASS()
class SPARTAMYPROJECT_API ADebuffItem : public ABaseItem
{
	GENERATED_BODY()
	
public:

	ADebuffItem();

	virtual void ActivateItem(AActor* Activator) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 DebuffTime;
};
