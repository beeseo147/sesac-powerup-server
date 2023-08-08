#include "../../ProudNet/include/ProudNetCommon.h"
#include <stdint.h>
#include <vcruntime.h>
#include <vector>

using namespace Proud;

namespace PowerupS2G {
    
	inline CMessage &operator>>(CMessage &m, std::map<int32_t,bool> &playersReady) {
		size_t size;
		m >> size;

		if (size < 0 || size >= CNetConfig::MessageMaxLength)
			ThrowExceptionOnReadArray(size);

		int32_t player_no;
		bool ready;
		for (int i = 0; i < size; i++) {
			m >> player_no >> ready;
			playersReady.insert(std::pair<int32_t,bool>(player_no, ready));
		}
		return m;
	}

	inline CMessage &operator<<(CMessage &m, const std::map<int32_t,bool> &playersReady) {
		size_t size = playersReady.size();
		m << size;

		for (std::map<int32_t, bool>::const_iterator i = playersReady.begin(); i != playersReady.end(); i++) {
			m << i->first << i->second;
		}

		return m;
	}
    
	inline CMessage &operator>>(CMessage &m, std::map<int32_t, int32_t> &playersRank) {
		size_t size;
		m >> size;

		if (size < 0 || size >= CNetConfig::MessageMaxLength)
			ThrowExceptionOnReadArray(size);

		int32_t player_no;
		int32_t rank;
		for (int i = 0; i < size; i++) {
			m >> player_no >> rank;
			playersRank.insert(std::pair<int32_t, int32_t>(player_no, rank));
		}
		return m;
	}

	inline CMessage &operator<<(CMessage &m, const std::map<int32_t, int32_t> &playersRank) {
		size_t size = playersRank.size();
		m << size;

		for (std::map<int32_t,  int32_t>::const_iterator i = playersRank.begin(); i != playersRank.end(); i++) {
			m << i->first << i->second;
		}

		return m;
	}
    
	inline CMessage &operator>>(CMessage &m, std::vector<int32_t> &enemies) {
		size_t size;
		m >> size;

		if (size < 0 || size >= CNetConfig::MessageMaxLength)
			ThrowExceptionOnReadArray(size);

		int32_t enemy_type;
		for (int i = 0; i < size; i++) {
			m >> enemy_type;
			enemies.push_back(enemy_type);
		}
		return m;
	}

	inline CMessage &operator<<(CMessage &m, const std::vector<int32_t> &enemies) {
		size_t size = enemies.size();
		m << size;

		for (std::vector<int32_t>::const_iterator i = enemies.begin(); i != enemies.end(); i++) {
			m << *i;
		}

		return m;
	}
}

namespace PowerupC2S {
	inline CMessage &operator>>(CMessage &m, std::vector<int32_t> &enemies) {
		size_t size;
		m >> size;

		if (size < 0 || size >= CNetConfig::MessageMaxLength)
			ThrowExceptionOnReadArray(size);

		int32_t enemy_type;
		for (int i = 0; i < size; i++) {
			m >> enemy_type;
			enemies.push_back(enemy_type);
		}
		return m;
	}

	inline CMessage &operator<<(CMessage &m, const std::vector<int32_t> &enemies) {
		size_t size = enemies.size();
		m << size;

		for (std::vector<int32_t>::const_iterator i = enemies.begin(); i != enemies.end(); i++) {
			m << *i;
		}

		return m;
	}
}

namespace Proud {
	inline void AppendTextOut(String &s, const std::map<int32_t,bool> &playersReady) {
		s += L"<map<int32_t,bool>>";
	}

	inline void AppendTextOut(String &s, const std::vector<int32_t> &enemies) {
		s += L"<vector<int32_t>>";
	}

	inline void AppendTextOut(String &s, const std::map<int32_t,int32_t> &playersRank) {
		s += L"<vector<int32_t>>";
	}
}