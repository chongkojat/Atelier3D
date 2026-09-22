using UnrealBuildTool;

public class LayoutCore : ModuleRules
{
	public LayoutCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.NoPCHs;
		bUseUnity = false;

		// Kept deliberately thin: only the module-loading glue (LayoutCoreModule.cpp)
		// touches Core. RoomLayout.h/.cpp must stay standard C++ so this module keeps
		// compiling and testing headless outside Unreal. See docs/architecture.md.
		PublicDependencyModuleNames.AddRange(new string[] { "Core" });
	}
}
