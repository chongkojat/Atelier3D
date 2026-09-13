using UnrealBuildTool;
using System.Collections.Generic;

public class Atelier3DTarget : TargetRules
{
	public Atelier3DTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

		ExtraModuleNames.AddRange(new string[] { "Atelier3D" });
	}
}
