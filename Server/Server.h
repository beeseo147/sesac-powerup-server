#pragma once

#include "stdafx.h"
#include "Manage\MatchMaker.h"

#include "../../ProudNet/include/ProudNetServer.h"
#include "../Common/Vars.h"

using namespace std;
using namespace Proud;

bool start_game_thread(HostID groupID, Room& room);