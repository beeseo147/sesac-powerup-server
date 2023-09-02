#pragma once

#include "../Patterns/Singleton.cpp"
#include "../Patterns/Singleton.h"
#include "Room.cpp"
#include "Room.h"
#include <map>
#include <set>

using namespace std;

class MatchMaker {
public:
  static void create_room(HostID groupID);
  static void destroy_room(HostID groupID);
  static HostID find_room_available();
  static HostID find_room_joined(HostID hostID);
  static Room &get_room_joined(HostID hostID);
  static Room &get_room_by_id(HostID groupID);
  static void join_room(HostID hostID, HostID groupID);
  static void leave_room(HostID hostID, HostID groupID);
  MatchMaker() {
    map<HostID, Room> m_GroupRoomMap;
    map<HostID, HostID> m_HostGroupMap;
  }

private:
  static MatchMaker instance;
  map<HostID, Room> m_GroupRoomMap;
  map<HostID, HostID> m_HostGroupMap;
};