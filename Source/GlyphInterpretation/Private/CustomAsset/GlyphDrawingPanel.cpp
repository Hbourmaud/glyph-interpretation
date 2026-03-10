#include "CustomAsset/GlyphDrawingPanel.h"
#include "Glyph.h"
#include "GlyphRecognizer.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Rendering/DrawElements.h"

#define LOCTEXT_NAMESPACE "SGlyphDrawingPanel"

void SGlyphDrawingPanel::Construct(const FArguments& InArgs, UGlyph* InGlyph)
{
    Glyph = InGlyph;
    IsDrawing = false;
    CanvasSize = FVector2D(512.0f, 512.0f);

    ChildSlot
        [
            SNew(SVerticalBox)
                + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("Instructions", "Click and drag to draw / Release to finish"))
                ]

                + SVerticalBox::Slot()
                .FillHeight(1.0f)
                .Padding(10.0f)
                [
                    SNew(SBorder)
                        .BorderImage(FCoreStyle::Get().GetBrush("WhiteBrush"))
                        .BorderBackgroundColor(FLinearColor::White)
                        .Padding(0.0f)
                        .HAlign(HAlign_Fill)
                        .VAlign(VAlign_Fill)
                ]

            + SVerticalBox::Slot()
                .AutoHeight()
                .Padding(10.0f)
                [
                    SNew(SHorizontalBox)

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(5.0f)
                        [
                            SNew(SButton)
                                .Text(LOCTEXT("ClearButton", "Clear"))
                                .OnClicked(this, &SGlyphDrawingPanel::OnClearClicked)
                        ]

                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .Padding(5.0f)
                        [
                            SNew(SButton)
                                .Text(LOCTEXT("SaveModelButton", "Save as Model"))
                                .OnClicked(this, &SGlyphDrawingPanel::OnSaveModelClicked)
                        ]
                ]
        ];
}

int32 SGlyphDrawingPanel::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool IsParentEnabled) const
{
    int32 RetLayerId = SCompoundWidget::OnPaint(
        Args,
        AllottedGeometry,
        MyCullingRect,
        OutDrawElements,
        LayerId,
        InWidgetStyle,
        IsParentEnabled
    );

    if (DrawnPoints.Num() > 1) {
        TArray<FVector2D> LinePoints;

        LinePoints.Reserve(DrawnPoints.Num());

        FVector2D LocalSize = AllottedGeometry.GetLocalSize();

        for (const FVector2D& Point : DrawnPoints) {
            FVector2D ScreenPoint = Point * (LocalSize / CanvasSize);

            LinePoints.Add(ScreenPoint);
        }

        FSlateDrawElement::MakeLines(
            OutDrawElements,
            RetLayerId + 1,
            AllottedGeometry.ToPaintGeometry(),
            LinePoints,
            ESlateDrawEffect::None,
            FLinearColor::Black,
            true,
            3.0f
        );
    }

    return RetLayerId + 1;
}

FReply SGlyphDrawingPanel::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton) {
        IsDrawing = true;
        DrawnPoints.Empty();

        FVector2D LocalPos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
        FVector2D LocalSize = MyGeometry.GetLocalSize();
        FVector2D CanvasPoint = LocalPos * (CanvasSize / LocalSize);

        DrawnPoints.Add(CanvasPoint);
        LastMousePosition = LocalPos;

        return FReply::Handled().CaptureMouse(SharedThis(this));
    }

    return FReply::Unhandled();
}

FReply SGlyphDrawingPanel::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && IsDrawing) {
        IsDrawing = false;

        return FReply::Handled().ReleaseMouseCapture();
    }

    return FReply::Unhandled();
}

FReply SGlyphDrawingPanel::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (IsDrawing) {
        FVector2D LocalPos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
        FVector2D LocalSize = MyGeometry.GetLocalSize();

        FVector2D CanvasPoint = LocalPos * (CanvasSize / LocalSize);

        DrawnPoints.Add(CanvasPoint);
        LastMousePosition = LocalPos;

        Invalidate(EInvalidateWidgetReason::Paint);

        return FReply::Handled();
    }

    return FReply::Unhandled();
}

FReply SGlyphDrawingPanel::OnClearClicked()
{
    DrawnPoints.Empty();
    Invalidate(EInvalidateWidget::Paint);

    return FReply::Handled();
}

FReply SGlyphDrawingPanel::OnSaveModelClicked()
{
    if (DrawnPoints.Num() < 2 || !Glyph) {
        return FReply::Handled();
    }

    TArray<FVector2D> Model = ConvertToModel();

    Glyph->ModelPoints = Model;
    Glyph->MarkPackageDirty();

    return FReply::Handled();
}

TArray<FVector2D> SGlyphDrawingPanel::ConvertToModel() const
{
    TArray<FVector2D> Model = DrawnPoints;

    Model = UGlyphRecognizer::Resample(Model, Glyph->NumResamplePoints);
    Model = UGlyphRecognizer::ScaleToUnitSquare(Model);
    Model = UGlyphRecognizer::TranslateToOrigin(Model);

    return Model;
}

#undef LOCTEXT_NAMESPACE