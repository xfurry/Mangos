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

#include "OutdoorPvPMgr.h"
#include "OutdoorPvPEP.h"
#include "OutdoorPvPGH.h"
#include "OutdoorPvPHP.h"
#include "OutdoorPvPNA.h"
#include "OutdoorPvPSI.h"
#include "OutdoorPvPTF.h"
#include "OutdoorPvPZM.h"
#include "Policies/SingletonImp.h"

INSTANTIATE_SINGLETON_1(OutdoorPvPMgr);

OutdoorPvPMgr::OutdoorPvPMgr()
{
    m_UpdateTimer.SetInterval(sWorld.getConfig(CONFIG_UINT32_INTERVAL_MAPUPDATE));
}

OutdoorPvPMgr::~OutdoorPvPMgr()
{
    for (OutdoorPvPSet::iterator itr = m_OutdoorPvPSet.begin(); itr != m_OutdoorPvPSet.end(); ++itr)
        delete *itr;
}

/**
   Function which loads the world pvp scripts
 */
void OutdoorPvPMgr::InitOutdoorPvP()
{
    uint8 count = 0;
    for (uint8 id = 0; id < 7; ++id)
    {
        OutdoorPvP* outdoorPvP = NULL;
        switch (id)
        {
            case 0: outdoorPvP = new OutdoorPvPEP; break;
            case 1: outdoorPvP = new OutdoorPvPSI; break;
            case 2: outdoorPvP = new OutdoorPvPHP; break;
            case 3: outdoorPvP = new OutdoorPvPNA; break;
            case 4: outdoorPvP = new OutdoorPvPTF; break;
            case 5: outdoorPvP = new OutdoorPvPZM; break;
            case 6: outdoorPvP = new OutdoorPvPGH; break;
        }

        if (outdoorPvP->InitOutdoorPvPArea())
        {
            m_OutdoorPvPSet.push_back(outdoorPvP);
            ++count;
        }
        else
        {
            sLog.outDebug("OutdoorPvP: Outdoor PvP id %u loading failed.", id);
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
void OutdoorPvPMgr::AddZone(uint32 uiZoneId, OutdoorPvP* pScriptHandler)
{
    m_OutdoorPvPMap[uiZoneId] = pScriptHandler;
}

/**
   Function that handles the players which enters a specific zone

   @param   player to be handled in the event
   @param   zone id used for the current world pvp script
 */
void OutdoorPvPMgr::HandlePlayerEnterZone(Player* pPlayer, uint32 uiZoneId)
{
    OutdoorPvPMap::iterator itr = m_OutdoorPvPMap.find(uiZoneId);
    if (itr == m_OutdoorPvPMap.end())
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
void OutdoorPvPMgr::HandlePlayerLeaveZone(Player* pPlayer, uint32 uiZoneId)
{
    OutdoorPvPMap::iterator itr = m_OutdoorPvPMap.find(uiZoneId);
    if (itr == m_OutdoorPvPMap.end())
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
/*void OutdoorPvPMgr::HandleDropFlag(Player* pPlayer, uint32 uiSpellId)
{
    for (OutdoorPvPSet::iterator itr = m_OutdoorPvPSet.begin(); itr != m_OutdoorPvPSet.end(); ++itr)
    {
        if ((*itr)->HandleDropFlag(pPlayer, uiSpellId))
            return;
    }
}*/

/**
   Function that handles the objective complete of a capture point

   @param   player set to which to send the credit
   @param   capture evetn id
 */
/*void OutdoorPvPMgr::HandleObjectiveComplete(uint32 uiEventId, std::list<Player*> players, Team team)
{
    for (OutdoorPvPSet::iterator itr = m_OutdoorPvPSet.begin(); itr != m_OutdoorPvPSet.end(); ++itr)
        (*itr)->HandleObjectiveComplete(uiEventId, players, team);
}*/

/**
   Function that handles the player kill inside a capture point

   @param   player
   @param   victim
 */
/*void OutdoorPvPMgr::HandlePlayerKill(Player* pPlayer, Unit* pVictim)
{
    for (OutdoorPvPSet::iterator itr = m_OutdoorPvPSet.begin(); itr != m_OutdoorPvPSet.end(); ++itr)
        (*itr)->HandlePlayerKill(pPlayer, pVictim);
}*/

/**
   Function that handles when a player uses a world pvp gameobject

   @param   player which executes the event
   @param   gameobject used
 */
/*bool OutdoorPvPMgr::HandleObjectUse(Player* pPlayer, GameObject* pGo)
{
    for (OutdoorPvPSet::iterator itr = m_OutdoorPvPSet.begin(); itr != m_OutdoorPvPSet.end(); ++itr)
    {
        if ((*itr)->HandleObjectUse(pPlayer, pGo))
            return true;
    }
    return false;
}*/

/**
   Function that returns a specific world pvp script for a given zone id

   @param   zone id used for the current world pvp script
 */
OutdoorPvP* OutdoorPvPMgr::GetOutdoorPvP(uint32 zoneId)
{
    OutdoorPvPMap::iterator itr = m_OutdoorPvPMap.find(zoneId);

    if (itr != m_OutdoorPvPMap.end())
        return itr->second;

    return NULL;
}

/**
   Function that returns the zone script for a specific zone id

   @param   zone id used for the current world pvp script
 */
ZoneScript* OutdoorPvPMgr::GetZoneScript(uint32 zoneId)
{
    OutdoorPvPMap::iterator itr = m_OutdoorPvPMap.find(zoneId);

    if (itr != m_OutdoorPvPMap.end())
        return itr->second;

    return NULL;
}

void OutdoorPvPMgr::Update(uint32 diff)
{
    m_UpdateTimer.Update(diff);
    if (!m_UpdateTimer.Passed())
        return;

    for (OutdoorPvPSet::iterator itr = m_OutdoorPvPSet.begin(); itr != m_OutdoorPvPSet.end(); ++itr)
        (*itr)->Update((uint32)m_UpdateTimer.GetCurrent());

    m_UpdateTimer.SetCurrent(0);
}

/**
   Function that gets the capture point slider value

   @param   capture point entry
 */
int8 OutdoorPvPMgr::GetCapturePointSliderValue(uint32 uiEntry)
{
    std::map<uint32, int8>::iterator find = m_CapturePointSlider.find(uiEntry);
    if (find != m_CapturePointSlider.end())
        return find->second;

    // return default value if we can't find any
    return CAPTURE_SLIDER_NEUTRAL;
}
