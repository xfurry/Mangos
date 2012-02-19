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

#include "WorldPvP.h"

/**
   Function that a players to a players set

   @param   player to be added to set
 */
void WorldPvP::HandlePlayerEnterZone(Player* pPlayer)
{
    m_sZonePlayers.insert(pPlayer);
}

/**
   Function that a players to a players set

   @param   player to be removed
 */
void WorldPvP::HandlePlayerLeaveZone(Player* pPlayer)
{
    // remove the world state information from the player
    if (!pPlayer->GetSession()->PlayerLogout())
        SendRemoveWorldStates(pPlayer);

    m_sZonePlayers.erase(pPlayer);

    sLog.outDebug("Player %s left an outdoorpvp zone", pPlayer->GetName());
}

/**
   Function that updates world state for all the players in an outdoor pvp area

   @param   world state it to update
   @param   value which should update the world state
 */
void WorldPvP::SendUpdateWorldState(uint32 uiField, uint32 uiValue)
{
    for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
        (*itr)->SendUpdateWorldState(uiField, uiValue);
}

/**
   Function that handles the player kill in outdoor pvp

   @param   player which kills another player
   @param   player or unit (pet) which is victim
 */
void WorldPvP::HandlePlayerKill(Player* pKiller, Unit* pVictim)
{
    if (Group* pGroup = pKiller->GetGroup())
    {
        for (GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
        {
            Player* pGroupGuy = itr->getSource();

            if (!pGroupGuy)
                continue;

            // skip if too far away
            if (!pGroupGuy->IsAtGroupRewardDistance(pVictim))
                continue;

            // creature kills must be notified, even if not inside objective / not outdoor pvp active
            // player kills only count if active and inside objective
            if (pGroupGuy->IsWorldPvPActive())
                HandlePlayerKillInsideArea(pGroupGuy, pVictim);
        }
    }
    else
    {
        // creature kills must be notified, even if not inside objective / not outdoor pvp active
        if (pKiller && pKiller->IsWorldPvPActive())
            HandlePlayerKillInsideArea(pKiller, pVictim);
    }
}

// register this zone as an outdoor pvp script
void WorldPvP::RegisterZone(uint32 uiZoneId)
{
    sWorldPvPMgr.AddZone(uiZoneId, this);
}

// return if has player inside the zone
bool WorldPvP::HasPlayer(Player* pPlayer) const
{
    return m_sZonePlayers.find(pPlayer) != m_sZonePlayers.end();
}

// set a capture point slider value for when the gameobject is being reloaded the next time
void WorldPvP::SetCapturePointSliderValue(uint32 pointEntry, CapturePointSlider value)
{
    sWorldPvPMgr.SetCapturePointSlider(pointEntry, value);
}

// apply a team buff for the specific zone
void WorldPvP::DoProcessTeamBuff(Team faction, uint32 uiSpellId, bool bRemove)
{
    for (PlayerSet::iterator itr = m_sZonePlayers.begin(); itr != m_sZonePlayers.end(); ++itr)
    {
        if ((*itr) && (*itr)->GetTeam() == faction)
        {
            if (bRemove)
                (*itr)->RemoveAurasDueToSpell(uiSpellId);
            else
                (*itr)->CastSpell(*itr, uiSpellId, true);
        }
    }
}

void WorldPvP::SetCapturePointVisual(const WorldObject* objRef, ObjectGuid capturePointGuid, uint32 artKit, uint32 animId)
{
    if (GameObject* capturePoint = objRef->GetMap()->GetGameObject(capturePointGuid))
        SetCapturePointVisual(capturePoint, artKit, animId);
}

void WorldPvP::SetCapturePointVisual(GameObject* go, uint32 artKit, uint32 animId)
{
    go->SendGameObjectCustomAnim(go->GetObjectGuid(), animId);
    go->SetGoArtKit(artKit);
    go->Refresh();
}
