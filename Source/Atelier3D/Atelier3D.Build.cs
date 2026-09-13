using UnrealBuildTool;

public class Atelier3D : ModuleRules
{
	public Atelier3D(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core", "CoreUObject", "Engine", "InputCore", "UMG", "Slate", "SlateCore"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {
			"LayoutCore"
		});
	}
}
