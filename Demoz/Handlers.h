#ifndef HANDLERS_H
#define HANDLERS_H

#include "DefaultHandlers.h"

namespace Windows
{
    class tControl;
}

int SpawnSettingsDialog(Windows::tControl*); // command handler for settings
int SpawnFileBrowser(Windows::tControl*);

#endif