// Module-loading glue only. Keep this the sole file in LayoutCore that touches
// Unreal headers — see LayoutCore.Build.cs.
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FDefaultModuleImpl, LayoutCore);
