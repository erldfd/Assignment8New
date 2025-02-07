#include "Spike.h"

ASpike::ASpike()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(Root);
}

void ASpike::BeginPlay()
{
	Super::BeginPlay();

	if (bShouldUseRelativePositon)
	{
		Destination1 = GetActorLocation();
		Destination2 = Destination1 + RelativeDestination;
	}
	
}

void ASpike::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RemainingDelay > 0)
	{
		RemainingDelay -= DeltaTime;
		return;
	}

	FVector Direction;

	if (bIsGoingToDestination1)
	{
		Direction = Destination1 - Destination2;
		Direction.Normalize();

		FVector DeltaDistance = Direction * Speed2 * DeltaTime;

		AddActorLocalOffset(DeltaDistance);
		/*if (FMath::IsNearlyZero(((GetActorLocation() - Destination1).Size(), DeltaDistance.Size())))
		{
			SetActorLocation(Destination1);
			RemainingDelay = Delay2;
			bIsGoingToDestination1 = (!bIsGoingToDestination1);
		}*/

		if ((GetActorLocation() - Destination1).Size() <= DeltaDistance.Size())
		{
			SetActorLocation(Destination1);
			RemainingDelay = Delay2;
			bIsGoingToDestination1 = (!bIsGoingToDestination1);
		}
	}
	else
	{
		Direction = Destination2 - Destination1;
		Direction.Normalize();

		FVector DeltaDistance = Direction * Speed1 * DeltaTime;

		AddActorLocalOffset(DeltaDistance);

		/*if (FMath::IsNearlyZero(((GetActorLocation() - Destination2).Size(), DeltaDistance.Size())))
		{
			SetActorLocation(Destination2);
			RemainingDelay = Delay1;
			bIsGoingToDestination1 = (!bIsGoingToDestination1);
		}*/
		if ((GetActorLocation() - Destination2).Size() <= DeltaDistance.Size())
		{
			SetActorLocation(Destination2);
			RemainingDelay = Delay1;
			bIsGoingToDestination1 = (!bIsGoingToDestination1);
		}
	}
}

void ASpike::SetDestinations(const FVector& NewDestination1, const FVector& NewDestination2)
{
	Destination1 = NewDestination1;
	Destination2 = NewDestination2;
}

void ASpike::SetSpeeds(float NewSpeed1, float NewSpeed2)
{
	Speed1 = NewSpeed1;
	Speed2 = NewSpeed2;
}

void ASpike::SetDelay(float NewDelay1, float NewDelay2)
{
	Delay1 = NewDelay1;
	Delay2 = NewDelay2;
}
