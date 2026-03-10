// Copyright Epic Games, Inc. All Rights Reserved.

#include "GlyphInterpretationCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"
#include "GlyphDrawingWidget.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AGlyphInterpretationCharacter

AGlyphInterpretationCharacter::AGlyphInterpretationCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Glyph Drawing Component
	GlyphDrawingComponent = CreateDefaultSubobject<UGlyphDrawingComponent>(TEXT("GlyphDrawingComponent"));
}

void AGlyphInterpretationCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GlyphDrawingComponent) {
		GlyphDrawingComponent->OnDrawingComplete.AddDynamic(this, &AGlyphInterpretationCharacter::OnGlyphDrawn);
	}

	if (DrawingWidgetClass)	{
		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		if (PlayerController) {
			DrawingWidget = CreateWidget<UGlyphDrawingWidget>(PlayerController, DrawingWidgetClass);

			if (DrawingWidget) {
				DrawingWidget->InitializeWidget(GlyphDrawingComponent);
				DrawingWidget->AddToViewport(100);
				DrawingWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGlyphInterpretationCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AGlyphInterpretationCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGlyphInterpretationCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGlyphInterpretationCharacter::Look);

		// Glyphs
		if (DrawGlyphAction) {
			EnhancedInputComponent->BindAction(DrawGlyphAction, ETriggerEvent::Started, this, &AGlyphInterpretationCharacter::DrawStarted);
			EnhancedInputComponent->BindAction(DrawGlyphAction, ETriggerEvent::Triggered, this, &AGlyphInterpretationCharacter::DrawOngoing);
			EnhancedInputComponent->BindAction(DrawGlyphAction, ETriggerEvent::Completed, this, &AGlyphInterpretationCharacter::DrawCompleted);
		}

		if (ToggleDrawingAction) {
			EnhancedInputComponent->BindAction(ToggleDrawingAction, ETriggerEvent::Started, this, &AGlyphInterpretationCharacter::ToggleDrawingMode);
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AGlyphInterpretationCharacter::Move(const FInputActionValue& Value)
{
	if (IsDrawingModeActive) {
		return;
	}

	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGlyphInterpretationCharacter::Look(const FInputActionValue& Value)
{
	if (IsDrawingModeActive) {
		return;
	}

	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGlyphInterpretationCharacter::DrawStarted(const FInputActionValue& Value)
{
	if (!IsDrawingModeActive || !GlyphDrawingComponent) {
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController) {
		float MouseX, MouseY;

		PlayerController->GetMousePosition(MouseX, MouseY);
		FVector2D NormalizedPosition = ScreenToNormalizedPosition(FVector2D(MouseX, MouseY));

		GlyphDrawingComponent->StartDrawing(NormalizedPosition);
	}
}

void AGlyphInterpretationCharacter::DrawOngoing(const FInputActionValue& Value)
{
	if (!IsDrawingModeActive || !GlyphDrawingComponent || !GlyphDrawingComponent->IsDrawing) {
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController) {
		float MouseX, MouseY;

		PlayerController->GetMousePosition(MouseX, MouseY);
		FVector2D NormalizedPosition = ScreenToNormalizedPosition(FVector2D(MouseX, MouseY));

		GlyphDrawingComponent->UpdateDrawing(NormalizedPosition);
	}
}

void AGlyphInterpretationCharacter::DrawCompleted(const FInputActionValue& Value)
{
	if (!IsDrawingModeActive || !GlyphDrawingComponent) {
		return;
	}

	GlyphDrawingComponent->EndDrawing();
}

void AGlyphInterpretationCharacter::OnGlyphDrawn(const TArray<FVector2D>& DrawnPoints)
{
	if (GlyphLibrary.Num() == 0) {
		UE_LOG(LogTemplateCharacter, Warning, TEXT("No glyphs in library to recognize against"));

		if (GlyphDrawingComponent) {
			GlyphDrawingComponent->ClearDrawing();
		}

		return;
	}

	FGlyphMatchResult Result = UGlyphRecognizer::RecognizeGlyph(DrawnPoints, GlyphLibrary);

	if (Result.IsMatch && Result.MatchedGlyph) {
		UE_LOG(LogTemplateCharacter, Warning, TEXT("Recognized: '%s' (Score: %.3f)"),
			*Result.MatchedGlyph->GlyphName, Result.Score);

		SpawnGlyphActor(Result.MatchedGlyph);

		HideDrawingWidget();
	} else {
		UE_LOG(LogTemplateCharacter, Warning, TEXT("No match found (Best score: %.3f)"), Result.Score);
	}

	if (GlyphDrawingComponent) {
		GlyphDrawingComponent->ClearDrawing();
	}
}

void AGlyphInterpretationCharacter::SpawnGlyphActor(UGlyph* MatchedGlyph)
{
	if (!MatchedGlyph || !MatchedGlyph->ActorToSpawn) {
		UE_LOG(LogTemplateCharacter, Warning, TEXT("No actor configured for glyph '%s'"),
			*MatchedGlyph->GlyphName);

		return;
	}

	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 200.0f + MatchedGlyph->SpawnOffset;
	FRotator SpawnRotation = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		MatchedGlyph->ActorToSpawn,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
}

void AGlyphInterpretationCharacter::ToggleDrawingMode()
{
	IsDrawingModeActive = !IsDrawingModeActive;

	if (IsDrawingModeActive) {
		ShowDrawingWidget();
	} else {
		HideDrawingWidget();
	}
}

void AGlyphInterpretationCharacter::ShowDrawingWidget()
{
	if (DrawingWidget) {
		DrawingWidget->SetVisibility(ESlateVisibility::Visible);
		IsDrawingModeActive = true;

		if (GlyphDrawingComponent) {
			GlyphDrawingComponent->ClearDrawing();
		}

		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		if (PlayerController) {
			PlayerController->bShowMouseCursor = true;
		}

		if (GetCharacterMovement()) {
			GetCharacterMovement()->DisableMovement();
		}
	}
}

void AGlyphInterpretationCharacter::HideDrawingWidget()
{
	if (DrawingWidget) {
		DrawingWidget->SetVisibility(ESlateVisibility::Hidden);
		IsDrawingModeActive = false;

		APlayerController* PlayerController = Cast<APlayerController>(GetController());

		if (PlayerController) {
			PlayerController->bShowMouseCursor = false;

			FInputModeGameOnly InputMode;

			InputMode.SetConsumeCaptureMouseDown(false);
			PlayerController->SetInputMode(InputMode);
		}

		if (GetCharacterMovement()) {
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}

		if (GlyphDrawingComponent && GlyphDrawingComponent->IsDrawing)	{
			GlyphDrawingComponent->EndDrawing();
		}
	}
}

FVector2D AGlyphInterpretationCharacter::ScreenToNormalizedPosition(FVector2D ScreenPosition) const
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController) {
		int32 ViewportSizeX, ViewportSizeY;

		PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);

		if (GlyphDrawingComponent) {
			float ClampedX = FMath::Clamp(ScreenPosition.X, 0.0f, (float)ViewportSizeX);
			float ClampedY = FMath::Clamp(ScreenPosition.Y, 0.0f, (float)ViewportSizeY);

			float NormalizedX = (ClampedX / ViewportSizeX) * GlyphDrawingComponent->RenderTargetSize;
			float NormalizedY = (ClampedY / ViewportSizeY) * GlyphDrawingComponent->RenderTargetSize;

			NormalizedX = FMath::Clamp(NormalizedX, 0.0f, (float)GlyphDrawingComponent->RenderTargetSize);
			NormalizedY = FMath::Clamp(NormalizedY, 0.0f, (float)GlyphDrawingComponent->RenderTargetSize);

			return FVector2D(NormalizedX, NormalizedY);
		}
	}

	return FVector2D::ZeroVector;
}