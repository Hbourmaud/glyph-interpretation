#pragma once

#include "CoreMinimal.h"
#include "Glyph.h"
#include "UObject/NoExportTypes.h"
#include "GlyphRecognizer.generated.h"

USTRUCT(BlueprintType)
struct FGlyphMatchResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Glyph Recognition")
	UGlyph* MatchedGlyph = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Glyph Recognition")
	float Score = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Glyph Recognition")
	bool IsMatch = false;
};

UCLASS()
class GLYPHINTERPRETATION_API UGlyphRecognizer : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Glyph Recognition")
	static FGlyphMatchResult RecognizeGlyph(const TArray<FVector2D>& DrawnPoints, const TArray<UGlyph*>& GlyphLibrary);

	UFUNCTION(BlueprintCallable, Category = "Glyph Recognition")
	static TArray<FVector2D> Resample(const TArray<FVector2D>& Points, int32 NumPoints);

	UFUNCTION(BlueprintCallable, Category = "Glyph Recognition")
	static TArray<FVector2D> ScaleToUnitSquare(const TArray<FVector2D>& Points);

	UFUNCTION(BlueprintCallable, Category = "Glyph Recognition")
	static TArray<FVector2D> TranslateToOrigin(const TArray<FVector2D>& Points);

	UFUNCTION(BlueprintCallable, Category = "Glyph Recognition")
	static float GreedyCloudMatch(const TArray<FVector2D>& PointsA, const TArray<FVector2D>& PointsB, float MinRotation = -45.0f, float MaxRotation = 45.0f, float RotationStep = 2.0f);

private:
	static float PathLength(const TArray<FVector2D>& Points);

	static FVector2D Centroid(const TArray<FVector2D>& Points);

	static TArray<FVector2D> RotateBy(const TArray<FVector2D>& Points, float AngleDegrees);

	static void GetBoundingBox(const TArray<FVector2D>& Points, FVector2D& Min, FVector2D& Max);

	static float CloudDistance(const TArray<FVector2D>& PointsA, const TArray<FVector2D>& PointsB);
};
