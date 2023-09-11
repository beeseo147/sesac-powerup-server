#pragma once

#include "../../ProudNet/include/ProudNetCommon.h"
#include "Powerup_marshaler.cpp"
#include <map>
#include <vector>

namespace PowerupS2G {

// Message ID that replies to each RMI method.

static const ::Proud::RmiID Rmi_GameStart = (::Proud::RmiID)(2000 + 1);

static const ::Proud::RmiID Rmi_GameEnd = (::Proud::RmiID)(2000 + 2);

static const ::Proud::RmiID Rmi_PlayersReady = (::Proud::RmiID)(2000 + 3);

static const ::Proud::RmiID Rmi_TimeNow = (::Proud::RmiID)(2000 + 4);

static const ::Proud::RmiID Rmi_PlayerMove = (::Proud::RmiID)(2000 + 5);

static const ::Proud::RmiID Rmi_PlayersRank = (::Proud::RmiID)(2000 + 6);

// List that has RMI ID.
extern ::Proud::RmiID g_RmiIDList[];
// RmiID List Count
extern int g_RmiIDListCount;

} // namespace PowerupS2G

namespace PowerupS2C {

// Message ID that replies to each RMI method.

static const ::Proud::RmiID Rmi_PlayerEnter = (::Proud::RmiID)(2100 + 1);

static const ::Proud::RmiID Rmi_PlayerExit = (::Proud::RmiID)(2100 + 2);

// List that has RMI ID.
extern ::Proud::RmiID g_RmiIDList[];
// RmiID List Count
extern int g_RmiIDListCount;

} // namespace PowerupS2C

namespace PowerupC2S {

// Message ID that replies to each RMI method.

static const ::Proud::RmiID Rmi_EnterRoom = (::Proud::RmiID)(2200 + 1);

static const ::Proud::RmiID Rmi_ExitRoom = (::Proud::RmiID)(2200 + 2);

static const ::Proud::RmiID Rmi_GetReady = (::Proud::RmiID)(2200 + 3);

static const ::Proud::RmiID Rmi_Move = (::Proud::RmiID)(2200 + 4);

static const ::Proud::RmiID Rmi_HasPoint = (::Proud::RmiID)(2200 + 5);

// List that has RMI ID.
extern ::Proud::RmiID g_RmiIDList[];
// RmiID List Count
extern int g_RmiIDListCount;

} // namespace PowerupC2S

// Forward declarations

// Declarations
