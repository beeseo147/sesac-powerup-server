#pragma once

#include "../../../ProudNet/include/ProudNetCommon.h"
#include "../../Common/Vars.h"
#include <stdint.h>

using namespace Proud;
using namespace std;

class Room
{
public:
    Room();
    size_t size() const;
    void join(HostID hostID);
    int32_t find_empty_player_no() const;
    void leave(HostID hostID);
    void set_player_ready(HostID hostID, bool ready);
    void set_player_point(HostID hostID, int32_t point);
    int32_t get_player_no(HostID hostID) const;
    map<int32_t, bool> get_players_ready() const;
    map<int32_t, int32_t> get_players_rank() const;
    bool is_game_started() const;
    bool is_all_players_ready() const;
    void reset();
    void start_game();
    void end_game();
private:
    map<HostID, int32_t> m_HostMap;
    map<int32_t, bool> m_PlayersReady;
    map<int32_t, int32_t> m_PlayersPoint;
    bool m_GameStarted = false;
};