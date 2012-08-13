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
#include "Policies/SingletonImp.h"
#include "OutdoorPvP.h"
#include "../World.h"
#include "Log.h"
#include "OutdoorPvPEP.h"
#include "OutdoorPvPGH.h"
#include "OutdoorPvPHP.h"
#include "OutdoorPvPNA.h"
#include "OutdoorPvPSI.h"
#include "OutdoorPvPTF.h"
#include "OutdoorPvPZM.h"

INSTANTIATE_SINGLETON_1(OutdoorPvPMgr);

OutdoorPvPMgr::OutdoorPvPMgr()
{
    m_updateTimer.SetInterval(sWorld.getConfig(CONFIG_UINT32_INTERVAL_MAPUPDATE));
}

OutdoorPvPMgr::~OutdoorPvPMgr()
{
    for (OutdoorPvPSet::iterator itr = m_outdoorPvPSet.begin(); itr != m_outdoorPvPSet.end(); ++itr)
        delete *itr;
}

/**
   Function which loads all outdoor pvp scripts
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
            m_outdoorPvPSet.push_back(outdoorPvP);
            ++count;
        }
        else
        {
            sLog.outDebug("OutdoorPvP: Outdoor PvP id %u loading failed.", id);
            delete outdoorPvP;
        }
    }

    sLog.outString();
    sLog.outString(">> Loaded %u Outdoor PvP zones", count);
}

/**
   Function that adds a specific zone to a outdoor pvp script

   @param   outdoor pvp script object
   @param   zone id used for the current outdoor pvp script
 */
void OutdoorPvPMgr::AddZone(OutdoorPvP* scriptHandler, uint32 zoneId)
{
    m_outdoorPvPMap[zoneId] = scriptHandler;
}

/**
   Function that handles the players which enters a specific zone

   @param   player to be handled in the event
   @param   zone id used for the current outdoor pvp script
 */
void OutdoorPvPMgr::HandlePlayerEnterZone(Player* player, uint32 zoneId)
{
    OutdoorPvPMap::iterator itr = m_outdoorPvPMap.find(zoneId);
    if (itr != m_outdoorPvPMap.end())
        itr->second->HandlePlayerEnterZone(player);
}

/**
   Function that handles the players which leaves a specific zone

   @param   player to be handled in the event
   @param   zone id used for the current outdoor pvp script
 */
void OutdoorPvPMgr::HandlePlayerLeaveZone(Player* player, uint32 zoneId)
{
    // teleport: remove once in removefromworld, once in updatezone
    OutdoorPvPMap::iterator itr = m_outdoorPvPMap.find(zoneId);
    if (itr != m_outdoorPvPMap.end())
        itr->second->HandlePlayerLeaveZone(player);
}

/**
   Function that returns a specific outdoor pvp script for a given zone id

   @param   zone id used for the current outdoor pvp script
 */
OutdoorPvP* OutdoorPvPMgr::GetOutdoorPvP(uint32 zoneId)
{
    OutdoorPvPMap::iterator itr = m_outdoorPvPMap.find(zoneId);

    if (itr != m_outdoorPvPMap.end())
        return itr->second;

    return NULL;
}

void OutdoorPvPMgr::Update(uint32 diff)
{
    m_updateTimer.Update(diff);
    if (!m_updateTimer.Passed())
        return;

    for (OutdoorPvPSet::iterator itr = m_outdoorPvPSet.begin(); itr != m_outdoorPvPSet.end(); ++itr)
        (*itr)->Update((uint32)m_updateTimer.GetCurrent());

    m_updateTimer.SetCurrent(0);
}

/**
   Function that gets the capture point slider value

   @param   capture point entry
 */
int8 OutdoorPvPMgr::GetCapturePointSliderValue(uint32 entry)
{
    std::map<uint32, int8>::iterator itr = m_CapturePointSlider.find(entry);

    if (itr != m_CapturePointSlider.end())
        return itr->second;

    // return default value if we can't find any
    return CAPTURE_SLIDER_NEUTRAL;
}
