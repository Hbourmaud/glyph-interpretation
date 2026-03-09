#pragma once

#include "CoreMinimal.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Widgets/Docking/SDockTab.h"

class UGlyph;
class SGlyphDrawingPanel;

class FGlyphAssetEditor : public FAssetEditorToolkit
{
public:
    FGlyphAssetEditor();
    virtual ~FGlyphAssetEditor();

    void InitGlyphEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UGlyph* InGlyph);

    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;

    virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

private:
    UGlyph* GlyphAsset;

    static const FName PropertiesTabId;
    static const FName DrawingTabId;

    TSharedRef<SDockTab> SpawnPropertiesTab(const FSpawnTabArgs& Args);
    TSharedRef<SDockTab> SpawnDrawingTab(const FSpawnTabArgs& Args);

    TSharedPtr<SGlyphDrawingPanel> DrawingPanel;
};
