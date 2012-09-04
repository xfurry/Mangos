/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef __BATTLEFIELD_H
#define __BATTLEFIELD_H

#include "BattlefieldMgr.h"
#include "MapManager.h"
#include "Group.h"
#include "GameObject.h"
#include "InstanceData.h"

#include <list>

enum Battlefields
{
    BATTLEFIELD_WG = 1,
    BATTLEFIELD_TB = 21
};

#define MAX_TEAM 2

typedef std::list<Player *> PlayerList;

class Map;
class GameObject;

class BattlefieldData : public InstanceData
{
    typedef std::map<uint32,uint64> ulongStorage;
    typedef std::map<uint32,uint32> uintStorage;

    public:
        BattlefieldData(Map * pMap) : InstanceData(pMap)
        {
            Initialize();
        }

        void Initialize();
        void Load(const char* input);
        const char* Save();

        uint64 GetData64(uint32 data);
        void SetData64(uint32 data, uint64 value);

        uint32 GetData(uint32 type);
        void SetData(uint32 type, uint32 data);

        bool CheckAchievementCriteriaMeet(uint32 criteria_id, Player const* source, Unit const* target = NULL, uint32 miscvalue1 = 0);
        bool CheckConditionCriteriaMeet(Player const* source, uint32 map_id, uint32 instance_condition_id);

    private:
        ulongStorage        m_ulongs;
        uintStorage         m_uints;
        std::string         m_saveString;
};

class Battlefield
{
    friend class BattlefieldMgr;

    public:

        Battlefield(uint8 BattleId);
        ~Battlefield();
        
        void Update(uint32 uiDiff);
        void BattleStart();
        void BattleEnd();

        void PlayerJoin(Player* player);
        void PlayerLeave(Player* player);
        bool IsBattleInProgress() const { return m_battleInProgress; }
        bool AddPlayerToGroup(Player * player);

        uint8  GetBattleId() { return m_battleId; }
        uint8  GetControllerTeam() { return m_controlledByTeam; }
        uint32 GetZoneId() { return m_zoneId; }
        uint32 GetTimeToNextBattle() { return m_nextBattleTimer; }
        uint32 GetTimeTIllEndOfWar() { return m_battleDurationTimer; }

    protected:
        uint8           m_defenderTeam;
        uint8           m_attackerTeam;
        Team            m_controlledByTeam;
        uint8           m_battleId;
        uint32          m_nextBattleTimer;
        uint32          m_battleDurationTimer;
        uint32          m_preBattleTimer;
        uint32          m_zoneId;
        bool            m_battleInProgress;
        bool            m_invitationSent;
        Group*          m_raidGroup[MAX_TEAM];
        Map*            m_map;

    protected:
        // Battlefield API

        //Called before battle has started
        virtual void BeforeBattleStarted() = 0;

        //Called after Battle has ended
        virtual void AfterBattleEnded() = 0;

        //Called on world tick
        virtual void OnUpdate(uint32 uiDiff) = 0;

        //Called when a player enters WG
        virtual void OnPlayerEnter(Player* player) = 0;

        //Called when a player leaves WG
        virtual void OnPlayerExit(Player* player) = 0;

};

#endif
