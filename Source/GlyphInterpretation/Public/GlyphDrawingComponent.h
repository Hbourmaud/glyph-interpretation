#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Canvas.h"
#include "GlyphDrawingComponent.generated.h"

// TODO : avoid making this ?
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDrawingComplete, const TArray<FVector2D>&, DrawnPoints);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GLYPHINTERPRETATION_API UGlyphDrawingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGlyphDrawingComponent();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glyph Drawing")
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glyph Drawing")
	FLinearColor DrawColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glyph Drawing")
	float LineThickness = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glyph Drawing")
	int32 RenderTargetSize = 512;

	UPROPERTY(BlueprintReadOnly, Category = "Glyph Drawing")
	bool bIsDrawing = false;

	UPROPERTY(BlueprintReadOnly, Category = "Glyph Drawing")
	TArray<FVector2D> CurrentDrawnPoints;

	UPROPERTY(BlueprintAssignable, Category = "Glyph Drawing")
	FOnDrawingComplete OnDrawingComplete;

	UFUNCTION(BlueprintCallable, Category = "Glyph Drawing")
	void StartDrawing(FVector2D StartPoint);

	UFUNCTION(BlueprintCallable, Category = "Glyph Drawing")
	void UpdateDrawing(FVector2D CurrentPoint);

	UFUNCTION(BlueprintCallable, Category = "Glyph Drawing")
	void EndDrawing();

	UFUNCTION(BlueprintCallable, Category = "Glyph Drawing")
	void ClearDrawing();

	UFUNCTION(BlueprintCallable, Category = "Glyph Drawing")
	void InitializeRenderTarget();

	UFUNCTION(BlueprintCallable, Category = "Glyph Drawing")
	void SaveRenderTargetToFile(FString Filename = TEXT("GlyphDrawing")); // TEMP

private:
	FVector2D LastPoint;

	void DrawLineOnRenderTarget(FVector2D From, FVector2D To);
};
