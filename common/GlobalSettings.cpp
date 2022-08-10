#include "GlobalSettings.h"
#include "Log.h"

#ifdef _WIN32
#if USE_VISUAL_LEAK_DETECTOR
#include "vld.h"
#endif
#endif

GlobalSettings::GlobalSettings()
{
}

GlobalSettings::~GlobalSettings()
{
}

void GlobalSettings::Print()
{
	LOG("[GlobalSettings]\n");

#if defined(_WIN32)
	LOG("window extent: %d x %d\n", static_cast<int>(windowWidth), static_cast<int>(windowHeight));
#endif

	LOG("\n");
}

GlobalSettings gGlobalSettings;

GlobalSettings & GetGlobalSettings()
{
	return gGlobalSettings;
}
