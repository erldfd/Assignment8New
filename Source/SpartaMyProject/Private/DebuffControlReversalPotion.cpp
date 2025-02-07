#include "DebuffControlReversalPotion.h"

#include "SpartaCharacter.h"

ADebuffControlReversalPotion::ADebuffControlReversalPotion()
{
}

void ADebuffControlReversalPotion::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			PlayerCharacter->ActivateControlReversalDebuff(DebuffTime);
		}

		DestroyItem();
	}

}
