#if WITH_EDITOR

#include "CustomAsset/GlyphAssetEditor.h"
#include "CustomAsset/GlyphDrawingPanel.h"
#include "Glyph.h"
#include "Widgets/Docking/SDockTab.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "GlyphAssetEditor"

const FName FGlyphAssetEditor::PropertiesTabId(TEXT("GlyphEditor_Properties"));
const FName FGlyphAssetEditor::DrawingTabId(TEXT("GlyphEditor_Drawing"));

FGlyphAssetEditor::FGlyphAssetEditor()
    : GlyphAsset(nullptr)
{
}

FGlyphAssetEditor::~FGlyphAssetEditor() {}

void FGlyphAssetEditor::InitGlyphEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UGlyph* InGlyph)
{
	GlyphAsset = InGlyph;

	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_GlyphEditor_Layout_v1")
		->AddArea
		(
			FTabManager::NewPrimaryArea()->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.3f)
				->AddTab(PropertiesTabId, ETabState::OpenedTab)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.7f)
				->AddTab(DrawingTabId, ETabState::OpenedTab)
			)
		);

	const bool IsCreateDefaultMenu = true;
	const bool IsCreateDefaultToolbar = true;

	FAssetEditorToolkit::InitAssetEditor(
		Mode,
		InitToolkitHost,
		FName("GlyphEditorApp"),
		StandaloneDefaultLayout,
		IsCreateDefaultMenu,
		IsCreateDefaultToolbar,
		InGlyph
	);
}

void FGlyphAssetEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_GlyphEditor", "Glyph Editor"));

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	InTabManager->RegisterTabSpawner(PropertiesTabId, FOnSpawnTab::CreateSP(this, &FGlyphAssetEditor::SpawnPropertiesTab))
		.SetDisplayName(LOCTEXT("PropertiesTab", "Details"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef());

	InTabManager->RegisterTabSpawner(DrawingTabId, FOnSpawnTab::CreateSP(this, &FGlyphAssetEditor::SpawnDrawingTab))
		.SetDisplayName(LOCTEXT("DrawingTab", "Draw Model"))
		.SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void FGlyphAssetEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);

	InTabManager->UnregisterTabSpawner(PropertiesTabId);
	InTabManager->UnregisterTabSpawner(DrawingTabId);
}

TSharedRef<SDockTab> FGlyphAssetEditor::SpawnPropertiesTab(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == PropertiesTabId);

	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

	TSharedRef<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);

	DetailsView->SetObject(GlyphAsset);

	return SNew(SDockTab).Label(LOCTEXT("PropertiesTitle", "Details"))[DetailsView];
}

TSharedRef<SDockTab> FGlyphAssetEditor::SpawnDrawingTab(const FSpawnTabArgs& Args)
{
	check(Args.GetTabId() == DrawingTabId);

	DrawingPanel = SNew(SGlyphDrawingPanel, GlyphAsset);

	return SNew(SDockTab).Label(LOCTEXT("DrawingTitle", "Draw Model"))[DrawingPanel.ToSharedRef()];
}

FName FGlyphAssetEditor::GetToolkitFName() const
{
	return FName("GlyphEditor");
}

FText FGlyphAssetEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "Glyph Editor");
}

FString FGlyphAssetEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Glyph ").ToString();
}

FLinearColor FGlyphAssetEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f);
}

#undef LOCTEXT_NAMESPACE

#endif // WITH_EDITOR