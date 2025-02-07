// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SpartaMyProject : ModuleRules
{
	public SpartaMyProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"UMG",
            "AdvancedWidgets"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
	}
}
