#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "GlyphFactory.generated.h"

UCLASS()
class GLYPHINTERPRETATION_API UGlyphFactory : public UFactory
{
	GENERATED_BODY()
	
	public:
		UGlyphFactory();

		virtual UObject* FactoryCreateNew(
			UClass* InClass,
			UObject* InParent,
			FName InName,
			EObjectFlags Flags,
			UObject* Context,
			FFeedbackContext* Warn,
			FName CallingContext
		) override;
};
