#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GlyphDrawingWidget.generated.h"

class UGlyphDrawingComponent;

UCLASS()
class GLYPHINTERPRETATION_API UGlyphDrawingWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "Glyph")
	UGlyphDrawingComponent* DrawingComponent;

	UFUNCTION(BlueprintCallable, Category = "Glyph")
	void InitializeWidget(UGlyphDrawingComponent* InDrawingComponent);

protected:
	virtual void NativeConstruct() override;
};
