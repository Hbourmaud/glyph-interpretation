#include "GlyphDrawingComponent.h"
#include "Engine/Canvas.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "CanvasItem.h"
#include <Serialization/BufferArchive.h>
#include <ImageUtils.h>

UGlyphDrawingComponent::UGlyphDrawingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGlyphDrawingComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!RenderTarget) {
		InitializeRenderTarget();
	}
}

void UGlyphDrawingComponent::InitializeRenderTarget()
{
	RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(
		GetWorld(),
		RenderTargetSize,
		RenderTargetSize,
		RTF_RGBA8
	);

	if (RenderTarget) {
		ClearDrawing();
	}
}

void UGlyphDrawingComponent::StartDrawing(FVector2D StartPoint)
{
	if (!RenderTarget) {
		InitializeRenderTarget();
	}

	ClearDrawing();

	IsDrawing = true;
	CurrentDrawnPoints.Empty();
	CurrentDrawnPoints.Add(StartPoint);
	LastPoint = StartPoint;
}

void UGlyphDrawingComponent::UpdateDrawing(FVector2D CurrentPoint)
{
	if (!IsDrawing || !RenderTarget) {
		return;
	}

	CurrentDrawnPoints.Add(CurrentPoint);
	DrawLineOnRenderTarget(LastPoint, CurrentPoint);
	LastPoint = CurrentPoint;
}

void UGlyphDrawingComponent::EndDrawing()
{
	if (!IsDrawing) {
		return;
	}

	IsDrawing = false;

	OnDrawingComplete.Broadcast(CurrentDrawnPoints);

	SaveRenderTargetToFile(TEXT("GlyphDrawing.png")); // TODO debug
	
	ClearDrawing();
}

void UGlyphDrawingComponent::ClearDrawing()
{
	if (!RenderTarget) {
		return;
	}		

	UKismetRenderingLibrary::ClearRenderTarget2D(GetWorld(), RenderTarget, FLinearColor::Black);
	CurrentDrawnPoints.Empty();
	IsDrawing = false;
}

void UGlyphDrawingComponent::DrawLineOnRenderTarget(FVector2D From, FVector2D To)
{
	if (!RenderTarget) {
		return;
	}

	UCanvas* Canvas;
	FVector2D Size;
	FDrawToRenderTargetContext Context;

	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(GetWorld(), RenderTarget, Canvas, Size, Context);

	if (Canvas) {
		FCanvasLineItem LineItem(From, To);
		LineItem.SetColor(DrawColor);
		LineItem.LineThickness = LineThickness;
		Canvas->DrawItem(LineItem);
	}

	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(GetWorld(), Context);
}

// TEMP
void UGlyphDrawingComponent::SaveRenderTargetToFile(FString Filename)
{
	if (!RenderTarget)
	{
		UE_LOG(LogTemp, Error, TEXT("No render target to save!"));
		return;
	}

	FString ProjectDir = FPaths::ProjectDir();
	FString SavePath = ProjectDir + TEXT("Saved/Screenshots/") + Filename + TEXT(".png");

	FArchive* Ar = IFileManager::Get().CreateFileWriter(*SavePath);
	if (Ar)
	{
		FBufferArchive Buffer;
		bool bSuccess = FImageUtils::ExportRenderTarget2DAsPNG(RenderTarget, Buffer);

		if (bSuccess)
		{
			Ar->Serialize(const_cast<uint8*>(Buffer.GetData()), Buffer.Num());
			UE_LOG(LogTemp, Warning, TEXT("Render target saved to: %s"), *SavePath);
		}

		delete Ar;
	}
}