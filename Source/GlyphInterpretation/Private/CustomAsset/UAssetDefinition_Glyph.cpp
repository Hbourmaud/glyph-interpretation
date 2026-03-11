#if WITH_EDITOR

#include "CustomAsset/UAssetDefinition_Glyph.h"
#include "CustomAsset/GlyphAssetEditor.h"
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

EAssetCommandResult UUAssetDefinition_Glyph::OpenAssets(const FAssetOpenArgs& OpenArgs) const
{
	for (UGlyph* Glyph : OpenArgs.LoadObjects<UGlyph>()) {
		TSharedRef<FGlyphAssetEditor> NewEditor = MakeShared<FGlyphAssetEditor>();

		NewEditor->InitGlyphEditor(OpenArgs.GetToolkitMode(), OpenArgs.ToolkitHost, Glyph);
	}

	return EAssetCommandResult::Handled;
}

#endif // WITH_EDITOR