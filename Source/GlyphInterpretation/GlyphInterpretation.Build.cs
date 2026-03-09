// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GlyphInterpretation : ModuleRules
{
	public GlyphInterpretation(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AssetDefinition",
			"UnrealEd",
			"UMG",
            "Slate",
            "SlateCore",
            "PropertyEditor",
			"EditorStyle"
        });
	}
}
