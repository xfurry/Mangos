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

#include "OutdoorPvPGH.h"
#include "../Map.h"
#include "../Object.h"
#include "../Creature.h"
#include "../GameObject.h"

OutdoorPvPGH::OutdoorPvPGH() : OutdoorPvP(),
    m_zoneOwner(TEAM_NONE)
{
}

void OutdoorPvPGH::HandleCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_WESTFALL_BRIGADE_DEFENDER:
        case NPC_COMMANDER_HOWSER:
            m_allianceSoldiers.push_back(creature->GetObjectGuid());
            if (m_zoneOwner == ALLIANCE)
                return;
            break;
        case NPC_HORSE:
            // check the area id because the horses can be found in other areas too
            if (creature->GetAreaId() != AREA_ID_VENTURE_BAY)
                return;
            // fall through
        case NPC_BLACKSMITH_JASON_RIGGINS:
        case NPC_STABLE_MASTER_TIM:
        case NPC_VENDOR_ADAMS:
            m_allianceVendors.push_back(creature->GetObjectGuid());
            if (m_zoneOwner == ALLIANCE)
                return;
            break;
        case NPC_CONQUEST_HOLD_DEFENDER:
        case NPC_GENERAL_GORLOK:
            m_hordeSoldiers.push_back(creature->GetObjectGuid());
            if (m_zoneOwner == HORDE)
                return;
            break;
        case NPC_BLACKSMITH_KOLOTH:
        case NPC_STABLE_MASTER_KOR:
        case NPC_VENDOR_PURKOM:
        case NPC_RIDING_WOLF: // no need to check for area id because the wolfs are within grizzly hills only in venture bay
            m_hordeVendors.push_back(creature->GetObjectGuid());
            if (m_zoneOwner == HORDE)
                return;
            break;

        default:
            return;
    }

    // Despawn creatures on create - will be spawned later in script
    creature->SetRespawnDelay(7 * DAY);
    creature->ForcedDespawn();
}

void OutdoorPvPGH::HandleGameObjectCreate(GameObject* go)
{
    if (go->GetEntry() == GO_VENTURE_BAY_LIGHTHOUSE)
        go->SetGoArtKit(GetBannerArtKit(m_zoneOwner, CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
}

// process the capture events
void OutdoorPvPGH::HandleEvent(uint32 eventId, GameObject* go)
{
    // If we are not using the lighthouse return
    if (go->GetEntry() != GO_VENTURE_BAY_LIGHTHOUSE)
        return;

    switch (eventId)
    {
        case EVENT_LIGHTHOUSE_WIN_ALLIANCE:
            // Spawn the npcs only when the tower is fully controlled
            m_zoneOwner = ALLIANCE;
            RespawnSoldiers(go);
            break;
        case EVENT_LIGHTHOUSE_WIN_HORDE:
            // Spawn the npcs only when the tower is fully controlled
            m_zoneOwner = HORDE;
            RespawnSoldiers(go);
            break;
        case EVENT_LIGHTHOUSE_PROGRESS_ALLIANCE:
            SetBannerVisual(go, CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ANIM_ALLIANCE);
            break;
        case EVENT_LIGHTHOUSE_PROGRESS_HORDE:
            SetBannerVisual(go, CAPTURE_ARTKIT_HORDE, CAPTURE_ANIM_HORDE);
            break;
        case EVENT_LIGHTHOUSE_NEUTRAL_ALLIANCE:
        case EVENT_LIGHTHOUSE_NEUTRAL_HORDE:
            m_zoneOwner = TEAM_NONE;
            SetBannerVisual(go, CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);
            break;
    }
}

void OutdoorPvPGH::RespawnSoldiers(const WorldObject* objRef)
{
    if (m_zoneOwner == ALLIANCE)
    {
        // despawn all horde vendors
        for (GuidList::const_iterator itr = m_hordeVendors.begin(); itr != m_hordeVendors.end(); ++itr)
        {
            if (Creature* soldier = objRef->GetMap()->GetCreature(*itr))
                soldier->ForcedDespawn();
        }

        // spawn all alliance soldiers and vendors
        for (GuidList::const_iterator itr = m_allianceSoldiers.begin(); itr != m_allianceSoldiers.end(); ++itr)
        {
            if (Creature* soldier = objRef->GetMap()->GetCreature(*itr))
                soldier->Respawn();
        }
        for (GuidList::const_iterator itr = m_allianceVendors.begin(); itr != m_allianceVendors.end(); ++itr)
        {
            if (Creature* soldier = objRef->GetMap()->GetCreature(*itr))
                soldier->Respawn();
        }
    }
    else
    {
        // despawn all alliance vendors
        for (GuidList::const_iterator itr = m_allianceVendors.begin(); itr != m_allianceVendors.end(); ++itr)
        {
            if (Creature* soldier = objRef->GetMap()->GetCreature(*itr))
                soldier->ForcedDespawn();
        }

        // spawn all horde soldiers and vendors
        for (GuidList::const_iterator itr = m_hordeSoldiers.begin(); itr != m_hordeSoldiers.end(); ++itr)
        {
            if (Creature* soldier = objRef->GetMap()->GetCreature(*itr))
                soldier->Respawn();
        }
        for (GuidList::const_iterator itr = m_hordeVendors.begin(); itr != m_hordeVendors.end(); ++itr)
        {
            if (Creature* soldier = objRef->GetMap()->GetCreature(*itr))
                soldier->Respawn();
        }
    }
}
