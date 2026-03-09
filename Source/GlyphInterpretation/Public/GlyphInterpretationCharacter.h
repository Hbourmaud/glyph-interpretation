// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GlyphRecognizer.h"
#include <GlyphDrawingWidget.h>
#include <GlyphDrawingComponent.h>
#include "GlyphInterpretationCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AGlyphInterpretationCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Draw Glyph Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DrawGlyphAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleDrawingAction;

public:
	AGlyphInterpretationCharacter();

	void BeginPlay() override;
	
	/* Glyph Part */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Glyph")
	UGlyphDrawingComponent* GlyphDrawingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glyph")
	TSubclassOf<UGlyphDrawingWidget> DrawingWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glyph Recognition")
	TArray<UGlyph*> GlyphLibrary;

	UFUNCTION()
	void OnGlyphDrawn(const TArray<FVector2D>& DrawnPoints);

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called when draw input is started */
	void DrawStarted(const FInputActionValue& Value);

	/** Called when draw input is ongoing */
	void DrawOngoing(const FInputActionValue& Value);

	/** Called when draw input is completed */
	void DrawCompleted(const FInputActionValue& Value);

	void SpawnGlyphActor(UGlyph* MatchedGlyph);

	void ToggleDrawingMode();

	void ShowDrawingWidget();

	void HideDrawingWidget();

protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY()
	UGlyphDrawingWidget* DrawingWidget;

	FVector2D ScreenToNormalizedPosition(FVector2D ScreenPosition) const;

	bool IsDrawingModeActive = false;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

