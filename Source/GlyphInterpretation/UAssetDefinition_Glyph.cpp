#include "UAssetDefinition_Glyph.h"
#include "Glyph.h"

FText UUAssetDefinition_Glyph::GetAssetDisplayName() const
{
	return FText::FromString(TEXT("Glyph"));
}

FLinearColor UUAssetDefinition_Glyph::GetAssetColor() const
{
	return FLinearColor(FColor::Red);
}

TSoftClassPtr<UObject> UUAssetDefinition_Glyph::GetAssetClass() const
{
	return UGlyph::StaticClass();
}

TConstArrayView<FAssetCategoryPath> UUAssetDefinition_Glyph::GetAssetCategories() const
{
	static const auto Categories = {
		FAssetCategoryPath(FText::FromName("Glyph")),
	};

	return Categories;
}