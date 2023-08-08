#pragma once

#include "../Patterns/Singleton.h"
#include "Room.h"
#include <map>
#include <set>

using namespace std;

class MatchMaker final : public Singleton<MatchMaker>
{
public:
    static void create_room(HostID groupID);
    static void destroy_room(HostID groupID);
    static HostID find_room_available();
    static HostID find_room_joined(HostID hostID);
    static Room& get_room_joined(HostID hostID);
    static Room& get_room_by_id(HostID groupID);
    static void join_room(HostID hostID, HostID groupID);
    static void leave_room(HostID hostID, HostID groupID);
private:
    map<HostID, Room> m_GroupRoomMap;
    map<HostID, HostID> m_HostGroupMap;
};