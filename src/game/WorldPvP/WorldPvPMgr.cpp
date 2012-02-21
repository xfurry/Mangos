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

#include "WorldPvPMgr.h"
#include "WorldPvPEP.h"
#include "WorldPvPGH.h"
#include "WorldPvPHP.h"
#include "WorldPvPNA.h"
#include "WorldPvPSI.h"
#include "WorldPvPTF.h"
#include "WorldPvPZM.h"
#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1(WorldPvPMgr);

WorldPvPMgr::WorldPvPMgr()
{
    m_UpdateTimer.SetInterval(sWorld.getConfig(CONFIG_UINT32_INTERVAL_MAPUPDATE));
}

WorldPvPMgr::~WorldPvPMgr()
{
    for (WorldPvPSet::iterator itr = m_WorldPvPSet.begin(); itr != m_WorldPvPSet.end(); ++itr)
        delete *itr;
}

/**
   Function which loads the world pvp scripts
 */
void WorldPvPMgr::InitWorldPvP()
{
    uint8 count = 7;
    for (uint8 id = 0; id < count; ++id)
    {
        WorldPvP* outdoorPvP = NULL;
        switch (id)
        {
            case 0: outdoorPvP = new WorldPvPEP; break;
            case 1: outdoorPvP = new WorldPvPSI; break;
            case 2: outdoorPvP = new WorldPvPHP; break;
            case 3: outdoorPvP = new WorldPvPNA; break;
            case 4: outdoorPvP = new WorldPvPTF; break;
            case 5: outdoorPvP = new WorldPvPZM; break;
            case 6: outdoorPvP = new WorldPvPGH; break;
        }

        if (outdoorPvP->InitWorldPvPArea())
            m_WorldPvPSet.push_back(outdoorPvP);
        else
        {
            sLog.outDebug("WorldPvP: Outdoor PvP id %u loading failed.", id);
            --count;
            delete outdoorPvP;
        }
    }

    sLog.outString();
    sLog.outString(">> Loaded %u World PvP zones", count);
}

/**
   Function that adds a specific zone to a outdoor pvp script

   @param   zone id used for the current outdoor pvp script
   @param   outdoor pvp script object
 */
void WorldPvPMgr::AddZone(uint32 uiZoneId, WorldPvP* pScriptHandler)
{
    m_WorldPvPMap[uiZoneId] = pScriptHandler;
}

/**
   Function that handles the players which enters a specific zone

   @param   player to be handled in the event
   @param   zone id used for the current world pvp script
 */
void WorldPvPMgr::HandlePlayerEnterZone(Player* pPlayer, uint32 uiZoneId)
{
    WorldPvPMap::iterator itr = m_WorldPvPMap.find(uiZoneId);
    if (itr == m_WorldPvPMap.end())
        return;

    if (itr->second->HasPlayer(pPlayer))
        return;

    itr->second->HandlePlayerEnterZone(pPlayer);
}

/**
   Function that handles the players which leaves a specific zone

   @param   player to be handled in the event
   @param   zone id used for the current outdoor pvp script
 */
void WorldPvPMgr::HandlePlayerLeaveZone(Player* pPlayer, uint32 uiZoneId)
{
    WorldPvPMap::iterator itr = m_WorldPvPMap.find(uiZoneId);
    if (itr == m_WorldPvPMap.end())
        return;

    // teleport: remove once in removefromworld, once in updatezone
    if (!itr->second->HasPlayer(pPlayer))
        return;

    itr->second->HandlePlayerLeaveZone(pPlayer);
}

/**
   Function that handles when a player drops a flag during an outtdoor pvp event

   @param   player which executes the event
   @param   spell id which acts as the flag
 */
void WorldPvPMgr::HandleDropFlag(Player* pPlayer, uint32 uiSpellId)
{
    for (WorldPvPSet::iterator itr = m_WorldPvPSet.begin(); itr != m_WorldPvPSet.end(); ++itr)
    {
        if ((*itr)->HandleDropFlag(pPlayer, uiSpellId))
            return;
    }
}

/**
   Function that handles the objective complete of a capture point

   @param   player set to which to send the credit
   @param   capture evetn id
 */
void WorldPvPMgr::HandleObjectiveComplete(uint32 uiEventId, std::list<Player*> players, Team team)
{
    for (WorldPvPSet::iterator itr = m_WorldPvPSet.begin(); itr != m_WorldPvPSet.end(); ++itr)
        (*itr)->HandleObjectiveComplete(players, uiEventId, team);
}

/**
   Function that handles the player kill inside a capture point

   @param   player
   @param   victim
 */
void WorldPvPMgr::HandlePlayerKill(Player* pPlayer, Unit* pVictim)
{
    for (WorldPvPSet::iterator itr = m_WorldPvPSet.begin(); itr != m_WorldPvPSet.end(); ++itr)
        (*itr)->HandlePlayerKill(pPlayer, pVictim);
}

/**
   Function that handles when a player uses a world pvp gameobject

   @param   player which executes the event
   @param   gameobject used
 */
bool WorldPvPMgr::HandleObjectUse(Player* pPlayer, GameObject* pGo)
{
    for (WorldPvPSet::iterator itr = m_WorldPvPSet.begin(); itr != m_WorldPvPSet.end(); ++itr)
    {
        if ((*itr)->HandleObjectUse(pPlayer, pGo))
            return true;
    }
    return false;
}

/**
   Function that returns a specific world pvp script for a given zone id

   @param   zone id used for the current world pvp script
 */
WorldPvP* WorldPvPMgr::GetWorldPvPToZoneId(uint32 uiZoneId)
{
    WorldPvPMap::iterator itr = m_WorldPvPMap.find(uiZoneId);

    // no handle for this zone, return
    if (itr == m_WorldPvPMap.end())
        return NULL;

    return itr->second;
}

/**
   Function that returns the zone script for a specific zone id

   @param   zone id used for the current world pvp script
 */
ZoneScript* WorldPvPMgr::GetZoneScript(uint32 uiZoneId)
{
    WorldPvPMap::iterator itr = m_WorldPvPMap.find(uiZoneId);

    if (itr != m_WorldPvPMap.end())
        return itr->second;
    else
        return NULL;
}

void WorldPvPMgr::Update(uint32 diff)
{
    m_UpdateTimer.Update(diff);
    if (!m_UpdateTimer.Passed())
        return;

    for (WorldPvPSet::iterator itr = m_WorldPvPSet.begin(); itr != m_WorldPvPSet.end(); ++itr)
        (*itr)->Update((uint32)m_UpdateTimer.GetCurrent());

    m_UpdateTimer.SetCurrent(0);
}

/**
   Function that gets the capture point slider value

   @param   capture point entry
 */
int8 WorldPvPMgr::GetCapturePointSliderValue(uint32 uiEntry)
{
    std::map<uint32, int8>::iterator find = m_CapturePointSlider.find(uiEntry);
    if (find != m_CapturePointSlider.end())
        return find->second;

    // return default value if we can't find any
    return CAPTURE_SLIDER_RESET;
}
