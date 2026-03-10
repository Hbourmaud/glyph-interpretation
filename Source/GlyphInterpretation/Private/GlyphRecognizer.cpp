#include "GlyphRecognizer.h"

TArray<FVector2D> UGlyphRecognizer::Resample(const TArray<FVector2D>& Points, int32 NumPoints)
{
	if (Points.Num() < 2 || NumPoints < 2) {
		return Points;
	}

	TArray<FVector2D> ResampledPoints;
	ResampledPoints.Add(Points[0]);

	float TotalLength = PathLength(Points);
	float IntervalLength = TotalLength / (NumPoints - 1);
	float CumulativeDistance = 0.0f;

	for (int32 i = 1; i < Points.Num(); ++i) {
		float SegmentLength = FVector2D::Distance(Points[i - 1], Points[i]);

		if (CumulativeDistance + SegmentLength >= IntervalLength) {
			while (CumulativeDistance + SegmentLength >= IntervalLength) {
				float Ratio = (IntervalLength - CumulativeDistance) / SegmentLength;
				FVector2D NewPoint = FMath::Lerp(Points[i - 1], Points[i], Ratio);

				ResampledPoints.Add(NewPoint);

				if (ResampledPoints.Num() == NumPoints) {
					return ResampledPoints;
				}

				CumulativeDistance = 0.0f;
				SegmentLength = FVector2D::Distance(NewPoint, Points[i]);
			}
		}
			CumulativeDistance += SegmentLength;
	}

	while (ResampledPoints.Num() < NumPoints) {
		ResampledPoints.Add(Points.Last());
	}

	return ResampledPoints;
}

TArray<FVector2D> UGlyphRecognizer::ScaleToUnitSquare(const TArray<FVector2D>& Points)
{
	FVector2D Min, Max;
	GetBoundingBox(Points, Min, Max);

	float Width = Max.X - Min.X;
	float Height = Max.Y - Min.Y;

	if (Width <= 0.0f || Height <= 0.0f) {
		return Points;
	}

	float Scale = FMath::Max(Width, Height);

	TArray<FVector2D> ScaledPoints;
	ScaledPoints.Reserve(Points.Num());

	for (const FVector2D& Point : Points) {
		ScaledPoints.Add((Point - Min) / Scale);
	}

	return ScaledPoints;
}

TArray<FVector2D> UGlyphRecognizer::TranslateToOrigin(const TArray<FVector2D>& Points)
{
	FVector2D Center = Centroid(Points);

	TArray<FVector2D> TranslatedPoints;
	TranslatedPoints.Reserve(Points.Num());

	for (const FVector2D& Point : Points) {
		TranslatedPoints.Add(Point - Center);
	}

	return TranslatedPoints;
}

float UGlyphRecognizer::GreedyCloudMatch(const TArray<FVector2D>& PointsA, const TArray<FVector2D>& PointsB, float MinRotation, float MaxRotation, float RotationStep)
{
	if (PointsA.Num() != PointsB.Num()) {
		UE_LOG(LogTemp, Error, TEXT("GreedyCloudMatch: Point counts don't match!"));
		return FLT_MAX;
	}

	float BestDistance = FLT_MAX;

	for (float Angle = MinRotation; Angle <= MaxRotation; Angle += RotationStep) {
		TArray<FVector2D> RotatedPoints = RotateBy(PointsA, Angle);
		float Distance = CloudDistance(RotatedPoints, PointsB);

		if (Distance < BestDistance) {
			BestDistance = Distance;
		}
	}

	return BestDistance;
}


FGlyphMatchResult UGlyphRecognizer::RecognizeGlyph(const TArray<FVector2D>& DrawnPoints, const TArray<UGlyph*>& GlyphLibrary)
{
	FGlyphMatchResult Result;
	Result.IsMatch = false;
	Result.Score = FLT_MAX;

	if (DrawnPoints.Num() < 2 || GlyphLibrary.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("RecognizeGlyph: Invalid input (DrawnPoints=%d, Library=%d)"), DrawnPoints.Num(), GlyphLibrary.Num());

		return Result;
	}

	for (UGlyph* Glyph : GlyphLibrary) {
		if (!Glyph || Glyph->ModelPoints.Num() == 0) {
			continue;
		}

		TArray<FVector2D> ProcessedPoints = DrawnPoints;
		ProcessedPoints = Resample(ProcessedPoints, Glyph->NumResamplePoints);
		ProcessedPoints = ScaleToUnitSquare(ProcessedPoints);
		ProcessedPoints = TranslateToOrigin(ProcessedPoints);

		if (ProcessedPoints.Num() != Glyph->ModelPoints.Num()) {
			UE_LOG(LogTemp, Warning, TEXT("Glyph '%s': Point count mismatch (Drawn: %d vs Model: %d)"),
				*Glyph->GlyphName, ProcessedPoints.Num(), Glyph->ModelPoints.Num());

			continue;
		}

		float Distance = GreedyCloudMatch(ProcessedPoints, Glyph->ModelPoints, -45.0f, 45.0f, 2.0f);

		UE_LOG(LogTemp, Log, TEXT("Glyph '%s' distance: %f (threshold: %f)"), *Glyph->GlyphName, Distance, Glyph->RecognitionThreshold);

		if (Distance < Result.Score) {
			Result.Score = Distance;
			Result.MatchedGlyph = Glyph;
			Result.IsMatch = (Distance <= Glyph->RecognitionThreshold);
		}
	}

	if (Result.IsMatch) {
		UE_LOG(LogTemp, Log, TEXT("Best match: '%s' with score %f"), *Result.MatchedGlyph->GlyphName, Result.Score);
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("No match found. Best score: %f"), Result.Score);
	}

	return Result;
}

float UGlyphRecognizer::PathLength(const TArray<FVector2D>& Points)
{
	float Length = 0.0f;

	for (int32 i = 1; i < Points.Num(); ++i) {
		Length += FVector2D::Distance(Points[i - 1], Points[i]);
	}

	return Length;
}

FVector2D UGlyphRecognizer::Centroid(const TArray<FVector2D>& Points)
{
	FVector2D Sum = FVector2D::ZeroVector;

	for (const FVector2D& Point : Points) {
		Sum += Point;
	}

	return Sum / Points.Num();
}

TArray<FVector2D> UGlyphRecognizer::RotateBy(const TArray<FVector2D>& Points, float AngleDegrees)
{
	float AngleRadians = FMath::DegreesToRadians(AngleDegrees);
	float CosAngle = FMath::Cos(AngleRadians);
	float SinAngle = FMath::Sin(AngleRadians);

	TArray<FVector2D> RotatedPoints;
	RotatedPoints.Reserve(Points.Num());

	for (const FVector2D& Point : Points) {
		float NewX = Point.X * CosAngle - Point.Y * SinAngle;
		float NewY = Point.X * SinAngle + Point.Y * CosAngle;
		RotatedPoints.Add(FVector2D(NewX, NewY));
	}

	return RotatedPoints;
}

void UGlyphRecognizer::GetBoundingBox(const TArray<FVector2D>& Points, FVector2D& Min, FVector2D& Max)
{
	if (Points.Num() == 0) {
		Min = Max = FVector2D::ZeroVector;

		return;
	}

	Min = Points[0];
	Max = Points[0];

	for (const FVector2D& Point : Points) {
		Min.X = FMath::Min(Min.X, Point.X);
		Min.Y = FMath::Min(Min.Y, Point.Y);
		Max.X = FMath::Max(Max.X, Point.X);
		Max.Y = FMath::Max(Max.Y, Point.Y);
	}
}

float UGlyphRecognizer::CloudDistance(const TArray<FVector2D>& PointsA, const TArray<FVector2D>& PointsB)
{
	int32 NumPoints = PointsA.Num();
	float TotalDistance = 0.0f;

	TArray<bool> Matched;
	Matched.SetNumZeroed(NumPoints);

	for (int32 i = 0; i < NumPoints; ++i) {
		float MinDist = FLT_MAX;
		int32 BestMatch = -1;

		for (int32 j = 0; j < NumPoints; ++j) {
			if (!Matched[j]) {
				float Dist = FVector2D::Distance(PointsA[i], PointsB[j]);

				if (Dist < MinDist) {
					MinDist = Dist;
					BestMatch = j;
				}
			}
		}

		if (BestMatch >= 0) {
			Matched[BestMatch] = true;

			float Influence = 1.0f - (float)i / (NumPoints - 1);
			TotalDistance += Influence * MinDist;
		}
	}

	return TotalDistance;
}