#include "SpartaPlayerController.h"

#include "SpartaGameState.h"
#include "SpartaGameInstance.h"

#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

ASpartaPlayerController::ASpartaPlayerController() :
	InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	InteractionAction(nullptr),
	HUDWidgetClass(nullptr),
	HUDWidgetInstance(nullptr),
	MainMenuWidgetClass(nullptr),
	MainMenuWidgetInstance(nullptr)
{

}

void ASpartaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}

	FString CurrentMapName = GetWorld()->GetMapName();
	if (CurrentMapName.Contains("MenuLevel"))
	{
		ShowMainMenu(false);
	}
}

void ASpartaPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckInteractableItem();
}

void ASpartaPlayerController::ActivateTrapText(const FString& TrapMessage)
{
	UTextBlock* TrapActivatedText = Cast<UTextBlock>(HUDWidgetInstance->GetWidgetFromName(TEXT("TrapActivatedText")));

	if (TrapActivatedText == nullptr)
	{
		return;
	}

	TrapActivatedText->SetText(FText::FromString(TrapMessage));
	TrapActivatedText->SetVisibility(ESlateVisibility::HitTestInvisible);
	UE_LOG(LogTemp, Error, TEXT("%s"), *TrapMessage);
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [&]()
		{
			UTextBlock* InTrapActivatedText = Cast<UTextBlock>(HUDWidgetInstance->GetWidgetFromName(TEXT("TrapActivatedText")));

			if (InTrapActivatedText == nullptr)
			{
				return;
			}

			InTrapActivatedText->SetVisibility(ESlateVisibility::Hidden);

		}, 3, false);
}

void ASpartaPlayerController::CheckInteractableItem()
{
	if (HUDWidgetInstance == nullptr)
	{
		return;
	}

	UWidget* PickupTextWidget = HUDWidgetInstance->GetWidgetFromName(TEXT("PickupStarText"));

	if (PickupTextWidget == nullptr)
	{
		return;
	}

	FVector WorldLocation;
	FVector WorldDirection;

	int32 ViewportX, ViewportY;
	GetViewportSize(ViewportX, ViewportY);

	FVector2D Center(ViewportX / 2, ViewportY / 2);


	if (DeprojectScreenPositionToWorld(Center.X, Center.Y, WorldLocation, WorldDirection) == false)
	{
		return;
	}

	float InteractableRange = 600.0f;
	const FVector& StartPoint = GetPawn()->GetActorLocation();
	const FVector& EndPoint = WorldLocation + (WorldDirection * InteractableRange);

	FHitResult Result;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetPawn());

	FCollisionObjectQueryParams ObjectQueryParam;
	ObjectQueryParam.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);

	TArray<FHitResult> Test;

	bool bIsHit = GetWorld()->LineTraceTestByChannel(StartPoint, EndPoint, ECollisionChannel::ECC_GameTraceChannel2, QueryParams);

	if (bIsHit)
	{
		PickupTextWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		PickupTextWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

UUserWidget* ASpartaPlayerController::GetHUDWidget() const
{
	return HUDWidgetInstance;
}

void ASpartaPlayerController::ShowMainMenu(bool bIsRestart)
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<UUserWidget>(this, MainMenuWidgetClass);
		if (MainMenuWidgetInstance)
		{
			MainMenuWidgetInstance->AddToViewport();

			bShowMouseCursor = true;
			SetInputMode(FInputModeUIOnly());
		}

		if (UTextBlock* ButtonText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName(TEXT("StartButtonText"))))
		{
			if (bIsRestart)
			{
				ButtonText->SetText(FText::FromString(TEXT("Restart")));
			}
			else
			{
				ButtonText->SetText(FText::FromString(TEXT("Start")));
			}
		}

		if (bIsRestart)
		{
			UFunction* PlayAnimFunc = MainMenuWidgetInstance->FindFunction(FName("PlayGameOverAnim"));
			if (PlayAnimFunc)
			{
				MainMenuWidgetInstance->ProcessEvent(PlayAnimFunc, nullptr);
			}

			if (UTextBlock* TotalScoreText = Cast<UTextBlock>(MainMenuWidgetInstance->GetWidgetFromName("TotalScoreText")))
			{
				if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
				{
					TotalScoreText->SetText(FText::FromString(
						FString::Printf(TEXT("Total Score: %d"), SpartaGameInstance->TotalScore)
						));
				}
			}
		}
	}
}

void ASpartaPlayerController::ShowGameHUD()
{
	if (HUDWidgetInstance)
	{
		HUDWidgetInstance->RemoveFromParent();
		HUDWidgetInstance = nullptr;
	}

	if (MainMenuWidgetInstance)
	{
		MainMenuWidgetInstance->RemoveFromParent();
		MainMenuWidgetInstance = nullptr;
	}

	if (HUDWidgetClass)
	{
		HUDWidgetInstance = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidgetInstance)
		{
			HUDWidgetInstance->AddToViewport();

			bShowMouseCursor = false;
			SetInputMode(FInputModeGameOnly());
		}

		ASpartaGameState* SpartaGameState = GetWorld() ? GetWorld()->GetGameState<ASpartaGameState>() : nullptr;
		if (SpartaGameState)
		{
			SpartaGameState->UpdateHUD();
		}
	}
}

void ASpartaPlayerController::StartGame()
{
	if (USpartaGameInstance* SpartaGameInstance = Cast<USpartaGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SpartaGameInstance->CurrentLevelIndex = 0;
		SpartaGameInstance->TotalScore = 0;
	}

	UGameplayStatics::OpenLevel(GetWorld(), FName("BasicLevel"));
	SetPause(false);
}
