// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FarmWorld : ModuleRules
{
	public FarmWorld(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate",
			"Chaos"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"FarmWorld",
			"FarmWorld/Variant_Platforming",
			"FarmWorld/Variant_Platforming/Animation",
			"FarmWorld/Variant_Combat",
			"FarmWorld/Variant_Combat/AI",
			"FarmWorld/Variant_Combat/Animation",
			"FarmWorld/Variant_Combat/Gameplay",
			"FarmWorld/Variant_Combat/Interfaces",
			"FarmWorld/Variant_Combat/UI",
			"FarmWorld/Variant_SideScrolling",
			"FarmWorld/Variant_SideScrolling/AI",
			"FarmWorld/Variant_SideScrolling/Gameplay",
			"FarmWorld/Variant_SideScrolling/Interfaces",
			"FarmWorld/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
