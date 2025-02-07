#include "SpartaCharacter.h"

#include "SpartaPlayerController.h"
#include "SpartaGameState.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/TextBlock.h"

ASpartaCharacter::ASpartaCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	// USpringArmComponent::SocketName은 springArm의 끝 부분을 가리킨다고 한다.
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	CameraComp->PostProcessSettings.VignetteIntensity = 1.0f;
	CameraComp->PostProcessSettings.AutoExposureBias = -4.0f;

	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(GetMesh());
	OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen); // 빌보드

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.7f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;
	bIsSprinting = false;

	CurrentDebuffedSpeedMultiplier = 1.0f;
	DebuffedSpeedMultiplier = 0.5f;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	MaxHealth = 100.0f;
	Health = MaxHealth;
}

void ASpartaCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateOverheadHP();
}

void ASpartaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ASpartaPlayerController* PlayerController = Cast<ASpartaPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Move
				);
			}

			if (PlayerController->JumpAction)
			{
				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartJump
				);

				EnhancedInput->BindAction(
					PlayerController->JumpAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopJump
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ASpartaCharacter::StartSprint
				);

				EnhancedInput->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ASpartaCharacter::StopSprint
				);
			}
		}
	}
}

void ASpartaCharacter::Move(const FInputActionValue& Value)
{
	// GetActorForwardVector나 GetActorRightVector 함수를 쓰려면 컨트롤러가 존재해야해서 체크 해준다.
	if (!Controller) return;

	FVector2D MoveInput = Value.Get<FVector2D>();

	if (GetRemainingControlDebuffTime() > 0)
	{
		MoveInput.X = -MoveInput.X;
		MoveInput.Y = -MoveInput.Y;
	}

	if (GetCharacterMovement())
	{
		if (bIsSprinting)
		{
			GetCharacterMovement()->MaxWalkSpeed = SprintSpeed * CurrentDebuffedSpeedMultiplier;
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = NormalSpeed * CurrentDebuffedSpeedMultiplier;
		}
	}

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void ASpartaCharacter::StartJump(const FInputActionValue& Value)
{
	if (Value.Get<bool>())
	{
		Jump();
	}
}

void ASpartaCharacter::StopJump(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}

void ASpartaCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void ASpartaCharacter::StartSprint(const FInputActionValue& Value)
{	
	bIsSprinting = true;
}

void ASpartaCharacter::StopSprint(const FInputActionValue& Value)
{
	bIsSprinting = false;
}

float ASpartaCharacter::GetHealth() const
{
	return Health;
}

void ASpartaCharacter::AddHealth(float Amount)
{
	Health = FMath::Clamp(Health + Amount, 0.0f, MaxHealth);
	UpdateOverheadHP();
}

void ASpartaCharacter::ActivateSlowDebuff(float DebuffTime)
{
	if (DebuffTime < GetRemainingSlowDebuffTime())
	{
		return;
	}

	CurrentDebuffedSpeedMultiplier = DebuffedSpeedMultiplier;

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("Slow Actiavted"));

	GetWorldTimerManager().SetTimer(SlowDebuffTimerHandle, [&]()
		{

			CurrentDebuffedSpeedMultiplier = 1.0f;
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("Slow Removed"));

		}, DebuffTime, false);
}

void ASpartaCharacter::ActivateControlReversalDebuff(float DebuffTime)
{
	if (DebuffTime < GetRemainingControlDebuffTime())
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("Moves Are Reveresed"));

	GetWorldTimerManager().SetTimer(ControlDebuffTimerHandle, [&]()
		{
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("Moves Return To Normal"));

		}, DebuffTime, false);
}

void ASpartaCharacter::ActivateBlindDebuff(float DebuffTime)
{
	if (DebuffTime < GetRemainingBlindDebuffTime())
	{
		return;
	}

	CameraComp->PostProcessSettings.bOverride_VignetteIntensity = true;
	CameraComp->PostProcessSettings.bOverride_AutoExposureBias = true;
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("You Are Blind"));

	GetWorldTimerManager().SetTimer(BlindDebuffTimerHandle, [&]()
		{
			CameraComp->PostProcessSettings.bOverride_VignetteIntensity = false;
			CameraComp->PostProcessSettings.bOverride_AutoExposureBias = false;
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.0f, FColor::Red, TEXT("You Are Not Blind Now"));

		}, DebuffTime, false);
}

float ASpartaCharacter::GetRemainingSlowDebuffTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(SlowDebuffTimerHandle);
}

float ASpartaCharacter::GetRemainingControlDebuffTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(ControlDebuffTimerHandle);
}

float ASpartaCharacter::GetRemainingBlindDebuffTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(BlindDebuffTimerHandle);
}

float ASpartaCharacter::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health = FMath::Clamp(Health - DamageAmount, 0.0f, MaxHealth);
	UpdateOverheadHP();

	if (Health <= 0.0f)
	{
		OnDeath();
	}

	return ActualDamage;
}

void ASpartaCharacter::OnDeath()
{
	ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
	if (SpartaGameState)
	{
		SpartaGameState->OnGameOver();
	}
}

void ASpartaCharacter::UpdateOverheadHP()
{
	if (!OverheadWidget) return;

	UUserWidget* OverheadWidgetInstance = OverheadWidget->GetUserWidgetObject();
	if (!OverheadWidgetInstance) return;

	if (UTextBlock* HPText = Cast<UTextBlock>(OverheadWidgetInstance->GetWidgetFromName(TEXT("OverHeadHP"))))
	{
		HPText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), Health, MaxHealth)));
	}
}
