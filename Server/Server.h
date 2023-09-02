#pragma once

#include "Manage\MatchMaker.cpp"
#include "Manage\MatchMaker.h"
#include "stdafx.h"


#include "../../ProudNet/include/ProudNetServer.h"

#include "../Common/Vars.h"

using namespace std;
using namespace Proud;

bool start_game_thread(HostID groupID, Room &room);