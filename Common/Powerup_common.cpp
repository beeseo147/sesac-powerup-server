﻿
// Generated by PIDL compiler.
// Do not modify this file, but modify the source .pidl file.


#include "Powerup_common.h"
namespace PowerupS2G {


	::Proud::RmiID g_RmiIDList[] = {
               
		Rmi_GameStart,
               
		Rmi_GameEnd,
               
		Rmi_PlayersReady,
               
		Rmi_TimeNow,
               
		Rmi_PlayerMove,
               
		Rmi_PlayersRank,
	};

	int g_RmiIDListCount = 6;

}


namespace PowerupS2C {


	::Proud::RmiID g_RmiIDList[] = {
               
		Rmi_PlayerEnter,
               
		Rmi_PlayerExit,
	};

	int g_RmiIDListCount = 2;

}


namespace PowerupC2S {


	::Proud::RmiID g_RmiIDList[] = {
               
		Rmi_EnterRoom,
               
		Rmi_ExitRoom,
               
		Rmi_GetReady,
               
		Rmi_Move,
               
		Rmi_HasPoint,
	};

	int g_RmiIDListCount = 5;

}





