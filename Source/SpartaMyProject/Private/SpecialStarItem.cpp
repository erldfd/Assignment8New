#include "SpecialStarItem.h"

#include "SpartaCharacter.h"

ASpecialStarItem::ASpecialStarItem()
{

}

void ASpecialStarItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (ASpartaCharacter* PlayerCharacter = Cast<ASpartaCharacter>(Activator))
		{
			PlayerCharacter->ActivateSpecialBuff(BuffTime);
		}

		DestroyItem();
	}
}

void ASpecialStarItem::OnItemOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 아무 것도 하지 않음
}

void ASpecialStarItem::OnItemEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 아무 것도 하지 않음
}

void ASpecialStarItem::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AddActorLocalRotation(FRotator(0, DeltaSeconds * 100, 0), false);
}
