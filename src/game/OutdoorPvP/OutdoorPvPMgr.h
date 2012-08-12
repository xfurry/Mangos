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

#ifndef WORLD_PVP_MGR_H
#define WORLD_PVP_MGR_H


#include "OutdoorPvP.h"
#include "Common.h"
#include "Policies/Singleton.h"
#include "../SharedDefines.h"
#include "Platform/Define.h"
#include "../Map.h"

class Player;
class GameObject;
class Creature;
class OutdoorPvP;

class OutdoorPvPMgr
{
    public:
        OutdoorPvPMgr();
        ~OutdoorPvPMgr();

        // create world pvp events
        void InitOutdoorPvP();

        // called when a player enters an world pvp area
        void HandlePlayerEnterZone(Player* player, uint32 zoneId);

        // called when player leaves an world pvp area
        void HandlePlayerLeaveZone(Player* player, uint32 zoneId);

        // return assigned world pvp
        OutdoorPvP* GetOutdoorPvP(uint32 zoneId);

        // add zone id to world pvp handler
        void AddZone(uint32 zoneId, OutdoorPvP* scriptHandler);

        void Update(uint32);

        // Handle cases when a player drops a flag
        //void HandleDropFlag(Player* player, uint32 spellId);

        // Handle cases when a gameobject is used
        //bool HandleObjectUse(Player* player, GameObject* go);

        // Handle the complete credit for capture point win events
        //void HandleObjectiveComplete(uint32 eventId, std::list<Player*> players, Team team);

        // Handle the kill event inside the area
        //void HandlePlayerKill(Player* player, Unit* victim);

        // Handle capture point stuff
        int8 GetCapturePointSliderValue(uint32 entry);
        void SetCapturePointSlider(uint32 entry, int8 value) { m_CapturePointSlider[entry] = value; }

        typedef std::vector<OutdoorPvP*> OutdoorPvPSet;
        typedef std::map<uint32 /* zoneid */, OutdoorPvP*> OutdoorPvPMap;

    private:
        // contains all initiated world pvp events
        // used when initing / cleaning up
        OutdoorPvPSet m_outdoorPvPSet;

        // maps the zone ids to an world pvp event
        // used in player event handling
        OutdoorPvPMap m_outdoorPvPMap;

        std::map<uint32 /*capture point entry*/, int8 /*slider value*/> m_CapturePointSlider;

        // update interval
        IntervalTimer m_updateTimer;
};

#define sOutdoorPvPMgr MaNGOS::Singleton<OutdoorPvPMgr>::Instance()

#endif
