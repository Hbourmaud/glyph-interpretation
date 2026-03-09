#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class UGlyph;
class UTextureRenderTarget2D;

class SGlyphDrawingPanel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SGlyphDrawingPanel) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UGlyph* InGlyph);

    virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool IsParentEnabled) const override;
    virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
    UGlyph* Glyph;

    TArray<FVector2D> DrawnPoints;

    bool IsDrawing;

    FVector2D LastMousePosition;

    FVector2D CanvasSize;

    FReply OnClearClicked();
    FReply OnSaveModelClicked();

    TArray<FVector2D> ConvertToModel() const;
};