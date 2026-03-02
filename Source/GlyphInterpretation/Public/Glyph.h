#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Glyph.generated.h"

/**
 * 
 */
UCLASS()
class GLYPHINTERPRETATION_API UGlyph : public UObject
{
	GENERATED_BODY()
	
	public:
		UGlyph();

		UPROPERTY(EditAnywhere, Category = "Glyph")
		FString GlyphName;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glyph")
		TArray<FVector2D> ModelPoints;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glyph", meta = (ClampMin = "16", ClampMax = "128"))
		int32 NumResamplePoints = 32;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glyph", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float RecognitionThreshold = 0.3f;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Glyph Spawn")
		TSubclassOf<AActor> ActorToSpawn;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Glyph Spawn")
		FVector SpawnOffset = FVector::ZeroVector;
};
