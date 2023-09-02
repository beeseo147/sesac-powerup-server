#include "stdafx.h"

#include "../../ProudNet/include/ProudNetClient.h"
#include <stdint.h>

using namespace std;
using namespace Proud;

#include "../Common/Powerup_common.cpp"
#include "../Common/Powerup_proxy.cpp"
#include "../Common/Powerup_stub.cpp"
#include "../Common/Vars.h"

/* Protects all variable here.
If you are developing a game program or an app with fast-running loop,
this is not needed, as you are making a single threaded app.
This is required as this sample program uses two threads.
One is for waiting user input, the other is for processing
received messages and events. */
CriticalSection g_critSec;

// Client-to-server RMI stub
PowerupC2S::Proxy g_GameProxy;

// Server-to-client RMI proxy
PowerupS2G::StubFunctional g_GameStubGroup;
PowerupS2C::StubFunctional g_GameStubClient;

std::vector<int32_t> enemies;

void printMap(const std::vector<int32_t> enemies) {
  int ix = 0;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      cout << enemies[ix++] << " ";
    }
    cout << "\n";
  }
}

template <typename T> void print_map(map<int32_t, T> map_to_print) {
  cout << "print_map"
       << "\n";
  for (pair<int32_t, T> pair : map_to_print) {
    int32_t player_no = pair.first;
    T value = pair.second;

    cout << player_no << ": " << value << "\n";
  }
}

int main(int argc, char *argv[]) {
  // Network client object.
  shared_ptr<CNetClient> netClient(CNetClient::Create());

  // set to true if server connection is established.
  bool isConnected = false;
  // changed if another P2P peer joined.
  HostID recentP2PGroupHostID = HostID_None;

  volatile bool keepWorkerThread = true;

  // set a routine which is run when the server connection attempt
  // is success or failed.
  netClient->OnJoinServerComplete = [&](ErrorInfo *info,
                                        const ByteArray &replyFromServer) {
    CriticalSectionLock lock(g_critSec, true);
    if (info->m_errorType == ErrorType_Ok) {
      // connection successful.
      printf("Succeed to connect server. Allocated hostID=%d\n",
             netClient->GetLocalHostID());
      isConnected = true;

    } else {
      // connection failure.
      cout << (int)info->m_errorType << "\n";
    }
  };

  // set a routine for network disconnection.
  netClient->OnLeaveServer = [&](ErrorInfo *errorInfo) {
    CriticalSectionLock lock(g_critSec, true);
    cout << "OnLeaveServer: " << StringT2A(errorInfo->m_comment).GetString()
         << endl;

    // let main loop exit
    isConnected = false;
  };

  // set a routine for P2P member join (P2P available)
  netClient->OnP2PMemberJoin = [&](HostID memberHostID, HostID groupHostID,
                                   int memberCount,
                                   const ByteArray &customField) {
    CriticalSectionLock lock(g_critSec, true);
    printf("[Client] P2P member %d joined group %d.\n", memberHostID,
           groupHostID);

    recentP2PGroupHostID = groupHostID;
  };

  // called when a P2P member left.
  netClient->OnP2PMemberLeave = [](HostID memberHostID, HostID groupHostID,
                                   int memberCount) {
    CriticalSectionLock lock(g_critSec, true);
    printf("[Client] P2P member %d left group %d.\n", memberHostID,
           groupHostID);
  };

  g_GameStubClient.PlayerEnter_Function = [&] PARAM_PowerupS2C_PlayerEnter {
    CriticalSectionLock lock(g_critSec, true);
    cout << "player Entered to " << (int)remote << "\n";
    return true;
  };

  g_GameStubClient.PlayerExit_Function = [&] PARAM_PowerupS2C_PlayerExit {
    CriticalSectionLock lock(g_critSec, true);
    cout << "player Exited to " << (int)remote << "\n";
    return true;
  };

  g_GameStubGroup.GameStart_Function = [&] PARAM_PowerupS2G_GameStart {
    CriticalSectionLock lock(g_critSec, true);
    cout << "Game started\n";
    return true;
  };

  g_GameStubGroup.GameEnd_Function = [&] PARAM_PowerupS2G_GameEnd {
    CriticalSectionLock lock(g_critSec, true);
    cout << "Game ended\n";
    return true;
  };

  g_GameStubGroup.PlayerMove_Function = [&] PARAM_PowerupS2G_PlayerMove {
    CriticalSectionLock lock(g_critSec, true);
    cout << "Player " << playerNo << " moved toward " << key << "\n";
    printMap(enemies);
    return true;
  };
  g_GameStubGroup.PlayersRank_Function = [&] PARAM_PowerupS2G_PlayersRank {
    CriticalSectionLock lock(g_critSec, true);
    print_map<int32_t>(playersRank);
    return true;
  };
  g_GameStubGroup.PlayersReady_Function = [&] PARAM_PowerupS2G_PlayersReady {
    CriticalSectionLock lock(g_critSec, true);
    print_map<bool>(playersReady);
    return true;
  };

  g_GameStubGroup.TimeNow_Function = [&] PARAM_PowerupS2G_TimeNow {
    CriticalSectionLock lock(g_critSec, true);
    cout << ticksRemain << " time remained\n";
    return true;
  };

  // attach RMI proxy and stub to client object.
  netClient->AttachProxy(&g_GameProxy);
  netClient->AttachStub(&g_GameStubGroup);
  netClient->AttachStub(&g_GameStubClient);

  CNetConnectionParam cp;

  // Protocol version which have to be same to the value at the server.
  cp.m_protocolVersion = g_Version;
  cp.m_closeNoPingPongTcpConnections = false;

  // server address
  if (argc > 1) {
    cp.m_serverIP = StringA2T(argv[1]);
  } else {
    cp.m_serverIP = _PNT("localhost");
  }

  enemies.push_back(1);
  enemies.push_back(2);
  enemies.push_back(3);
  enemies.push_back(4);
  enemies.push_back(5);
  enemies.push_back(6);
  enemies.push_back(7);
  enemies.push_back(8);
  enemies.push_back(9);

  // server port
  cp.m_serverPort = g_ServerPort;

  netClient->Connect(cp);

  Proud::Thread workerThread([&]() {
    while (keepWorkerThread) {
      // Prevent CPU full usage.
      Proud::Sleep(10);

      // process received RMI and events.
      netClient->FrameMove();
    }
  });
  workerThread.Start();

  bool isReady = false;
  while (keepWorkerThread) {
    // get user input
    string userInput;
    cin >> userInput;

    if (userInput == "w") {
      g_GameProxy.Move(HostID_Server, RmiContext::ReliableSend, (int32_t)'w',
                       enemies);
    } else if (userInput == "a") {
      g_GameProxy.Move(HostID_Server, RmiContext::ReliableSend, (int32_t)'a',
                       enemies);
    } else if (userInput == "s") {
      g_GameProxy.Move(HostID_Server, RmiContext::ReliableSend, (int32_t)'s',
                       enemies);
    } else if (userInput == "d") {
      g_GameProxy.Move(HostID_Server, RmiContext::ReliableSend, (int32_t)'d',
                       enemies);
    } else if (userInput == "f") {
      g_GameProxy.HasPoint(HostID_Server, RmiContext::ReliableSend, 10);
    } else if (userInput == "e") {
      g_GameProxy.EnterRoom(HostID_Server, RmiContext::ReliableSend);
    } else if (userInput == "q") {
      g_GameProxy.ExitRoom(HostID_Server, RmiContext::ReliableSend);
    } else if (userInput == "r") {
      isReady = !isReady;
      g_GameProxy.GetReady(HostID_Server, RmiContext::ReliableSend, isReady);
    } else if (userInput == "x") {
      keepWorkerThread = false;
    }
  }

  netClient->Disconnect();

  return 0;
}
