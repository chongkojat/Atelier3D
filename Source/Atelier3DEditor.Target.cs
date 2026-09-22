using UnrealBuildTool;
using System.Collections.Generic;

public class Atelier3DEditorTarget : TargetRules
{
	public Atelier3DEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[] { "Atelier3D" });
	}
}
