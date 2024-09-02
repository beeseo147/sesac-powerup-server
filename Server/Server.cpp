﻿#include "Server.h"
#include "../Common/Powerup_common.cpp"
#include "../Common/Powerup_proxy.cpp"
#include "../Common/Powerup_stub.cpp"
#include <chrono>
#include <thread>
#include <utility>

CriticalSection g_critSec;
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
// map<HostID, Proud::Thread *> map_groupID_thread;
map<HostID, bool> map_groupID_on;
vector<Thread *> games;
queue<ThreadDAO> roomReadyInfo;

int main(int argc, char *argv[]) {
  // 서버 인스턴스 생성
  shared_ptr<CNetServer> server(CNetServer::Create());

  // 클라이언트 이동 동작 이벤트 처리 함수
  g_GameStub.EnterRoom_Function = [&] PARAM_PowerupC2S_EnterRoom {
    CriticalSectionLock lock(g_critSec, true);
    cout << "EnterRoom(): Start\n";
    if (HostID_None != MatchMaker::find_room_joined(remote))
      return true;

    HostID groupID = MatchMaker::find_room_available();

    if (groupID == HostID_None) {
      groupID = server->CreateP2PGroup();
      MatchMaker::create_room(groupID);
    }
    server->JoinP2PGroup(remote, groupID);
    MatchMaker::join_room(remote, groupID);

    Room &room = MatchMaker::get_room_joined(remote);
    int player_no = room.get_player_no(remote);

    g_GameProxyClient.PlayerEnter(remote, RmiContext::ReliableSend, player_no);
    g_GameProxyGroup.PlayersReady(groupID, RmiContext::ReliableSend,
                                  room.get_players_ready());

    cout << "EnterRoom(): End\n";
    return true;
  };

  // 클라이언트 이동 동작 이벤트 처리 함수
  g_GameStub.ExitRoom_Function = [&] PARAM_PowerupC2S_ExitRoom {
    CriticalSectionLock lock(g_critSec, true);
    cout << "ExitRoom(): Start\n";
    HostID groupID = MatchMaker::find_room_joined(remote);

    if (groupID == HostID_None)
      return true;

    MatchMaker::leave_room(remote, groupID);
    server->LeaveP2PGroup(remote, groupID);

    Room &room = MatchMaker::get_room_by_id(groupID);

    g_GameProxyClient.PlayerExit(remote, RmiContext::ReliableSend, true);

    if (room.is_game_started() && room.get_players_rank().size() < 2) {
      map_groupID_on[groupID] = false;
      return true;
    }

    if (room.is_all_players_ready()) {
      ThreadDAO threadDAO;
      threadDAO.groupID = groupID;
      threadDAO.roomPtr = &room;

      roomReadyInfo.push(threadDAO);
    }

    g_GameProxyGroup.PlayersReady(groupID, RmiContext::ReliableSend,
                                  room.get_players_ready());

    cout << "ExitRoom(): End\n";
    return true;
  };

  g_GameStub.GetReady_Function = [&] PARAM_PowerupC2S_GetReady {
    CriticalSectionLock lock(g_critSec, true);
    cout << "GetReadyRoom(): Start\n";
    HostID groupID = MatchMaker::find_room_joined(remote);
    if (HostID_None == groupID)
      return true;

    Room &room = MatchMaker::get_room_joined(remote);

    room.set_player_ready(remote, isReady);

    if (room.is_all_players_ready()) {
      ThreadDAO threadDAO;
      threadDAO.groupID = groupID;
      threadDAO.roomPtr = &room;

      roomReadyInfo.push(threadDAO);
    }

    g_GameProxyGroup.PlayersReady(
        groupID, RmiContext::ReliableSend,
        MatchMaker::get_room_joined(remote).get_players_ready());

    cout << "GetReadyRoom(): End\n";
    return true;
  };

  g_GameStub.HasPoint_Function = [&] PARAM_PowerupC2S_HasPoint {
    CriticalSectionLock lock(g_critSec, true);
    cout << "HasPoint(): Start\n";
    HostID groupID = MatchMaker::find_room_joined(remote);
    if (HostID_None == groupID)
      return true;

    Room &room = MatchMaker::get_room_joined(remote);

    room.set_player_point(remote, point);
    map<int32_t, int32_t> playersRank = room.get_players_rank();

    g_GameProxyGroup.PlayersRank(groupID, RmiContext::ReliableSend,
                                 playersRank);

    cout << "HasPoint(): End\n";
    return true;
  };

  g_GameStub.Move_Function = [&] PARAM_PowerupC2S_Move {
    CriticalSectionLock lock(g_critSec, true);
    cout << "Move(): Start\n";
    Room &room = MatchMaker::get_room_joined(remote);

    int32_t player_no = room.get_player_no(remote);
    g_GameProxyGroup.PlayerMove(remote, RmiContext::ReliableSend, player_no,
                                key, enemies);

    cout << "Move(): End\n";
    return true;
  };

  // set a routine which is executed when a client is joining.
  // clientInfo has the client player including its HostID.
  server->OnClientJoin = [&server](CNetClientInfo *clientInfo) {
    CriticalSectionLock lock(g_critSec, true);
    cout << "Client " << clientInfo->m_HostID << " Joined\n";
  };

  // set a routine for client leave event.
  server->OnClientLeave = [&server](CNetClientInfo *clientInfo,
                                    ErrorInfo *errorInfo,
                                    const ByteArray &comment) {
    CriticalSectionLock lock(g_critSec, true);
    cout << "Client " << clientInfo->m_HostID << " Leaved\n";
  };

  // Associate RMI proxy and stub instances to network object.
  server->AttachProxy(&g_GameProxyClient);
  server->AttachProxy(&g_GameProxyGroup);
  server->AttachStub(&g_GameStub);

  CStartServerParameter server_param;
  server_param.m_protocolVersion =
      g_Version; // This must be the same to the client.
  server_param.m_tcpPorts.Add(g_ServerPort); // TCP listening endpoint
  server_param.m_allowServerAsP2PGroupMember = true;

  ErrorInfoPtr err;
  try {
    server->Start(server_param);
  } catch (Exception &e) {
    cout << "Server start failed: " << e.what() << endl;
    return 0;
  }

  while (true) {
    if (roomReadyInfo.size()) {
      ThreadDAO threadDAO = roomReadyInfo.front();
      roomReadyInfo.pop();

      Thread *workerThread = new Thread([&]() {
        cout << "thread start\n";
        int32_t time_remain = 60;

        threadDAO.roomPtr->start_game();
        g_GameProxyGroup.GameStart(threadDAO.groupID, RmiContext::ReliableSend);
        map_groupID_on[threadDAO.groupID] = true;

        while (map_groupID_on[threadDAO.groupID] && time_remain) {
          Proud::Sleep(1000);

          time_remain--;
          g_GameProxyGroup.TimeNow(threadDAO.groupID, RmiContext::ReliableSend,
                                   time_remain);
        }

        g_GameProxyGroup.GameEnd(threadDAO.groupID, RmiContext::ReliableSend);
        threadDAO.roomPtr->reset();
        cout << "thread end\n";
      });

      workerThread->Start();
      games.push_back(workerThread);
    }
  }

  for (auto thread : games) {
    thread->Join();
  }

  return 0;
}
