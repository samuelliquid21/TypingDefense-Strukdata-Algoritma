#pragma once
#include "DataManager.h"

// Panel debug yang menampilkan info statistik player di pojok kanan bawah.
// Visibility dikontrol oleh Config::enableDebugPlayerInfo.
void DrawPlayerInfoPanel(const PlayerProfile& profile);
