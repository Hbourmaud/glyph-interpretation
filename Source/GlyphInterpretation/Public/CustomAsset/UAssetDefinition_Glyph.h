#pragma once

#include "CoreMinimal.h"
#include "AssetDefinitionDefault.h"
#include "UAssetDefinition_Glyph.generated.h"

UCLASS()
class GLYPHINTERPRETATION_API UUAssetDefinition_Glyph : public UAssetDefinitionDefault
{
	GENERATED_BODY()
	
	protected:
		virtual FText GetAssetDisplayName() const override;
		virtual FLinearColor GetAssetColor() const override;
		virtual TSoftClassPtr<UObject> GetAssetClass() const override;
		virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
};
