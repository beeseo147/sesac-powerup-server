#include "Server.h"
#include <stdint.h>
#include <stdlib.h>

// Client-to-server RMI stub
PowerupC2S::StubFunctional g_GameStub;

// Server-to-client RMI proxy
PowerupS2G::Proxy g_GameProxyGroup;
PowerupS2C::Proxy g_GameProxyClient;

// 게임 데이터 관리 클래스로 분리 필요
// bool game_started = false;
// set<int> set_player;
// map<HostID, int> map_HostID_player;
// map<int, Player> map_player_Player;
// map<int, set<Item>> map_player_Items;

int main(int argc, char *argv[]) {
  // 서버 인스턴스 생성
  shared_ptr<CNetServer> server(CNetServer::Create());

  // 클라이언트 이동 동작 이벤트 처리 함수
  g_GameStub.EnterRoom_Function = [&] PARAM_PowerupC2S_EnterRoom {
    HostID groupID = MatchMaker::find_room_available();
    
    if (groupID == HostID_None) {
      groupID = server->CreateP2PGroup();
      MatchMaker::create_room(groupID);
    }

    server->JoinP2PGroup(remote, groupID);
    MatchMaker::join_room(remote, groupID);
    
    Room room = MatchMaker::get_room_joined(remote);
    
    g_GameProxyClient.PlayerEnter(remote, RmiContext::ReliableSend, true);
    g_GameProxyGroup.PlayersReady(groupID, RmiContext::ReliableSend, room.get_players_ready());
    
    return true;
  };

  // 클라이언트 이동 동작 이벤트 처리 함수
  g_GameStub.ExitRoom_Function = [&] PARAM_PowerupC2S_ExitRoom {
    HostID groupID = MatchMaker::find_room_joined(remote);
    MatchMaker::leave_room(remote, groupID);
    server->LeaveP2PGroup(remote, groupID);
    
    Room room = MatchMaker::get_room_by_id(groupID);
    
    g_GameProxyClient.PlayerExit(remote, RmiContext::ReliableSend, true);
    
    if (start_game_thread(groupID, room)) 
      return true;

    g_GameProxyGroup.PlayersReady(groupID, RmiContext::ReliableSend, room.get_players_ready());

    return true;
  };

  g_GameStub.GetReady_Function = [&] PARAM_PowerupC2S_GetReady {
    HostID groupID = MatchMaker::find_room_joined(remote);
    Room room = MatchMaker::get_room_joined(remote);
    
    room.set_player_ready(remote, true);
    
    if (start_game_thread(groupID, room)) 
      return true;

    g_GameProxyGroup.PlayersReady(groupID, RmiContext::ReliableSend, MatchMaker::get_room_joined(remote).get_players_ready());
    
    return true;
  };

  g_GameStub.HasPoint_Function = [&] PARAM_PowerupC2S_HasPoint {
    HostID groupID = MatchMaker::find_room_joined(remote);
    Room room = MatchMaker::get_room_joined(remote);

    room.set_player_point(remote, point);
    map<int32_t, int32_t> playersRank = room.get_players_rank();

    g_GameProxyGroup.PlayersRank(groupID, RmiContext::ReliableSend, playersRank);
    
    return true;
  };

  g_GameStub.Move_Function = [&] PARAM_PowerupC2S_Move {
    Room room = MatchMaker::get_room_joined(remote);

    int32_t player_no = room.get_player_no(remote);
    g_GameProxyGroup.PlayerMove(remote, RmiContext::ReliableSend, player_no, key, enemies);
    
    return true;
  };

  // set a routine which is executed when a client is joining.
  // clientInfo has the client player including its HostID.
  server->OnClientJoin = [&server](CNetClientInfo *clientInfo) {

  };

  // set a routine for client leave event.
  server->OnClientLeave = [&server](CNetClientInfo *clientInfo,
                                    ErrorInfo *errorInfo,
                                    const ByteArray &comment) {
    
  };

  // Associate RMI proxy and stub instances to network object.
  server->AttachProxy(&g_GameProxyClient);
  server->AttachProxy(&g_GameProxyGroup);
  server->AttachStub(&g_GameStub);

  CStartServerParameter server_param;
  server_param.m_protocolVersion = g_Version; // This must be the same to the client.
  server_param.m_tcpPorts.Add(g_ServerPort);  // TCP listening endpoint
  server_param.m_allowServerAsP2PGroupMember = true;

  ErrorInfoPtr err;
  try {
    server->Start(server_param);
  } catch (Exception &e) {
    cout << "Server start failed: " << e.what() << endl;
    return 0;
  }

  while (true) {
    ;
  }

  return 0;
}

bool start_game_thread(HostID groupID, Room& room)
{
  if (room.is_all_players_ready()) {
    room.start_game();
    g_GameProxyGroup.GameStart(groupID, RmiContext::ReliableSend);
    
    Thread workerThread([&](){
      int32_t time_remain = 60;
      while (time_remain)
      {
        Proud::Sleep(1000);
        time_remain--;
        g_GameProxyGroup.TimeNow(groupID, RmiContext::ReliableSend, time_remain);
      }
      g_GameProxyGroup.GameEnd(groupID, RmiContext::ReliableSend);
      room.reset();
    });
    return true;
  }

  return false;
}