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
#include "WorldPvPZM.h"


WorldPvPZM::WorldPvPZM() : WorldPvP(),
    m_uiEastBeaconController(0),
    m_uiWestBeaconController(0),

    m_uiEastBeaconState(NEUTRAL),
    m_uiWestBeaconState(NEUTRAL),

    m_uiEastBeaconWorldState(WORLD_STATE_TOWER_EAST_NEUTRAL),
    m_uiWestBeaconWorldState(WORLD_STATE_TOWER_WEST_NEUTRAL),
    m_uiEastBeaconMapState(WORLD_STATE_BEACON_EAST_NEUTRAL),
    m_uiWestBeaconMapState(WORLD_STATE_BEACON_WEST_NEUTRAL),
    m_uiGraveyardWorldState(WORLD_STATE_GRAVEYARD_NEUTRAL),
    m_uiAllianceScoutWorldState(WORLD_STATE_ALY_FLAG_NOT_READY),
    m_uiHordeScoutWorldState(WORLD_STATE_HORDE_FLAG_NOT_READY),

    m_uiTowersAlly(0),
    m_uiTowersHorde(0)
{
    m_uiTypeId = WORLD_PVP_TYPE_ZM;
}

bool WorldPvPZM::InitWorldPvPArea()
{
    RegisterZone(ZONE_ID_ZANGARMARSH);
    RegisterZone(ZONE_ID_SERPENTSHRINE_CAVERN);
    RegisterZone(ZONE_ID_STREAMVAULT);
    RegisterZone(ZONE_ID_UNDERBOG);
    RegisterZone(ZONE_ID_SLAVE_PENS);

    return true;
}

void WorldPvPZM::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, m_uiEastBeaconWorldState,    1);
    FillInitialWorldState(data, count, m_uiWestBeaconWorldState,    1);
    FillInitialWorldState(data, count, m_uiEastBeaconMapState,      1);
    FillInitialWorldState(data, count, m_uiWestBeaconMapState,      1);
    FillInitialWorldState(data, count, m_uiAllianceScoutWorldState, 1);
    FillInitialWorldState(data, count, m_uiHordeScoutWorldState,    1);
    FillInitialWorldState(data, count, m_uiGraveyardWorldState,     1);
}

void WorldPvPZM::SendRemoveWorldStates(Player* pPlayer)
{
    pPlayer->SendUpdateWorldState(m_uiEastBeaconWorldState,         0);
    pPlayer->SendUpdateWorldState(m_uiWestBeaconWorldState,         0);
    pPlayer->SendUpdateWorldState(m_uiEastBeaconMapState,           0);
    pPlayer->SendUpdateWorldState(m_uiWestBeaconMapState,           0);
    pPlayer->SendUpdateWorldState(m_uiAllianceScoutWorldState,      0);
    pPlayer->SendUpdateWorldState(m_uiHordeScoutWorldState,         0);
    pPlayer->SendUpdateWorldState(m_uiGraveyardWorldState,          0);
}

void WorldPvPZM::HandlePlayerEnterZone(Player* pPlayer)
{
    // remove the buff from the player first; Sometimes on relog players still have the aura
    pPlayer->RemoveAurasDueToSpell(SPELL_TWIN_SPIRE_BLESSING);

    // cast buff the the player which enters the zone
    if ((pPlayer->GetTeam() == ALLIANCE ? m_uiTowersAlly : m_uiTowersHorde) == MAX_ZM_TOWERS)
        pPlayer->CastSpell(pPlayer, SPELL_TWIN_SPIRE_BLESSING, true);

    WorldPvP::HandlePlayerEnterZone(pPlayer);
}

void WorldPvPZM::HandlePlayerLeaveZone(Player* pPlayer)
{
    // remove the buff from the player
    pPlayer->RemoveAurasDueToSpell(SPELL_TWIN_SPIRE_BLESSING);

    WorldPvP::HandlePlayerLeaveZone(pPlayer);
}

void WorldPvPZM::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ALLIANCE_FIELD_SCOUT:
            m_AllianceScountGUID = pCreature->GetObjectGuid();
            break;
        case NPC_HORDE_FIELD_SCOUT:
            m_HorderScountGUID = pCreature->GetObjectGuid();
            break;
        case NPC_PVP_BEAM_RED:
            if (pCreature->GetPositionY() < 7000.0f)
                m_BeamEastRedGUID = pCreature->GetObjectGuid();
            else if (pCreature ->GetPositionY() < 7300.0f)
                m_BeamCenterRedGUID = pCreature->GetObjectGuid();
            else
                m_BeamWestRedGUID = pCreature->GetObjectGuid();
            pCreature->SetRespawnDelay(7*DAY);
            pCreature->ForcedDespawn();
            break;
        case NPC_PVP_BEAM_BLUE:
            if (pCreature->GetPositionY() < 7000.0f)
                m_BeamEastBlueGUID = pCreature->GetObjectGuid();
            else if (pCreature ->GetPositionY() < 7300.0f)
                m_BeamCenterBlueGUID = pCreature->GetObjectGuid();
            else
                m_BeamWestBlueGUID = pCreature->GetObjectGuid();
            pCreature->SetRespawnDelay(7*DAY);
            pCreature->ForcedDespawn();
            break;
    }
}

void WorldPvPZM::OnGameObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_ZANGA_BANNER_WEST:
            m_TowerBannerWestGUID = pGo->GetObjectGuid();
            break;
        case GO_ZANGA_BANNER_EAST:
            m_TowerBannerEastGUID = pGo->GetObjectGuid();
            break;
        case GO_ZANGA_BANNER_CENTER_ALY:
            m_TowerBannerCenterAlyGUID = pGo->GetObjectGuid();
            break;
        case GO_ZANGA_BANNER_CENTER_HORDE:
            m_TowerBannerCenterHordeGUID = pGo->GetObjectGuid();
            break;
        case GO_ZANGA_BANNER_CENTER_NEUTRAL:
            m_TowerBannerCenterNeutralGUID = pGo->GetObjectGuid();
            break;
    }
}

// process the capture events
void WorldPvPZM::ProcessEvent(GameObject* pGo, Player* pPlayer, uint32 uiEventId)
{
    switch(pGo->GetEntry())
    {
        case GO_ZANGA_BANNER_EAST:
            switch(uiEventId)
            {
                case EVENT_EAST_BEACON_PROGRESS_ALLIANCE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_EAST_BEACON);
                    sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_EAST_A));
                    break;
                case EVENT_EAST_BEACON_PROGRESS_HORDE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_EAST_BEACON);
                    sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_EAST_H));
                    break;
                case EVENT_EAST_BEACON_NEUTRAL_ALLIANCE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_EAST_BEACON);
                    sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_LOOSE_EAST_A));
                    break;
                case EVENT_EAST_BEACON_NEUTRAL_HORDE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_EAST_BEACON);
                    sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_LOOSE_EAST_H));
                    break;
            }
            break;
        case GO_ZANGA_BANNER_WEST:
            switch(uiEventId)
            {
                case EVENT_WEST_BEACON_PROGRESS_ALLIANCE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_WEST_BEACON);
                    sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_WEST_A));
                    break;
                case EVENT_WEST_BEACON_PROGRESS_HORDE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_WEST_BEACON);
                    sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_WEST_H));
                    break;
                case EVENT_WEST_BEACON_NEUTRAL_ALLIANCE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_WEST_BEACON);
                    sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_LOOSE_WEST_A));
                    break;
                case EVENT_WEST_BEACON_NEUTRAL_HORDE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_WEST_BEACON);
                    sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_LOOSE_WEST_H));
                    break;
            }
            break;
    }
}

void WorldPvPZM::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_EAST_BEACON_CONTROLLER:
            if (uiData == ALLIANCE)
            {
                if (GetData(TYPE_EAST_BEACON_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersHorde;
                    DoSetBeaconArtkit(m_BeamEastRedGUID, false);
                    m_uiEastBeaconWorldState = WORLD_STATE_TOWER_EAST_NEUTRAL;
                    m_uiEastBeaconMapState = WORLD_STATE_BEACON_EAST_NEUTRAL;
                }
                else if (GetData(TYPE_EAST_BEACON_STATE) == PROGRESS)
                {
                    m_uiEastBeaconWorldState = WORLD_STATE_TOWER_EAST_ALY;
                    m_uiEastBeaconMapState = WORLD_STATE_BEACON_EAST_ALY;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_EAST_BEACON_CONTROLLER))
                    {
                        DoSetBeaconArtkit(m_BeamEastBlueGUID, true);
                        ++m_uiTowersAlly;
                    }
                }
            }
            else if (uiData == HORDE)
            {
                if (GetData(TYPE_EAST_BEACON_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersAlly;
                    DoSetBeaconArtkit(m_BeamEastBlueGUID, false);
                    m_uiEastBeaconWorldState = WORLD_STATE_TOWER_EAST_NEUTRAL;
                    m_uiEastBeaconMapState = WORLD_STATE_BEACON_EAST_NEUTRAL;
                }
                if (GetData(TYPE_EAST_BEACON_STATE) == PROGRESS)
                {
                    m_uiEastBeaconWorldState = WORLD_STATE_TOWER_EAST_HORDE;
                    m_uiEastBeaconMapState = WORLD_STATE_BEACON_EAST_HORDE;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_EAST_BEACON_CONTROLLER))
                    {
                        DoSetBeaconArtkit(m_BeamEastRedGUID, true);
                        ++m_uiTowersHorde;
                    }
                }
            }

            // set controller only for progress and neutral
            if (GetData(TYPE_EAST_BEACON_STATE) == PROGRESS)
                m_uiEastBeaconController = GetData(TYPE_EAST_BEACON_STATE) == PROGRESS ? uiData : 0;

            break;
        case TYPE_WEST_BEACON_CONTROLLER:
            if (uiData == ALLIANCE)
            {
                if (GetData(TYPE_WEST_BEACON_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersHorde;
                    DoSetBeaconArtkit(m_BeamWestRedGUID, false);
                    m_uiWestBeaconWorldState = WORLD_STATE_TOWER_WEST_NEUTRAL;
                    m_uiWestBeaconMapState = WORLD_STATE_BEACON_WEST_NEUTRAL;
                }
                else if (GetData(TYPE_WEST_BEACON_STATE) == PROGRESS)
                {
                    m_uiWestBeaconWorldState = WORLD_STATE_TOWER_WEST_ALY;
                    m_uiWestBeaconMapState = WORLD_STATE_BEACON_WEST_ALY;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_WEST_BEACON_CONTROLLER))
                    {
                        DoSetBeaconArtkit(m_BeamWestBlueGUID, true);
                        ++m_uiTowersAlly;
                    }
                }
            }
            else if (uiData == HORDE)
            {
                if (GetData(TYPE_WEST_BEACON_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersAlly;
                    DoSetBeaconArtkit(m_BeamWestBlueGUID, false);
                    m_uiWestBeaconWorldState = WORLD_STATE_TOWER_WEST_NEUTRAL;
                    m_uiWestBeaconMapState = WORLD_STATE_BEACON_WEST_NEUTRAL;
                }
                if (GetData(TYPE_WEST_BEACON_STATE) == PROGRESS)
                {
                    m_uiWestBeaconWorldState = WORLD_STATE_TOWER_WEST_HORDE;
                    m_uiWestBeaconMapState = WORLD_STATE_BEACON_WEST_HORDE;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_WEST_BEACON_CONTROLLER))
                    {
                        DoSetBeaconArtkit(m_BeamWestRedGUID, true);
                        ++m_uiTowersHorde;
                    }
                }
            }

            // set controller only for progress and neutral
            if (GetData(TYPE_WEST_BEACON_STATE) == PROGRESS)
                m_uiWestBeaconController = GetData(TYPE_WEST_BEACON_STATE) == PROGRESS ? uiData : 0;

            break;
        case TYPE_EAST_BEACON_STATE:
            m_uiEastBeaconState = uiData;
            return;
        case TYPE_WEST_BEACON_STATE:
            m_uiWestBeaconState = uiData;
            return;
    }

    // buff players
    if (m_uiTowersAlly == MAX_ZM_TOWERS)
        DoPrepareFactionScouts(ALLIANCE);
    else if (m_uiTowersHorde == MAX_ZM_TOWERS)
        DoPrepareFactionScouts(HORDE);

    // debuff players if towers == 0; spell to remove will be always the first
    if (m_uiTowersHorde < MAX_ZM_TOWERS)
        DoResetScouts(HORDE);
    if (m_uiTowersAlly < MAX_ZM_TOWERS)
        DoResetScouts(ALLIANCE);
}

uint32 WorldPvPZM::GetData(uint32 uiType)
{
    switch (uiType)
    {
        case TYPE_EAST_BEACON_CONTROLLER:
            return m_uiEastBeaconController;
        case TYPE_WEST_BEACON_CONTROLLER:
            return m_uiWestBeaconController;
        case TYPE_EAST_BEACON_STATE:
            return m_uiEastBeaconState;
        case TYPE_WEST_BEACON_STATE:
            return m_uiWestBeaconState;
    }

    return 0;
}

void WorldPvPZM::ProcessCaptureEvent(uint32 uiCaptureType, uint32 uiTeam, uint32 uiTower)
{
    switch(uiTower)
    {
        case TOWER_ID_EAST_BEACON:
             // remove old tower state
            SendUpdateWorldState(m_uiEastBeaconWorldState, 0);
            SendUpdateWorldState(m_uiEastBeaconMapState, 0);
            // update data
            SetData(TYPE_EAST_BEACON_STATE, uiCaptureType);
            SetData(TYPE_EAST_BEACON_CONTROLLER, uiTeam);
            // send new tower state
            SendUpdateWorldState(m_uiEastBeaconWorldState, 1);
            SendUpdateWorldState(m_uiEastBeaconMapState, 1);
            break;
        case TOWER_ID_WEST_BEACON:
            // remove old tower state
            SendUpdateWorldState(m_uiWestBeaconWorldState, 0);
            SendUpdateWorldState(m_uiWestBeaconMapState, 0);
            // update data
            SetData(TYPE_WEST_BEACON_STATE, uiCaptureType);
            SetData(TYPE_WEST_BEACON_CONTROLLER, uiTeam);
            // send new tower state
            SendUpdateWorldState(m_uiWestBeaconWorldState, 1);
            SendUpdateWorldState(m_uiWestBeaconMapState, 1);
            break;
    }
}

void WorldPvPZM::DoPrepareFactionScouts(uint32 uiFaction)
{
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    if (uiFaction == ALLIANCE)
    {
        if (Creature* pScout = pPlayer->GetMap()->GetCreature(m_AllianceScountGUID))
            pScout->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        SendUpdateWorldState(m_uiAllianceScoutWorldState, 0);
        m_uiAllianceScoutWorldState = WORLD_STATE_ALY_FLAG_READY;
        SendUpdateWorldState(m_uiAllianceScoutWorldState, 1);
    }
    else if (uiFaction == HORDE)
    {
        if (Creature* pScout = pPlayer->GetMap()->GetCreature(m_HorderScountGUID))
            pScout->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        SendUpdateWorldState(m_uiHordeScoutWorldState, 0);
        m_uiHordeScoutWorldState = WORLD_STATE_HORDE_FLAG_READY;
        SendUpdateWorldState(m_uiHordeScoutWorldState, 1);
    }
}

void WorldPvPZM::DoResetScouts(uint32 uiFaction, bool bIncludeWorldStates)
{
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    if (uiFaction == ALLIANCE)
    {
        if (Creature* pScout = pPlayer->GetMap()->GetCreature(m_AllianceScountGUID))
            pScout->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        // reset world states only if requested
        if (bIncludeWorldStates)
        {
            SendUpdateWorldState(m_uiAllianceScoutWorldState, 0);
            m_uiAllianceScoutWorldState = WORLD_STATE_ALY_FLAG_NOT_READY;
            SendUpdateWorldState(m_uiAllianceScoutWorldState, 1);
        }
    }
    else if (uiFaction == HORDE)
    {
        if (Creature* pScout = pPlayer->GetMap()->GetCreature(m_HorderScountGUID))
            pScout->RemoveFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_GOSSIP);

        // reset world states only if requested
        if (bIncludeWorldStates)
        {
            SendUpdateWorldState(m_uiHordeScoutWorldState, 0);
            m_uiHordeScoutWorldState = WORLD_STATE_HORDE_FLAG_NOT_READY;
            SendUpdateWorldState(m_uiHordeScoutWorldState, 1);
        }
    }
}

bool WorldPvPZM::HandleObjectUse(Player* pPlayer, GameObject* pGo)
{
    if (!pPlayer->HasAura(pPlayer->GetTeam() == ALLIANCE ? SPELL_BATTLE_STANDARD_ALY : SPELL_BATTLE_STANDARD_HORDE))
        return false;

    switch (pGo->GetEntry())
    {
        case GO_ZANGA_BANNER_CENTER_ALY:
            // clicked only by the horde
            if (pPlayer->GetTeam() == ALLIANCE)
                return false;

            // change banners
            SendUpdateWorldState(m_uiGraveyardWorldState, 0);
            DoHandleBanners(m_TowerBannerCenterAlyGUID, false);
            DoHandleBanners(m_TowerBannerCenterHordeGUID, true);
            DoSetBeaconArtkit(m_BeamCenterBlueGUID, false);
            sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_LOOSE_GY_A));

            // remove buff and graveyard from ally
            DoProcessTeamBuff(ALLIANCE, SPELL_TWIN_SPIRE_BLESSING, true);
            DoSetGraveyard(ALLIANCE, true);

            // add the buff and the graveyard to horde
            m_uiGraveyardWorldState = WORLD_STATE_GRAVEYARD_HORDE;
            SendUpdateWorldState(m_uiGraveyardWorldState, 1);
            DoProcessTeamBuff(HORDE, SPELL_TWIN_SPIRE_BLESSING);
            DoSetGraveyard(HORDE);

            // reset scout and remove player aura
            DoResetScouts(HORDE);
            pPlayer->RemoveAurasDueToSpell(SPELL_BATTLE_STANDARD_HORDE);
            DoSetBeaconArtkit(m_BeamCenterRedGUID, true);
            sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_GY_H));

            return true;
        case GO_ZANGA_BANNER_CENTER_HORDE:
            // clicked only by the alliance
            if (pPlayer->GetTeam() == HORDE)
                return false;

            // change banners
            SendUpdateWorldState(m_uiGraveyardWorldState, 0);
            DoHandleBanners(m_TowerBannerCenterHordeGUID, false);
            DoHandleBanners(m_TowerBannerCenterAlyGUID, true);
            DoSetBeaconArtkit(m_BeamCenterRedGUID, false);
            sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_LOOSE_GY_H));

            // remove buff and graveyard from ally
            DoProcessTeamBuff(HORDE, SPELL_TWIN_SPIRE_BLESSING, true);
            DoSetGraveyard(HORDE, true);

            // add the buff and the graveyard to horde
            m_uiGraveyardWorldState = WORLD_STATE_GRAVEYARD_ALY;
            SendUpdateWorldState(m_uiGraveyardWorldState, 1);
            DoProcessTeamBuff(ALLIANCE, SPELL_TWIN_SPIRE_BLESSING);
            DoSetGraveyard(ALLIANCE);

            // reset scout and remove player aura
            DoResetScouts(ALLIANCE);
            pPlayer->RemoveAurasDueToSpell(SPELL_BATTLE_STANDARD_ALY);
            DoSetBeaconArtkit(m_BeamCenterBlueGUID, true);
            sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_GY_A));

            return true;
        case GO_ZANGA_BANNER_CENTER_NEUTRAL:

            // remove old world state
            SendUpdateWorldState(m_uiGraveyardWorldState, 0);

            if (pPlayer->GetTeam() == ALLIANCE)
            {
                // change banners
                DoHandleBanners(m_TowerBannerCenterNeutralGUID, false);
                DoHandleBanners(m_TowerBannerCenterAlyGUID, true);

                // add the buff and the graveyard to horde
                m_uiGraveyardWorldState= WORLD_STATE_GRAVEYARD_ALY;
                DoProcessTeamBuff(ALLIANCE, SPELL_TWIN_SPIRE_BLESSING);
                DoSetGraveyard(ALLIANCE);

                // reset scout and remove player aura
                DoResetScouts(ALLIANCE);
                pPlayer->RemoveAurasDueToSpell(SPELL_BATTLE_STANDARD_ALY);
                DoSetBeaconArtkit(m_BeamCenterBlueGUID, true);
                sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_GY_H));
            }
            else if (pPlayer->GetTeam() == HORDE)
            {
                // change banners
                DoHandleBanners(m_TowerBannerCenterNeutralGUID, false);
                DoHandleBanners(m_TowerBannerCenterHordeGUID, true);

                // add the buff and the graveyard to horde
                m_uiGraveyardWorldState = WORLD_STATE_GRAVEYARD_HORDE;
                DoProcessTeamBuff(HORDE, SPELL_TWIN_SPIRE_BLESSING);
                DoSetGraveyard(HORDE);

                // reset scout and remove player aura
                DoResetScouts(HORDE);
                pPlayer->RemoveAurasDueToSpell(SPELL_BATTLE_STANDARD_HORDE);
                DoSetBeaconArtkit(m_BeamCenterRedGUID, true);
                sWorld.SendZoneText(ZONE_ID_ZANGARMARSH, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_ZM_CAPTURE_GY_H));
            }

            // add new world state
            SendUpdateWorldState(m_uiGraveyardWorldState, 1);

            return true;
    }

    return false;
}

void WorldPvPZM::DoHandleBanners(ObjectGuid BannerGuid, bool bRespawn)
{
    // neet to use a player as anchor for the map
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    if (GameObject* pBanner = pPlayer->GetMap()->GetGameObject(BannerGuid))
    {
        if (bRespawn)
        {
            pBanner->SetRespawnTime(7*DAY);
            pBanner->Refresh();
        }
        else if (pBanner->isSpawned())
            pBanner->Delete();
    }
}

void WorldPvPZM::DoSetBeaconArtkit(ObjectGuid BeaconGuid, bool bRespawn)
{
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    if (Creature* pBeam = pPlayer->GetMap()->GetCreature(BeaconGuid))
    {
        if (bRespawn)
            pBeam->Respawn();
        else
            pBeam->ForcedDespawn();
    }
}
