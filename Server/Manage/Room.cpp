#include "Room.h"
#include <stdint.h>
#include <vcruntime.h>

Room::Room()
{
    m_HostMap = map<HostID, int32_t>();
    m_PlayersReady = map<int32_t, bool>();
    m_PlayersPoint = map<int32_t, int32_t>();
    m_GameStarted = false;
}

size_t Room::size() const
{
    return m_HostMap.size();
}

void Room::join(HostID hostID)
{
    int32_t player_no = find_empty_player_no();

    m_HostMap.insert(pair<HostID, int32_t>(hostID, player_no));
    m_PlayersReady.insert(pair<int32_t, bool>(player_no, false));
    m_PlayersPoint.insert(pair<int32_t, int32_t>(player_no, 0));
}

int32_t Room::find_empty_player_no() const
{
    if (m_HostMap.size() == MAX_PLAYERS_PER_ROOM)
        throw "Room is full";
    
    if (m_HostMap.size() == 0)
        return 1;
    
    for (int32_t player_no = 1; player_no <= MAX_PLAYERS_PER_ROOM; player_no++)
    {
        bool found = false;
        for (auto it = m_HostMap.begin(); it != m_HostMap.end(); it++)
        {
            if (it->second == player_no)
            {
                found = true;
                break;
            }
        }
        if (!found)
            return player_no;
    }

    throw "Room is full";
}

void Room::leave(HostID hostID)
{
    int32_t player_no = m_HostMap[hostID];

    m_HostMap.erase(hostID);
    m_PlayersReady.erase(player_no);
    m_PlayersPoint.erase(player_no);
}

void Room::set_player_ready(HostID hostID, bool ready)
{
    int32_t player_no = m_HostMap[hostID];

    m_PlayersReady[player_no] = ready;
}

void Room::set_player_point(HostID hostID, int32_t point)
{
    int32_t player_no = m_HostMap[hostID];

    m_PlayersPoint[player_no] = point;
}

int32_t Room::get_player_no(HostID hostID) const
{
    return m_HostMap.at(hostID);
}

map<int32_t, bool> Room::get_players_ready() const
{
    return m_PlayersReady;
}

map<int32_t, int32_t> Room::get_players_rank() const
{
    map<int32_t, int32_t> players_rank;

    for (auto it = m_PlayersPoint.begin(); it != m_PlayersPoint.end(); it++)
    {
        int32_t player_no = it->first;
        int32_t points = it->second;

        int32_t rank = 1;
        for (auto it2 = m_PlayersPoint.begin(); it2 != m_PlayersPoint.end(); it2++)
        {
            if (it2->second > points)
                rank++;
        }

        players_rank.insert(pair<int32_t, int32_t>(player_no, rank));
    }

    return players_rank;
}

bool Room::is_game_started() const
{
    return m_GameStarted;
}

bool Room::is_all_players_ready() const
{
    for (auto it = m_PlayersReady.begin(); it != m_PlayersReady.end(); it++)
    {
        if (!it->second)
            return false;
    }

    return true;
}

void Room::reset()
{
    for (auto& ready : m_PlayersReady) {
        ready.second = false;
    }
    
    for (auto& point : m_PlayersPoint) {
        point.second = 0;
    }

    m_GameStarted = false;
}

void Room::start_game()
{
    m_GameStarted = true;
}

void Room::end_game()
{
    m_GameStarted = false;
}