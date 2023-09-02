#include "MatchMaker.h"

MatchMaker MatchMaker::instance;

void MatchMaker::create_room(HostID groupID) {
  instance.m_GroupRoomMap.insert(pair<HostID, Room>(groupID, Room()));
}

void MatchMaker::destroy_room(HostID groupID) {
  instance.m_GroupRoomMap.erase(groupID);
}

HostID MatchMaker::find_room_available() {
  for (auto it = instance.m_GroupRoomMap.begin();
       it != instance.m_GroupRoomMap.end(); it++) {
    if (it->second.size() < MAX_PLAYERS_PER_ROOM &&
        !it->second.is_game_started())
      return it->first;
  }
  return Proud::HostID_None;
}

HostID MatchMaker::find_room_joined(HostID hostID) {
  if (instance.m_HostGroupMap.find(hostID) == instance.m_HostGroupMap.end())
    throw "Host is not in any room2";
  return instance.m_HostGroupMap[hostID];
}

Room &MatchMaker::get_room_joined(HostID hostID) {
  if (instance.m_HostGroupMap.find(hostID) == instance.m_HostGroupMap.end())
    throw "Host is not in any room2";
  HostID groupID = instance.m_HostGroupMap[hostID];
  return instance.m_GroupRoomMap[groupID];
}

Room &MatchMaker::get_room_by_id(HostID groupID) {
  if (instance.m_GroupRoomMap.find(groupID) == instance.m_GroupRoomMap.end())
    throw "Room does not exist";
  return instance.m_GroupRoomMap[groupID];
}

void MatchMaker::join_room(HostID hostID, HostID groupID) {
  if (instance.m_GroupRoomMap.find(groupID) == instance.m_GroupRoomMap.end())
    throw "Room does not exist";
  if (instance.m_GroupRoomMap[groupID].size() >= MAX_PLAYERS_PER_ROOM)
    throw "Room is full";
  instance.m_GroupRoomMap[groupID].join(hostID);
  instance.m_HostGroupMap.insert(pair<HostID, HostID>(hostID, groupID));
}

void MatchMaker::leave_room(HostID hostID, HostID groupID) {
  instance.m_GroupRoomMap[groupID].leave(hostID);
  instance.m_HostGroupMap.erase(hostID);
}
