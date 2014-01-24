#ifndef HANDLERS_H
#define HANDLERS_H

#include "DefaultHandlers.h"

namespace Windows
{
    class tControl;
}

int SpawnSettingsDialog(Windows::tControl*, PVOID); // command handler for settings
int SpawnFileBrowser(Windows::tControl*, PVOID);
int RunDemo(Windows::tControl*, PVOID);
int StopDemo(Windows::tControl*, PVOID);
int SaveSettings(Windows::tControl*, PVOID);

#endif