#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "SpartaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UWidgetComponent;
struct FInputActionValue;

UCLASS()
class SPARTAMYPROJECT_API ASpartaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASpartaCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* OverheadWidget;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float Amount);

	void ActivateSlowDebuff(float DebuffTime);
	void ActivateControlReversalDebuff(float DebuffTime);
	void ActivateBlindDebuff(float DebuffTime);

	float GetRemainingSlowDebuffTime() const;
	float GetRemainingControlDebuffTime() const;
	float GetRemainingBlindDebuffTime() const;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float Health;

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void StartJump(const FInputActionValue& Value);

	UFUNCTION()
	void StopJump(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void StartSprint(const FInputActionValue& Value);

	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);

	void OnDeath();
	void UpdateOverheadHP();

private:

	float NormalSpeed;
	float SprintSpeedMultiplier;
	float SprintSpeed;
	float bIsSprinting;

	float CurrentDebuffedSpeedMultiplier;
	float DebuffedSpeedMultiplier;

	FTimerHandle SlowDebuffTimerHandle;
	FTimerHandle ControlDebuffTimerHandle;
	FTimerHandle BlindDebuffTimerHandle;
};
