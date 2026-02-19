#include "GlyphFactory.h"
#include "Glyph.h"

UGlyphFactory::UGlyphFactory()
{
    SupportedClass = UGlyph::StaticClass();
    bCreateNew = true;
    bEditAfterNew = true;
}

UObject* UGlyphFactory::FactoryCreateNew(
    UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
    check(InClass->IsChildOf(UGlyph::StaticClass()));
    return NewObject<UGlyph>(InParent, InClass, InName, Flags);
}