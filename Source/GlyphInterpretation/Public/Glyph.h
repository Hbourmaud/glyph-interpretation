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

		UPROPERTY(EditAnywhere, Category = "Data")
		FString GlyphName;
};
