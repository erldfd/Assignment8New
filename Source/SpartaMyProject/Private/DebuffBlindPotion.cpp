#include "DebuffBlindPotion.h"

#include "SpartaCharacter.h"

ADebuffBlindPotion::ADebuffBlindPotion()
{
	ItemType = "BlindPotion";
}

void ADebuffBlindPotion::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			PlayerCharacter->ActivateBlindDebuff(DebuffTime);
		}

		DestroyItem();
	}
}
