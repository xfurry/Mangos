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
#include "OutdoorPvPZM.h"
#include "../GameObject.h"


OutdoorPvPZM::OutdoorPvPZM() : OutdoorPvP(),
    m_graveyardWorldState(WORLD_STATE_GRAVEYARD_NEUTRAL),
    m_scoutWorldStateAlliance(WORLD_STATE_ALLIANCE_FLAG_NOT_READY),
    m_scoutWorldStateHorde(WORLD_STATE_HORDE_FLAG_NOT_READY),

    m_graveyardOwner(TEAM_NONE),
    m_towersAlliance(0),
    m_towersHorde(0)
{
    // init world states
    m_towerWorldState[0] = WORLD_STATE_TOWER_EAST_NEUTRAL;
    m_towerWorldState[1] = WORLD_STATE_TOWER_WEST_NEUTRAL;
    m_towerMapState[0] = WORLD_STATE_BEACON_EAST_NEUTRAL;
    m_towerMapState[1] = WORLD_STATE_BEACON_WEST_NEUTRAL;

    for (uint8 i = 0; i < MAX_ZM_TOWERS; ++i)
        m_towerOwner[i] = TEAM_NONE;
}

bool OutdoorPvPZM::InitOutdoorPvPArea()
{
    RegisterZone(ZONE_ID_ZANGARMARSH);
    RegisterZone(ZONE_ID_SERPENTSHRINE_CAVERN);
    RegisterZone(ZONE_ID_STREAMVAULT);
    RegisterZone(ZONE_ID_UNDERBOG);
    RegisterZone(ZONE_ID_SLAVE_PENS);

    return true;
}

void OutdoorPvPZM::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, m_scoutWorldStateAlliance, WORLD_STATE_ADD);
    FillInitialWorldState(data, count, m_scoutWorldStateHorde, WORLD_STATE_ADD);
    FillInitialWorldState(data, count, m_graveyardWorldState, WORLD_STATE_ADD);

    for (uint8 i = 0; i < MAX_ZM_TOWERS; ++i)
    {
        FillInitialWorldState(data, count, m_towerWorldState[i], WORLD_STATE_ADD);
        FillInitialWorldState(data, count, m_towerMapState[i], WORLD_STATE_ADD);
    }
}

void OutdoorPvPZM::SendRemoveWorldStates(Player* player)
{
    player->SendUpdateWorldState(m_scoutWorldStateAlliance, WORLD_STATE_REMOVE);
    player->SendUpdateWorldState(m_scoutWorldStateHorde, WORLD_STATE_REMOVE);
    player->SendUpdateWorldState(m_graveyardWorldState, WORLD_STATE_REMOVE);

    for (uint8 i = 0; i < MAX_ZM_TOWERS; ++i)
    {
        player->SendUpdateWorldState(m_towerWorldState[i], WORLD_STATE_REMOVE);
        player->SendUpdateWorldState(m_towerMapState[i], WORLD_STATE_REMOVE);
    }
}

void OutdoorPvPZM::HandlePlayerEnterZone(Player* player)
{
    // remove the buff from the player first; Sometimes on relog players still have the aura
    player->RemoveAurasDueToSpell(SPELL_TWIN_SPIRE_BLESSING);

    // cast buff the the player which enters the zone
    if ((player->GetTeam() == ALLIANCE ? m_towersAlliance : m_towersHorde) == MAX_ZM_TOWERS)
        player->CastSpell(player, SPELL_TWIN_SPIRE_BLESSING, true);

    OutdoorPvP::HandlePlayerEnterZone(player);
}

void OutdoorPvPZM::HandlePlayerLeaveZone(Player* player)
{
    // remove the buff from the player
    player->RemoveAurasDueToSpell(SPELL_TWIN_SPIRE_BLESSING);

    OutdoorPvP::HandlePlayerLeaveZone(player);
}

void OutdoorPvPZM::OnCreatureCreate(Creature* creature)
{
    switch (creature->GetEntry())
    {
        case NPC_ALLIANCE_FIELD_SCOUT:
            m_allianceScout = creature->GetObjectGuid();
            break;
        case NPC_HORDE_FIELD_SCOUT:
            m_hordeScout = creature->GetObjectGuid();
            break;
        case NPC_PVP_BEAM_RED:
            // East Beam
            if (creature->GetPositionY() < 7000.0f)
            {
                m_beamTowerRed[0] = creature->GetObjectGuid();
                if (m_towerOwner[0] == HORDE)
                    return;
            }
            // Center Beam
            else if (creature ->GetPositionY() < 7300.0f)
            {
                m_beamGraveyardRed = creature->GetObjectGuid();
                if (m_graveyardOwner == HORDE)
                    return;
            }
            // West Beam
            else
            {
                m_beamTowerRed[1] = creature->GetObjectGuid();
                if (m_towerOwner[1] == HORDE)
                    return;
            }
            break;
        case NPC_PVP_BEAM_BLUE:
            // East Beam
            if (creature->GetPositionY() < 7000.0f)
            {
                m_beamTowerBlue[0] = creature->GetObjectGuid();
                if (m_towerOwner[0] == ALLIANCE)
                    return;
            }
            // Center Beam
            else if (creature ->GetPositionY() < 7300.0f)
            {
                m_beamGraveyardBlue = creature->GetObjectGuid();
                if (m_graveyardOwner == ALLIANCE)
                    return;
            }
            // West Beam
            else
            {
                m_beamTowerBlue[1] = creature->GetObjectGuid();
                if (m_towerOwner[1] == ALLIANCE)
                    return;
            }
            break;
    }
}

void OutdoorPvPZM::OnGameObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case GO_ZANGA_BANNER_EAST:
            m_towerBanners[0] = go->GetObjectGuid();
            break;
        case GO_ZANGA_BANNER_WEST:
            m_towerBanners[1] = go->GetObjectGuid();
            break;
        case GO_ZANGA_BANNER_CENTER_ALLIANCE:
            m_graveyardBannerAlliance = go->GetObjectGuid();
            break;
        case GO_ZANGA_BANNER_CENTER_HORDE:
            m_graveyardBannerHorde = go->GetObjectGuid();
            break;
        case GO_ZANGA_BANNER_CENTER_NEUTRAL:
            m_graveyardBannerNeutral = go->GetObjectGuid();
            break;
    }
}

// Cast player spell on opponent kill
void OutdoorPvPZM::HandlePlayerKillInsideArea(Player* player, Unit* victim)
{
    for (uint8 i = 0; i < MAX_ZM_TOWERS; ++i)
    {
        if (GameObject* capturePoint = player->GetMap()->GetGameObject(m_towerBanners[i]))
        {
            // check capture point range
            GameObjectInfo const* info = capturePoint->GetGOInfo();
            if (info && player->IsWithinDistInMap(capturePoint, info->capturePoint.radius))
            {
                // check capture point team
                if (player->GetTeam() == m_towerOwner[i])
                    player->CastSpell(player, player->GetTeam() == ALLIANCE ? SPELL_ZANGA_TOWER_TOKEN_ALLIANCE : SPELL_ZANGA_TOWER_TOKEN_HORDE, true);

                return;
            }
        }
    }
}

// process the capture events
void OutdoorPvPZM::ProcessEvent(uint32 eventId, GameObject* go)
{
    for (uint8 i = 0; i < MAX_ZM_TOWERS; ++i)
    {
        if (ZANGA_TOWERS[i] == go->GetEntry())
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (ZANGA_TOWER_EVENTS[i][j].eventEntry == eventId)
                {
                    if (ZANGA_TOWER_EVENTS[i][j].team != m_towerOwner[i])
                    {
                        ProcessCaptureEvent(go, i, ZANGA_TOWER_EVENTS[i][j].team, ZANGA_TOWER_EVENTS[i][j].worldState, ZANGA_TOWER_EVENTS[i][j].mapState);
                        sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(ZANGA_TOWER_EVENTS[i][j].zoneText));
                    }
                    return;
                }
            }
            return;
        }
    }
}

void OutdoorPvPZM::ProcessCaptureEvent(GameObject* go, uint32 towerId, Team team, uint32 newWorldState, uint32 newMapState)
{
    if (team == ALLIANCE)
    {
        SetBeaconArtKit(go, m_beamTowerBlue[towerId], SPELL_BEAM_BLUE);
        ++m_towersAlliance;

        if (m_towersAlliance == MAX_ZM_TOWERS)
            PrepareFactionScouts(go, ALLIANCE);
    }
    else if (team == HORDE)
    {
        SetBeaconArtKit(go, m_beamTowerRed[towerId], SPELL_BEAM_RED);
        ++m_towersHorde;

        if (m_towersHorde == MAX_ZM_TOWERS)
            PrepareFactionScouts(go, HORDE);
    }
    else
    {
        if (m_towerOwner[towerId] == ALLIANCE)
        {
            SetBeaconArtKit(go, m_beamTowerBlue[towerId], 0);

            if (m_towersAlliance == MAX_ZM_TOWERS)
                ResetScouts(go, ALLIANCE);

            --m_towersAlliance;
        }
        else
        {
            SetBeaconArtKit(go, m_beamTowerRed[towerId], 0);

            if (m_towersHorde == MAX_ZM_TOWERS)
                ResetScouts(go, HORDE);

            --m_towersHorde;
        }
    }

    // update tower state
    SendUpdateWorldState(m_towerWorldState[towerId], WORLD_STATE_REMOVE);
    m_towerWorldState[towerId] = newWorldState;
    SendUpdateWorldState(m_towerWorldState[towerId], WORLD_STATE_ADD);

    SendUpdateWorldState(m_towerMapState[towerId], WORLD_STATE_REMOVE);
    m_towerMapState[towerId] = newMapState;
    SendUpdateWorldState(m_towerMapState[towerId], WORLD_STATE_ADD);;

    // update capture point owner
    m_towerOwner[towerId] = team;
}

void OutdoorPvPZM::PrepareFactionScouts(const WorldObject* objRef, Team team)
{
    if (team == ALLIANCE)
    {
        if (Creature* scout = objRef->GetMap()->GetCreature(m_allianceScout))
            scout->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        SendUpdateWorldState(m_scoutWorldStateAlliance, WORLD_STATE_REMOVE);
        m_scoutWorldStateAlliance = WORLD_STATE_ALLIANCE_FLAG_READY;
        SendUpdateWorldState(m_scoutWorldStateAlliance, WORLD_STATE_ADD);
    }
    else
    {
        if (Creature* scout = objRef->GetMap()->GetCreature(m_hordeScout))
            scout->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        SendUpdateWorldState(m_scoutWorldStateHorde, WORLD_STATE_REMOVE);
        m_scoutWorldStateHorde = WORLD_STATE_HORDE_FLAG_READY;
        SendUpdateWorldState(m_scoutWorldStateHorde, WORLD_STATE_ADD);
    }
}

void OutdoorPvPZM::ResetScouts(const WorldObject* objRef, Team team, bool includeWorldStates)
{
    if (team == ALLIANCE)
    {
        if (Creature* scout = objRef->GetMap()->GetCreature(m_allianceScout))
            scout->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        // reset world states only if requested
        if (includeWorldStates)
        {
            SendUpdateWorldState(m_scoutWorldStateAlliance, WORLD_STATE_REMOVE);
            m_scoutWorldStateAlliance = WORLD_STATE_ALLIANCE_FLAG_NOT_READY;
            SendUpdateWorldState(m_scoutWorldStateAlliance, WORLD_STATE_ADD);
        }
    }
    else
    {
        if (Creature* scout = objRef->GetMap()->GetCreature(m_hordeScout))
            scout->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        // reset world states only if requested
        if (includeWorldStates)
        {
            SendUpdateWorldState(m_scoutWorldStateHorde, WORLD_STATE_REMOVE);
            m_scoutWorldStateHorde = WORLD_STATE_HORDE_FLAG_NOT_READY;
            SendUpdateWorldState(m_scoutWorldStateHorde, WORLD_STATE_ADD);
        }
    }
}

bool OutdoorPvPZM::HandleObjectUse(Player* player, GameObject* go)
{
    if (!player->HasAura(player->GetTeam() == ALLIANCE ? SPELL_BATTLE_STANDARD_ALLIANCE : SPELL_BATTLE_STANDARD_HORDE))
        return false;

    switch (go->GetEntry())
    {
        case GO_ZANGA_BANNER_CENTER_ALLIANCE:
            if (player->GetTeam() == ALLIANCE)
                return false;

            // change banners
            SetGraveyardArtKit(go, m_graveyardBannerAlliance, false);
            SetGraveyardArtKit(go, m_graveyardBannerHorde, true);
            SetBeaconArtKit(go, m_beamGraveyardBlue, 0);
            sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_LOOSE_GY_A));

            // remove buff and graveyard
            BuffTeam(m_graveyardOwner, SPELL_TWIN_SPIRE_BLESSING, true);
            SetGraveyard(true, player->GetTeam());

            // update graveyard owner
            m_graveyardOwner = HORDE;

            // add the buff and the graveyard
            BuffTeam(m_graveyardOwner, SPELL_TWIN_SPIRE_BLESSING);
            SetGraveyard(0, TEAM_INVALID);

            SendUpdateWorldState(m_graveyardWorldState, WORLD_STATE_REMOVE);
            m_graveyardWorldState = WORLD_STATE_GRAVEYARD_HORDE;
            SendUpdateWorldState(m_graveyardWorldState, WORLD_STATE_ADD);

            // reset scout and remove player aura
            ResetScouts(go, m_graveyardOwner);
            player->RemoveAurasDueToSpell(SPELL_BATTLE_STANDARD_HORDE);
            SetBeaconArtKit(go, m_beamGraveyardRed, SPELL_BEAM_RED);
            sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_GY_H));

            return true;
        case GO_ZANGA_BANNER_CENTER_HORDE:
            if (player->GetTeam() == HORDE)
                return false;

            // change banners
            SetGraveyardArtKit(go, m_graveyardBannerHorde, false);
            SetGraveyardArtKit(go, m_graveyardBannerAlliance, true);
            SetBeaconArtKit(go, m_beamGraveyardRed, 0);
            sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_LOOSE_GY_H));

            // remove buff and graveyard
            BuffTeam(m_graveyardOwner, SPELL_TWIN_SPIRE_BLESSING, true);
            SetGraveyard(true, player->GetTeam());

            // update graveyard owner
            m_graveyardOwner = ALLIANCE;

            // add the buff and the graveyard to horde
            BuffTeam(m_graveyardOwner, SPELL_TWIN_SPIRE_BLESSING);
            SetGraveyard(0, TEAM_INVALID);

            SendUpdateWorldState(m_graveyardWorldState, WORLD_STATE_REMOVE);
            m_graveyardWorldState = WORLD_STATE_GRAVEYARD_ALLIANCE;
            SendUpdateWorldState(m_graveyardWorldState, WORLD_STATE_ADD);

            // reset scout and remove player aura
            ResetScouts(go, m_graveyardOwner);
            player->RemoveAurasDueToSpell(SPELL_BATTLE_STANDARD_ALLIANCE);
            SetBeaconArtKit(go, m_beamGraveyardBlue, SPELL_BEAM_BLUE);
            sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_GY_A));

            return true;
        case GO_ZANGA_BANNER_CENTER_NEUTRAL:

            // remove old world state
            SendUpdateWorldState(m_graveyardWorldState, WORLD_STATE_REMOVE);

            if (player->GetTeam() == ALLIANCE)
            {
                // update graveyard owner
                m_graveyardOwner = ALLIANCE;

                // change banners
                SetGraveyardArtKit(go, m_graveyardBannerNeutral, false);
                SetGraveyardArtKit(go, m_graveyardBannerAlliance, true);

                // add the buff and the graveyard to horde
                m_graveyardWorldState = WORLD_STATE_GRAVEYARD_ALLIANCE;
                BuffTeam(m_graveyardOwner, SPELL_TWIN_SPIRE_BLESSING);
                SetGraveyard(false, TEAM_INVALID);

                // reset scout and remove player aura
                ResetScouts(go, m_graveyardOwner);
                player->RemoveAurasDueToSpell(SPELL_BATTLE_STANDARD_ALLIANCE);
                SetBeaconArtKit(go, m_beamGraveyardBlue, SPELL_BEAM_BLUE);
                sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_GY_H));
            }
            else
            {
                // update graveyard owner
                m_graveyardOwner = HORDE;

                // change banners
                SetGraveyardArtKit(go, m_graveyardBannerNeutral, false);
                SetGraveyardArtKit(go, m_graveyardBannerHorde, true);

                // add the buff and the graveyard to horde
                m_graveyardWorldState = WORLD_STATE_GRAVEYARD_HORDE;
                BuffTeam(HORDE, SPELL_TWIN_SPIRE_BLESSING);
                SetGraveyard(0, TEAM_INVALID);

                // reset scout and remove player aura
                ResetScouts(go, m_graveyardOwner);
                player->RemoveAurasDueToSpell(SPELL_BATTLE_STANDARD_HORDE);
                SetBeaconArtKit(go, m_beamGraveyardRed, SPELL_BEAM_RED);
                sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_GY_H));
            }

            // add new world state
            SendUpdateWorldState(m_graveyardWorldState, WORLD_STATE_ADD);
            return true;
    }

    return false;
}

void OutdoorPvPZM::SetGraveyard(bool remove, Team team)
{
    if (remove)
        sObjectMgr.SetGraveYardLinkTeam(GRAVEYARD_ID_TWIN_SPIRE, GRAVEYARD_ZONE_TWIN_SPIRE, TEAM_INVALID);
    else
        sObjectMgr.SetGraveYardLinkTeam(GRAVEYARD_ID_TWIN_SPIRE, GRAVEYARD_ZONE_TWIN_SPIRE, team);
}

void OutdoorPvPZM::SetGraveyardArtKit(const WorldObject* objRef, ObjectGuid goGuid, bool respawn)
{
    if (GameObject* banner = objRef->GetMap()->GetGameObject(goGuid))
    {
        if (respawn)
        {
            banner->SetRespawnTime(7 * DAY);
            banner->Refresh();
        }
        // Note: this doesn't work fine, becuase the GO doesn't despawn by itself
        else if (banner->isSpawned())
            banner->SetLootState(GO_JUST_DEACTIVATED);
    }
}

void OutdoorPvPZM::SetBeaconArtKit(const WorldObject* objRef, ObjectGuid creatureGuid, uint32 auraId)
{
    if (Creature* beam = objRef->GetMap()->GetCreature(creatureGuid))
    {
        if (auraId)
            beam->CastSpell(beam, auraId, true);
        else
            beam->RemoveAllAuras();
    }
}
