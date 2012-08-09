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
#include "OutdoorPvPEP.h"
#include "../GameObject.h"


OutdoorPvPEP::OutdoorPvPEP() : OutdoorPvP(),
    m_towersAlliance(0),
    m_towersHorde(0)
{
    m_towerWorldState[0] = WORLD_STATE_NORTHPASS_NEUTRAL;
    m_towerWorldState[1] = WORLD_STATE_CROWNGUARD_NEUTRAL;
    m_towerWorldState[2] = WORLD_STATE_EASTWALL_NEUTRAL;
    m_towerWorldState[3] = WORLD_STATE_PLAGUEWOOD_NEUTRAL;

    for (uint8 i = 0; i < TOWER_COUNT; ++i)
        m_towerOwner[i] = TEAM_NONE;
}

bool OutdoorPvPEP::InitOutdoorPvPArea()
{
    RegisterZone(ZONE_ID_EASTERN_PLAGUELANDS);
    RegisterZone(ZONE_ID_STRATHOLME);
    RegisterZone(ZONE_ID_SCHOLOMANCE);

    return true;
}

void OutdoorPvPEP::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_COUNT_ALLIANCE, m_towersAlliance);
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_COUNT_HORDE, m_towersHorde);

    for (uint8 i = 0; i < TOWER_COUNT; ++i)
        FillInitialWorldState(data, count, m_towerWorldState[i], WORLD_STATE_ADD);
}

void OutdoorPvPEP::SendRemoveWorldStates(Player* player)
{
    player->SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_ALLIANCE, WORLD_STATE_REMOVE);
    player->SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HORDE, WORLD_STATE_REMOVE);

    for (uint8 i = 0; i < TOWER_COUNT; ++i)
        player->SendUpdateWorldState(m_towerWorldState[i], WORLD_STATE_REMOVE);
}

void OutdoorPvPEP::UpdateWorldState()
{
    // update only tower count; tower states are sent in the process event
    SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_ALLIANCE, m_towersAlliance);
    SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HORDE, m_towersHorde);
}

void OutdoorPvPEP::HandlePlayerEnterZone(Player* player)
{
    // remove the buff from the player first; Sometimes on relog players still have the aura
    for (uint8 i = 0; i < TOWER_COUNT; ++i)
        player->RemoveAurasDueToSpell(player->GetTeam() == ALLIANCE ? PLAGUELANDS_TOWER_BUFFS[i].spellIdAlliance : PLAGUELANDS_TOWER_BUFFS[i].spellIdHorde);

    // buff the player
    switch (player->GetTeam())
    {
        case ALLIANCE:
            if (m_towersAlliance > 0)
                player->CastSpell(player, PLAGUELANDS_TOWER_BUFFS[m_towersAlliance - 1].spellIdAlliance, true);
            break;
        case HORDE:
            if (m_towersHorde > 0)
                player->CastSpell(player, PLAGUELANDS_TOWER_BUFFS[m_towersHorde - 1].spellIdHorde, true);
            break;
    }

    OutdoorPvP::HandlePlayerEnterZone(player);
}

void OutdoorPvPEP::HandlePlayerLeaveZone(Player* player)
{
    // remove the buff from the player
    for (uint8 i = 0; i < TOWER_COUNT; ++i)
        player->RemoveAurasDueToSpell(player->GetTeam() == ALLIANCE ? PLAGUELANDS_TOWER_BUFFS[i].spellIdAlliance : PLAGUELANDS_TOWER_BUFFS[i].spellIdHorde);

    OutdoorPvP::HandlePlayerLeaveZone(player);
}

void OutdoorPvPEP::OnGameObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_1:
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_2:
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_3:
        case GO_BATTLEFIELD_BANNER_PLAGUELANDS_4:
        case GO_TOWER_BANNER:
            // sort banners
            if (go->IsWithinDist2d(PLAGUELANDS_TOWER_LOCATIONS[0].x, PLAGUELANDS_TOWER_LOCATIONS[0].y, 50.0f))
            {
                m_towerBanners[0].push_back(go->GetObjectGuid());
                go->SetGoArtKit(GetBannerArtKit(m_towerOwner[0], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            }
            else if (go->IsWithinDist2d(PLAGUELANDS_TOWER_LOCATIONS[1].x, PLAGUELANDS_TOWER_LOCATIONS[1].y, 50.0f))
            {
                m_towerBanners[1].push_back(go->GetObjectGuid());
                go->SetGoArtKit(GetBannerArtKit(m_towerOwner[1], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            }
            else if (go->IsWithinDist2d(PLAGUELANDS_TOWER_LOCATIONS[2].x, PLAGUELANDS_TOWER_LOCATIONS[2].y, 50.0f))
            {
                m_towerBanners[2].push_back(go->GetObjectGuid());
                go->SetGoArtKit(GetBannerArtKit(m_towerOwner[2], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            }
            else if (go->IsWithinDist2d(PLAGUELANDS_TOWER_LOCATIONS[3].x, PLAGUELANDS_TOWER_LOCATIONS[3].y, 50.0f))
            {
                m_towerBanners[3].push_back(go->GetObjectGuid());
                go->SetGoArtKit(GetBannerArtKit(m_towerOwner[3], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            }
            break;
        case GO_LORDAERON_SHRINE_ALLIANCE:
            m_lordaeronShrineAlliance = go->GetObjectGuid();
            break;
        case GO_LORDAERON_SHRINE_HORDE:
            m_lordaeronShrineHorde = go->GetObjectGuid();
            break;
    }
}

void OutdoorPvPEP::HandleObjectiveComplete(uint32 eventId, std::list<Player*> players, Team team)
{
    uint32 credit = 0;

    switch (eventId)
    {
        case EVENT_CROWNGUARD_PROGRESS_ALLIANCE:
        case EVENT_CROWNGUARD_PROGRESS_HORDE:
            credit = NPC_CROWNGUARD_TOWER_QUEST_DOODAD;
            break;
        case EVENT_EASTWALL_PROGRESS_ALLIANCE:
        case EVENT_EASTWALL_PROGRESS_HORDE:
            credit = NPC_EASTWALL_TOWER_QUEST_DOODAD;
            break;
        case EVENT_NORTHPASS_PROGRESS_ALLIANCE:
        case EVENT_NORTHPASS_PROGRESS_HORDE:
            credit = NPC_NORTHPASS_TOWER_QUEST_DOODAD;
            break;
        case EVENT_PLAGUEWOOD_PROGRESS_ALLIANCE:
        case EVENT_PLAGUEWOOD_PROGRESS_HORDE:
            credit = NPC_PLAGUEWOOD_TOWER_QUEST_DOODAD;
            break;
        default:
            return;
    }

    for (std::list<Player*>::iterator itr = players.begin(); itr != players.end(); ++itr)
    {
        if ((*itr) && (*itr)->GetTeam() == team)
        {
            (*itr)->KilledMonsterCredit(credit);
            (*itr)->RewardHonor(NULL, 1, HONOR_REWARD_PLAGUELANDS);
        }
    }
}

// process the capture events
void OutdoorPvPEP::ProcessEvent(uint32 eventId, GameObject* go)
{
    for (uint8 i = 0; i < TOWER_COUNT; ++i)
    {
        if (PLAGUELANDS_BANNERS[i] == go->GetEntry())
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (PLAGUELANDS_TOWER_EVENTS[i][j].eventEntry == eventId)
                {
                    if (PLAGUELANDS_TOWER_EVENTS[i][j].team != m_towerOwner[i])
                    {
                        ProcessCaptureEvent(go, i, PLAGUELANDS_TOWER_EVENTS[i][j].team, PLAGUELANDS_TOWER_EVENTS[i][j].worldState);
                        go->GetMap()->SendZoneDefenseMessage(PLAGUELANDS_TOWER_EVENTS[i][j].zoneText, ZONE_ID_EASTERN_PLAGUELANDS);
                    }
                    return;
                }
            }
            return;
        }
    }
}

void OutdoorPvPEP::ProcessCaptureEvent(GameObject* go, uint32 towerId, Team team, uint32 newWorldState)
{
    if (team == ALLIANCE)
    {
        for (std::list<ObjectGuid>::iterator itr = m_towerBanners[towerId].begin(); itr != m_towerBanners[towerId].end(); ++itr)
            SetBannerVisual(go, (*itr), CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ANIM_ALLIANCE);

        ++m_towersAlliance;
        BuffTeam(ALLIANCE, PLAGUELANDS_TOWER_BUFFS[m_towersAlliance - 1].spellIdAlliance);
    }
    else if (team == HORDE)
    {
        for (std::list<ObjectGuid>::iterator itr = m_towerBanners[towerId].begin(); itr != m_towerBanners[towerId].end(); ++itr)
            SetBannerVisual(go, (*itr), CAPTURE_ARTKIT_HORDE, CAPTURE_ANIM_HORDE);

        ++m_towersHorde;
        BuffTeam(HORDE, PLAGUELANDS_TOWER_BUFFS[m_towersHorde - 1].spellIdHorde);
    }
    else
    {
        for (std::list<ObjectGuid>::iterator itr = m_towerBanners[towerId].begin(); itr != m_towerBanners[towerId].end(); ++itr)
            SetBannerVisual(go, (*itr), CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);

        if (m_towerOwner[towerId] == ALLIANCE)
        {
            if (--m_towersAlliance == 0)
                BuffTeam(ALLIANCE, PLAGUELANDS_TOWER_BUFFS[0].spellIdAlliance, true);
        }
        else
        {
            if (--m_towersHorde == 0)
                BuffTeam(HORDE, PLAGUELANDS_TOWER_BUFFS[0].spellIdHorde, true);
        }
    }

    // handle rewards of each tower
    if (team != TEAM_NONE)
    {
        // update capture point owner
        m_towerOwner[towerId] = team;

        switch (towerId)
        {
            case TOWER_ID_NORTHPASS:
                UpdateShrine(go);
                break;
            case TOWER_ID_CROWNGUARD:
                sObjectMgr.SetGraveYardLinkTeam(GRAVEYARD_ID_EASTERN_PLAGUE, GRAVEYARD_ZONE_EASTERN_PLAGUE, team);
                break;
            case TOWER_ID_EASTWALL:
                if (m_towerOwner[TOWER_ID_NORTHPASS] != team)
                    SummonSoldiers(go);
                break;
            case TOWER_ID_PLAGUEWOOD:
                SummonFlightMaster(go);
                break;
        }
    }
    else
    {
        switch (towerId)
        {
            case TOWER_ID_NORTHPASS:
                UpdateShrine(go, true);
                break;
            case TOWER_ID_CROWNGUARD:
                sObjectMgr.SetGraveYardLinkTeam(GRAVEYARD_ID_EASTERN_PLAGUE, GRAVEYARD_ZONE_EASTERN_PLAGUE, TEAM_INVALID);
                break;
            case TOWER_ID_EASTWALL:
                UnsummonSoldiers(go);
                break;
            case TOWER_ID_PLAGUEWOOD:
                UnsummonFlightMaster(go);
                break;
        }

        // update capture point owner
        m_towerOwner[towerId] = team;
    }

    // update tower state
    SendUpdateWorldState(m_towerWorldState[towerId], WORLD_STATE_REMOVE);
    m_towerWorldState[towerId] = newWorldState;
    SendUpdateWorldState(m_towerWorldState[towerId], WORLD_STATE_ADD);

    // update counter state
    UpdateWorldState();
}

void OutdoorPvPEP::SummonFlightMaster(WorldObject* objRef)
{
    if (Creature* flightMaster = objRef->SummonCreature(NPC_SPECTRAL_FLIGHTMASTER, FLIGHTMASTER_SPAWN_LOCATION[0], FLIGHTMASTER_SPAWN_LOCATION[1], FLIGHTMASTER_SPAWN_LOCATION[2], FLIGHTMASTER_SPAWN_LOCATION[3], TEMPSUMMON_DEAD_DESPAWN, 0))
        m_flightMaster = flightMaster->GetObjectGuid();
}

void OutdoorPvPEP::UnsummonFlightMaster(const WorldObject* objRef)
{
    if (Creature* flightMaster = objRef->GetMap()->GetCreature(m_flightMaster))
        flightMaster->ForcedDespawn();
}

void OutdoorPvPEP::SummonSoldiers(WorldObject* objRef)
{
    uint32 entry = m_towerOwner[TOWER_ID_NORTHPASS] == ALLIANCE ? NPC_LORDAERON_COMMANDER : NPC_LORDAERON_VETERAN;

    for (uint8 i = 0; i < 5; ++i)
    {
        if (Creature* soldier = objRef->SummonCreature(entry, PLAGUELANDS_SOLDIER_SPAWN_LOCATIONS[i].x, PLAGUELANDS_SOLDIER_SPAWN_LOCATIONS[i].y, PLAGUELANDS_SOLDIER_SPAWN_LOCATIONS[i].z, 2.2f, TEMPSUMMON_DEAD_DESPAWN, 0))
            m_soldiers.push_back(soldier->GetObjectGuid());

        // change the entry id to the soldiers for the last 4 iterations
        if (i == 0)
            entry = entry == NPC_LORDAERON_COMMANDER ? NPC_LORDAERON_SOLDIER : NPC_LORDAERON_FIGHTER;
    }
}

void OutdoorPvPEP::UnsummonSoldiers(const WorldObject* objRef)
{
    for (std::list<ObjectGuid>::iterator itr = m_soldiers.begin(); itr != m_soldiers.end(); ++itr)
    {
        if (Creature* soldier = objRef->GetMap()->GetCreature(*itr))
            soldier->ForcedDespawn();
    }
}

void OutdoorPvPEP::UpdateShrine(const WorldObject* objRef, bool remove)
{
    ObjectGuid shrineGuid = m_towerOwner[TOWER_ID_NORTHPASS] == ALLIANCE ? m_lordaeronShrineAlliance : m_lordaeronShrineHorde;
    if (GameObject* shrine = objRef->GetMap()->GetGameObject(shrineGuid))
    {
        if (!remove)
        {
            shrine->SetRespawnTime(7 * DAY);
            shrine->Refresh();
        }
        else if (shrine->isSpawned())
            shrine->Delete();
    }
}
