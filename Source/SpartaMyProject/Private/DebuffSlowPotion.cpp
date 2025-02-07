#include "DebuffSlowPotion.h"

#include "SpartaCharacter.h"

ADebuffSlowPotion::ADebuffSlowPotion()
{
	ItemType = "SlowPotion";
}

void ADebuffSlowPotion::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			PlayerCharacter->ActivateSlowDebuff(DebuffTime);
		}

		DestroyItem();
	}
}
