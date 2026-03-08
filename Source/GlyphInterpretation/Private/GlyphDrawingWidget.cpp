#include "GlyphDrawingWidget.h"
#include "GlyphDrawingComponent.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Blueprint/WidgetTree.h"

void UGlyphDrawingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (DrawingCanvas) {
		DrawingCanvas->SetVisibility(ESlateVisibility::Visible);
	}
}

void UGlyphDrawingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateRenderTarget();
}

void UGlyphDrawingWidget::InitializeWidget(UGlyphDrawingComponent* InDrawingComponent)
{
	DrawingComponent = InDrawingComponent;
}

void UGlyphDrawingWidget::UpdateRenderTarget()
{
	if (DrawingComponent && DrawingComponent->RenderTarget && DrawingCanvas) {
		DrawingCanvas->SetBrushResourceObject(DrawingComponent->RenderTarget);
	}
}