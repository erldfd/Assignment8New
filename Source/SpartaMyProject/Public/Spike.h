#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Spike.generated.h"

UCLASS()
class SPARTAMYPROJECT_API ASpike : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpike();

	void SetDestinations(const FVector& NewDestination1, const FVector& NewDestination2);
	void SetSpeeds(float NewSpeed1, float NewSpeed2);
	void SetDelay(float NewDelay1, float NewDelay2);

protected:

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

protected:

	UPROPERTY(EditAnywhere, Category = "Spike")
	bool bShouldUseRelativePositon = true;

	UPROPERTY(EditAnywhere, Category = "Spike")
	bool bShouldUseWorldPositon = false;

	UPROPERTY(EditAnywhere, Category = "Spike", meta = (Tooltip = "This actor moves from Destination1 to Destination2 or vice versa"), meta = (EditCondition = "bShouldUseWorldPositon", EditConditionHides))
	FVector Destination1 = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Spike", meta = (Tooltip = "This actor moves from Destination1 to Destination2 or vice versa"), meta = (EditCondition = "bShouldUseWorldPositon", EditConditionHides))
	FVector Destination2 = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Spike", meta = (EditCondition = "bShouldUseRelativePositon", EditConditionHides))
	FVector RelativeDestination = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Spike", meta = (Tooltip = "When Moving to Destination2"))
	float Speed1 = 500;

	UPROPERTY(EditAnywhere, Category = "Spike", meta = (Tooltip = "When Moving to Destination1"))
	float Speed2 = 200;

	UPROPERTY(EditAnywhere, Category = "Spike", meta = (Tooltip = "When Arrived at Destination1, Wait for Delay1"))
	float Delay1 = 1;

	UPROPERTY(EditAnywhere, Category = "Spike", meta = (Tooltip = "When Arrived at Destination2, Wait for Delay2"))
	float Delay2 = 10;

	bool bIsGoingToDestination1 = false;
	float RemainingDelay = 0;
};
