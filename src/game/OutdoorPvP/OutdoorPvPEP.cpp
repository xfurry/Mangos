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

#include "OutdoorPvPEP.h"
#include "WorldPacket.h"
#include "../World.h"
#include "../ObjectMgr.h"
#include "../Object.h"
#include "../Creature.h"
#include "../GameObject.h"
#include "../Player.h"

OutdoorPvPEP::OutdoorPvPEP() : OutdoorPvP(),
    m_towersAlliance(0),
    m_towersHorde(0)
{
    m_towerWorldState[0] = WORLD_STATE_EP_NORTHPASS_NEUTRAL;
    m_towerWorldState[1] = WORLD_STATE_EP_CROWNGUARD_NEUTRAL;
    m_towerWorldState[2] = WORLD_STATE_EP_EASTWALL_NEUTRAL;
    m_towerWorldState[3] = WORLD_STATE_EP_PLAGUEWOOD_NEUTRAL;

    for (uint8 i = 0; i < MAX_EP_TOWERS; ++i)
        m_towerOwner[i] = TEAM_NONE;
}

void OutdoorPvPEP::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, WORLD_STATE_EP_TOWER_COUNT_ALLIANCE, m_towersAlliance);
    FillInitialWorldState(data, count, WORLD_STATE_EP_TOWER_COUNT_HORDE, m_towersHorde);

    for (uint8 i = 0; i < MAX_EP_TOWERS; ++i)
        FillInitialWorldState(data, count, m_towerWorldState[i], WORLD_STATE_ADD);
}

void OutdoorPvPEP::SendRemoveWorldStates(Player* player)
{
    player->SendUpdateWorldState(WORLD_STATE_EP_TOWER_COUNT_ALLIANCE, WORLD_STATE_REMOVE);
    player->SendUpdateWorldState(WORLD_STATE_EP_TOWER_COUNT_HORDE, WORLD_STATE_REMOVE);

    for (uint8 i = 0; i < MAX_EP_TOWERS; ++i)
        player->SendUpdateWorldState(m_towerWorldState[i], WORLD_STATE_REMOVE);
}

void OutdoorPvPEP::HandlePlayerEnterZone(Player* player, bool isMainZone)
{
    OutdoorPvP::HandlePlayerEnterZone(player, isMainZone);

    // remove the buff from the player first; Sometimes on relog players still have the aura
    for (uint8 i = 0; i < MAX_EP_TOWERS; ++i)
        player->RemoveAurasDueToSpell(player->GetTeam() == ALLIANCE ? plaguelandsTowerBuffs[i].spellIdAlliance : plaguelandsTowerBuffs[i].spellIdHorde);

    // buff the player
    switch (player->GetTeam())
    {
        case ALLIANCE:
            if (m_towersAlliance > 0)
                player->CastSpell(player, plaguelandsTowerBuffs[m_towersAlliance - 1].spellIdAlliance, true);
            break;
        case HORDE:
            if (m_towersHorde > 0)
                player->CastSpell(player, plaguelandsTowerBuffs[m_towersHorde - 1].spellIdHorde, true);
            break;
    }
}

void OutdoorPvPEP::HandlePlayerLeaveZone(Player* player, bool isMainZone)
{
    // remove the buff from the player
    for (uint8 i = 0; i < MAX_EP_TOWERS; ++i)
        player->RemoveAurasDueToSpell(player->GetTeam() == ALLIANCE ? plaguelandsTowerBuffs[i].spellIdAlliance : plaguelandsTowerBuffs[i].spellIdHorde);

    OutdoorPvP::HandlePlayerLeaveZone(player, isMainZone);
}

void OutdoorPvPEP::HandleGameObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case GO_TOWER_BANNER_NORTHPASS:
            InitBanner(go, TOWER_ID_NORTHPASS);
            break;
        case GO_TOWER_BANNER_CROWNGUARD:
            InitBanner(go, TOWER_ID_CROWNGUARD);
            break;
        case GO_TOWER_BANNER_EASTWALL:
            InitBanner(go, TOWER_ID_EASTWALL);
            break;
        case GO_TOWER_BANNER_PLAGUEWOOD:
            InitBanner(go, TOWER_ID_PLAGUEWOOD);
            break;
        case GO_TOWER_BANNER:
            // sort banners
            if (go->IsWithinDist2d(plaguelandsTowerLocations[TOWER_ID_NORTHPASS].x, plaguelandsTowerLocations[TOWER_ID_NORTHPASS].y, 50.0f))
                InitBanner(go, TOWER_ID_NORTHPASS);
            else if (go->IsWithinDist2d(plaguelandsTowerLocations[TOWER_ID_CROWNGUARD].x, plaguelandsTowerLocations[TOWER_ID_CROWNGUARD].y, 50.0f))
                InitBanner(go, TOWER_ID_CROWNGUARD);
            else if (go->IsWithinDist2d(plaguelandsTowerLocations[TOWER_ID_EASTWALL].x, plaguelandsTowerLocations[TOWER_ID_EASTWALL].y, 50.0f))
                InitBanner(go, TOWER_ID_EASTWALL);
            else if (go->IsWithinDist2d(plaguelandsTowerLocations[TOWER_ID_PLAGUEWOOD].x, plaguelandsTowerLocations[TOWER_ID_PLAGUEWOOD].y, 50.0f))
                InitBanner(go, TOWER_ID_PLAGUEWOOD);
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
void OutdoorPvPEP::HandleEvent(uint32 eventId, GameObject* go)
{
    for (uint8 i = 0; i < MAX_EP_TOWERS; ++i)
    {
        if (plaguelandsBanners[i] == go->GetEntry())
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (plaguelandsTowerEvents[i][j].eventEntry == eventId)
                {
                    // prevent processing if the owner did not change (happens if progress event is called after contest event)
                    if (plaguelandsTowerEvents[i][j].team != m_towerOwner[i])
                    {
                        if (plaguelandsTowerEvents[i][j].defenseMessage)
                            sWorld.SendDefenseMessage(ZONE_ID_EASTERN_PLAGUELANDS, plaguelandsTowerEvents[i][j].defenseMessage);

                        ProcessCaptureEvent(go, i, plaguelandsTowerEvents[i][j].team, plaguelandsTowerEvents[i][j].worldState);
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
        // update banner
        for (GuidList::iterator itr = m_towerBanners[towerId].begin(); itr != m_towerBanners[towerId].end(); ++itr)
            SetBannerVisual(go, (*itr), CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ANIM_ALLIANCE);

        // update counter
        ++m_towersAlliance;
        SendUpdateWorldState(WORLD_STATE_EP_TOWER_COUNT_ALLIANCE, m_towersAlliance);

        // buff players
        BuffTeam(ALLIANCE, plaguelandsTowerBuffs[m_towersAlliance - 1].spellIdAlliance);
    }
    else if (team == HORDE)
    {
        // update banner
        for (GuidList::iterator itr = m_towerBanners[towerId].begin(); itr != m_towerBanners[towerId].end(); ++itr)
            SetBannerVisual(go, (*itr), CAPTURE_ARTKIT_HORDE, CAPTURE_ANIM_HORDE);

        // update counter
        ++m_towersHorde;
        SendUpdateWorldState(WORLD_STATE_EP_TOWER_COUNT_HORDE, m_towersHorde);

        // buff players
        BuffTeam(HORDE, plaguelandsTowerBuffs[m_towersHorde - 1].spellIdHorde);
    }
    else
    {
        // update banner
        for (GuidList::iterator itr = m_towerBanners[towerId].begin(); itr != m_towerBanners[towerId].end(); ++itr)
            SetBannerVisual(go, (*itr), CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);

        if (m_towerOwner[towerId] == ALLIANCE)
        {
            // update counter
            --m_towersAlliance;
            SendUpdateWorldState(WORLD_STATE_EP_TOWER_COUNT_ALLIANCE, m_towersAlliance);

            if (m_towersAlliance == 0)
                BuffTeam(ALLIANCE, plaguelandsTowerBuffs[0].spellIdAlliance, true);
        }
        else
        {
            // update counter
            --m_towersHorde;
            SendUpdateWorldState(WORLD_STATE_EP_TOWER_COUNT_HORDE, m_towersHorde);

            if (m_towersHorde == 0)
                BuffTeam(HORDE, plaguelandsTowerBuffs[0].spellIdHorde, true);
        }
    }

    if (team != TEAM_NONE)
    {
        // update capture point owner
        m_towerOwner[towerId] = team;

        // apply rewards of changed tower
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
        // remove rewards of changed tower
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
}

void OutdoorPvPEP::InitBanner(GameObject* go, uint32 towerId)
{
    m_towerBanners[towerId].push_back(go->GetObjectGuid());
    go->SetGoArtKit(GetBannerArtKit(m_towerOwner[towerId], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
}

void OutdoorPvPEP::SummonFlightMaster(WorldObject* objRef)
{
    if (Creature* flightMaster = objRef->SummonCreature(NPC_SPECTRAL_FLIGHTMASTER, plaguelandsFlightmasterSpawnLocation[0], plaguelandsFlightmasterSpawnLocation[1], plaguelandsFlightmasterSpawnLocation[2], plaguelandsFlightmasterSpawnLocation[3], TEMPSUMMON_DEAD_DESPAWN, 0))
        m_flightMaster = flightMaster->GetObjectGuid();
}

void OutdoorPvPEP::UnsummonFlightMaster(const WorldObject* objRef)
{
    if (Creature* flightMaster = objRef->GetMap()->GetCreature(m_flightMaster))
        flightMaster->ForcedDespawn();
}

void OutdoorPvPEP::SummonSoldiers(WorldObject* objRef)
{
    uint32 entry = m_towerOwner[TOWER_ID_EASTWALL] == ALLIANCE ? NPC_LORDAERON_COMMANDER : NPC_LORDAERON_VETERAN;

    for (uint8 i = 0; i < 5; ++i)
    {
        if (Creature* soldier = objRef->SummonCreature(entry, plaguelandsSoldierSpawnLocations[i].x, plaguelandsSoldierSpawnLocations[i].y, plaguelandsSoldierSpawnLocations[i].z, 2.2f, TEMPSUMMON_DEAD_DESPAWN, 0))
            m_soldiers.push_back(soldier->GetObjectGuid());

        // change the entry id to the soldiers for the last 4 iterations
        if (i == 0)
            entry = entry == NPC_LORDAERON_COMMANDER ? NPC_LORDAERON_SOLDIER : NPC_LORDAERON_FIGHTER;
    }
}

void OutdoorPvPEP::UnsummonSoldiers(const WorldObject* objRef)
{
    for (GuidList::iterator itr = m_soldiers.begin(); itr != m_soldiers.end(); ++itr)
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
