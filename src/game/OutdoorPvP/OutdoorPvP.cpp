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

#include "OutdoorPvP.h"

/**
   Function that adds a player to the players of the affected outdoor pvp zones

   @param   player to add
 */
void OutdoorPvP::HandlePlayerEnterZone(Player* player)
{
    m_zonePlayers.insert(player);
}

/**
   Function that removes a player from the players of the affected outdoor pvp zones

   @param   player to remove
 */
void OutdoorPvP::HandlePlayerLeaveZone(Player* player)
{
    if (m_zonePlayers.erase(player))
    {
        // remove the world state information from the player
        if (!player->GetSession()->PlayerLogout())
            SendRemoveWorldStates(player);

        sLog.outDebug("Player %s left an outdoorpvp zone", player->GetName());
    }
}

/**
   Function that updates the world state for all the players of the affected outdoor pvp zones

   @param   world state to update
   @param   new world state value
 */
void OutdoorPvP::SendUpdateWorldState(uint32 field, uint32 value)
{
    for (PlayerSet::iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
        (*itr)->SendUpdateWorldState(field, value);
}

/**
   Function that handles player kills in outdoor pvp zones

   @param   player who killed another player
   @param   victim who was killed
 */
void OutdoorPvP::HandlePlayerKill(Player* killer, Unit* victim)
{
    if (Group* group = killer->GetGroup())
    {
        for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* groupMember = itr->getSource();

            if (!groupMember)
                continue;

            // skip if too far away
            if (!groupMember->IsAtGroupRewardDistance(victim))
                continue;

            // creature kills must be notified, even if not inside objective / not outdoor pvp active
            // player kills only count if active and inside objective
            if (groupMember->CanUseOutdoorCapturePoint())
                HandlePlayerKillInsideArea(groupMember, victim);
        }
    }
    else
    {
        // creature kills must be notified, even if not inside objective / not outdoor pvp active
        if (killer && killer->CanUseOutdoorCapturePoint())
            HandlePlayerKillInsideArea(killer, victim);
    }
}

// register this zone as an outdoor pvp zone
void OutdoorPvP::RegisterZone(uint32 zoneId)
{
    //sOutdoorPvPMgr.AddZone(zoneId, this);
}

// set a capture point slider value for when the gameobject is being reloaded the next time
void OutdoorPvP::SetCapturePointSliderValue(uint32 entry, CapturePointSlider value)
{
    //sOutdoorPvPMgr.SetCapturePointSlider(entry, value);
}

// apply a team buff for the specific zone
void OutdoorPvP::BuffTeam(Team team, uint32 spellId, bool remove)
{
    for (PlayerSet::iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
    {
        if ((*itr) && (*itr)->GetTeam() == team)
        {
            if (remove)
                (*itr)->RemoveAurasDueToSpell(spellId);
            else
                (*itr)->CastSpell(*itr, spellId, true);
        }
    }
}

uint32 OutdoorPvP::GetBannerArtKit(Team team, uint32 artKitAlliance, uint32 artKitHorde, uint32 artKitNeutral)
{
    switch (team)
    {
        case ALLIANCE:
            return artKitAlliance;
        case HORDE:
            return artKitHorde;
        default:
            return artKitNeutral;
    }
}

void OutdoorPvP::SetBannerVisual(const WorldObject* objRef, ObjectGuid goGuid, uint32 artKit, uint32 animId)
{
    if (GameObject* go = objRef->GetMap()->GetGameObject(goGuid))
        SetBannerVisual(go, artKit, animId);
}

void OutdoorPvP::SetBannerVisual(GameObject* go, uint32 artKit, uint32 animId)
{
    go->SendGameObjectCustomAnim(go->GetObjectGuid(), animId);
    go->SetGoArtKit(artKit);
    go->Refresh();
}
