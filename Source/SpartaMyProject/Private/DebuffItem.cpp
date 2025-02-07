#include "DebuffItem.h"

ADebuffItem::ADebuffItem()
{
	DebuffTime = 5.0f;
	ItemType = "DebuffItem";
}

void ADebuffItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);
}
