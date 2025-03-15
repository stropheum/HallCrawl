// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HallCrawl : ModuleRules
{
	public HallCrawl(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PrivatePCHHeaderFile = "HallCrawlPrivatePCH.h";
		
		bUseUnity = false;
		bUsePrecompiled = true;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "GameplayTags", "GameplayTasks"
		});
	}
}
