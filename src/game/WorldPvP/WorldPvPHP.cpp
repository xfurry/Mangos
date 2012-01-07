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
#include "WorldPvPHP.h"


WorldPvPHP::WorldPvPHP() : WorldPvP(),
    m_uiBrokenHillController(0),
    m_uiStadiumController(0),
    m_uiOverlookController(0),

    m_uiBrokenHillState(NEUTRAL),
    m_uiStadiumState(NEUTRAL),
    m_uiOverlookState(NEUTRAL),

    m_uiBrokenHillWorldState(WORLD_STATE_BROKEN_HILL_NEUTRAL),
    m_uiStadiumWorldState(WORLD_STATE_STADIUM_NEUTRAL),
    m_uiOverlookWorldState(WORLD_STATE_OVERLOOK_NEUTRAL),

    m_uiTowersAlly(0),
    m_uiTowersHorde(0)
{
    m_uiTypeId = WORLD_PVP_TYPE_HP;
}

bool WorldPvPHP::InitOutdoorPvPArea()
{
    RegisterZone(ZONE_ID_HELLFIRE_PENINSULA);
    RegisterZone(ZONE_ID_HELLFIRE_CITADEL);
    RegisterZone(ZONE_ID_HELLFIRE_RAMPARTS);
    RegisterZone(ZONE_ID_BLOOD_FURNACE);
    RegisterZone(ZONE_ID_SHATTERED_HALLS);
    RegisterZone(ZONE_ID_MAGTHERIDON_LAIR);

    return true;
}

void WorldPvPHP::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_COUNT_HP_ALY,   m_uiTowersAlly);
    FillInitialWorldState(data, count, WORLD_STATE_TOWER_COUNT_HP_HORDE, m_uiTowersHorde);

    FillInitialWorldState(data, count, m_uiBrokenHillWorldState,    1);
    FillInitialWorldState(data, count, m_uiStadiumWorldState,       1);
    FillInitialWorldState(data, count, m_uiOverlookWorldState,      1);
}

void WorldPvPHP::SendRemoveWorldStates(Player* pPlayer)
{
    pPlayer->SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HP_ALY,      0);
    pPlayer->SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HP_HORDE,    0);

    pPlayer->SendUpdateWorldState(m_uiBrokenHillWorldState,         0);
    pPlayer->SendUpdateWorldState(m_uiStadiumWorldState,            0);
    pPlayer->SendUpdateWorldState(m_uiOverlookWorldState,           0);
}

void WorldPvPHP::UpdateWorldState()
{
    // update only tower count; tower states is updated in the process event
    SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HP_ALY,   m_uiTowersAlly);
    SendUpdateWorldState(WORLD_STATE_TOWER_COUNT_HP_HORDE, m_uiTowersHorde);
}

void WorldPvPHP::HandlePlayerEnterZone(Player* pPlayer)
{
    // cast buff the the player which enters the zone
    if ((pPlayer->GetTeam() == ALLIANCE ? m_uiTowersAlly : m_uiTowersHorde) == MAX_HP_TOWERS)
        pPlayer->CastSpell(pPlayer, pPlayer->GetTeam() == ALLIANCE ? SPELL_HELLFIRE_SUPERIORITY_ALY : SPELL_HELLFIRE_SUPERIORITY_HORDE, true);

    WorldPvP::HandlePlayerEnterZone(pPlayer);
}

void WorldPvPHP::HandlePlayerLeaveZone(Player* pPlayer)
{
    // remove the buff from the player
    pPlayer->RemoveAurasDueToSpell(pPlayer->GetTeam() == ALLIANCE ? SPELL_HELLFIRE_SUPERIORITY_ALY : SPELL_HELLFIRE_SUPERIORITY_HORDE);

    WorldPvP::HandlePlayerLeaveZone(pPlayer);
}

void WorldPvPHP::OnGameObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_TOWER_BANNER_OVERLOOK:
            m_TowerBannerOverlookGUID = pGo->GetObjectGuid();
            break;
        case GO_TOWER_BANNER_STADIUM:
            m_TowerBannerStadiumGUID = pGo->GetObjectGuid();
            break;
        case GO_TOWER_BANNER_BROKEN_HILL:
            m_TowerBannerBrokenHillGUID = pGo->GetObjectGuid();
            break;
        case GO_HELLFIRE_BANNER_OVERLOOK:
            m_TowerPointOverlookGUID = pGo->GetObjectGuid();
            break;
        case GO_HELLFIRE_BANNER_STADIUM:
            m_TowerPointStadiumGUID = pGo->GetObjectGuid();
            break;
        case GO_HELLFIRE_BANNER_BROKEN_HILL:
            m_TowerPointBrokenHillGUID = pGo->GetObjectGuid();
            break;

    }
}

void WorldPvPHP::HandleObjectiveComplete(PlayerSet m_sPlayersSet, uint32 uiEventId)
{
    uint32 uiCredit = 0;

    switch(uiEventId)
    {
        case EVENT_OVERLOOK_PROGRESS_ALLIANCE:
        case EVENT_OVERLOOK_PROGRESS_HORDE:
            uiCredit = NPC_CAPTURE_CREDIT_OVERLOOK;
            break;
        case EVENT_STADIUM_PROGRESS_ALLIANCE:
        case EVENT_STADIUM_PROGRESS_HORDE:
            uiCredit = NPC_CAPTURE_CREDIT_STADIUM;
            break;
        case EVENT_BROKEN_HILL_PROGRESS_ALLIANCE:
        case EVENT_BROKEN_HILL_PROGRESS_HORDE:
            uiCredit = NPC_CAPTURE_CREDIT_BROKEN_HILL;
            break;
    }

    if (!uiCredit)
        return;

    for (PlayerSet::iterator itr = m_sPlayersSet.begin(); itr != m_sPlayersSet.end(); ++itr)
    {
        if (!(*itr))
            continue;

        (*itr)->KilledMonsterCredit(uiCredit);
        (*itr)->RewardHonor(NULL, 1, HONOR_REWARD_HELLFIRE);
    }
}

// process the capture events
void WorldPvPHP::ProcessEvent(GameObject* pGo, Player* pPlayer, uint32 uiEventId)
{
    switch(pGo->GetEntry())
    {
        case GO_TOWER_BANNER_OVERLOOK:
            switch(uiEventId)
            {
                case EVENT_OVERLOOK_PROGRESS_ALLIANCE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_OVERLOOK);
                    sWorld.SendZoneText(ZONE_ID_HELLFIRE_PENINSULA, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_HP_CAPTURE_OVERLOOK_A));
                    break;
                case EVENT_OVERLOOK_PROGRESS_HORDE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_OVERLOOK);
                    sWorld.SendZoneText(ZONE_ID_HELLFIRE_PENINSULA, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_HP_CAPTURE_OVERLOOK_H));
                    break;
                case EVENT_OVERLOOK_NEUTRAL_ALLIANCE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_OVERLOOK);
                    sWorld.SendZoneText(ZONE_ID_HELLFIRE_PENINSULA, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_HP_LOOSE_OVERLOOK_A));
                    break;
                case EVENT_OVERLOOK_NEUTRAL_HORDE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_OVERLOOK);
                    sWorld.SendZoneText(ZONE_ID_HELLFIRE_PENINSULA, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_HP_LOOSE_OVERLOOK_H));
                    break;
            }
            break;
        case GO_TOWER_BANNER_STADIUM:
            switch(uiEventId)
            {
                case EVENT_STADIUM_PROGRESS_ALLIANCE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_STADIUM);
                    sWorld.SendZoneText(ZONE_ID_HELLFIRE_PENINSULA, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_HP_CAPTURE_STADIUM_A));
                    break;
                case EVENT_STADIUM_PROGRESS_HORDE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_STADIUM);
                    sWorld.SendZoneText(ZONE_ID_HELLFIRE_PENINSULA, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_HP_CAPTURE_STADIUM_H));
                    break;
                case EVENT_STADIUM_NEUTRAL_ALLIANCE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_STADIUM);
                    sWorld.SendZoneText(ZONE_ID_HELLFIRE_PENINSULA, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_HP_LOOSE_STADIUM_A));
                    break;
                case EVENT_STADIUM_NEUTRAL_HORDE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_STADIUM);
                    sWorld.SendZoneText(ZONE_ID_HELLFIRE_PENINSULA, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_HP_LOOSE_STADIUM_H));
                    break;
            }
            break;
        case GO_TOWER_BANNER_BROKEN_HILL:
            switch(uiEventId)
            {
                case EVENT_BROKEN_HILL_PROGRESS_ALLIANCE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_BROKEN_HILL);
                    sWorld.SendZoneText(ZONE_ID_HELLFIRE_PENINSULA, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_HP_CAPTURE_BROKENHILL_A));
                    break;
                case EVENT_BROKEN_HILL_PROGRESS_HORDE:
                    ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam(), TOWER_ID_BROKEN_HILL);
                    sWorld.SendZoneText(ZONE_ID_HELLFIRE_PENINSULA, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_HP_CAPTURE_BROKENHILL_H));
                    break;
                case EVENT_BROKEN_HILL_NEUTRAL_ALLIANCE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_BROKEN_HILL);
                    sWorld.SendZoneText(ZONE_ID_HELLFIRE_PENINSULA, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_HP_LOOSE_BROKENHILL_A));
                    break;
                case EVENT_BROKEN_HILL_NEUTRAL_HORDE:
                    // handle event - in this case player's team will be the opposite team
                    ProcessCaptureEvent(NEUTRAL, pPlayer->GetTeam(), TOWER_ID_BROKEN_HILL);
                    sWorld.SendZoneText(ZONE_ID_HELLFIRE_PENINSULA, sObjectMgr.GetMangosStringForDBCLocale(LANG_OPVP_HP_LOOSE_BROKENHILL_H));
                    break;
            }
            break;
    }
}

void WorldPvPHP::SetData(uint32 uiType, uint32 uiData)
{
    switch(uiType)
    {
        case TYPE_OVERLOOK_CONTROLLER:
            if (uiData == ALLIANCE)
            {
                if (GetData(TYPE_OVERLOOK_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersHorde;
                    m_uiOverlookWorldState = WORLD_STATE_OVERLOOK_NEUTRAL;
                    // set artkit
                    SetBannerArtKit(m_TowerBannerOverlookGUID, GO_ARTKIT_BANNER_NEUTRAL);
                    SetBannerArtKit(m_TowerPointOverlookGUID, GO_ARTKIT_OVERLOOK_NEUTRAL);
                }
                else if (GetData(TYPE_OVERLOOK_STATE) == PROGRESS)
                {
                    m_uiOverlookWorldState = WORLD_STATE_OVERLOOK_ALY;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_OVERLOOK_CONTROLLER))
                    {
                        ++m_uiTowersAlly;
                        // set artkit
                        SetBannerArtKit(m_TowerBannerOverlookGUID, GO_ARTKIT_BANNER_ALLIANCE);
                        SetBannerArtKit(m_TowerPointOverlookGUID, GO_ARTKIT_OVERLOOK_ALY);
                    }
                }
            }
            else if (uiData == HORDE)
            {
                if (GetData(TYPE_OVERLOOK_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersAlly;
                    m_uiOverlookWorldState = WORLD_STATE_OVERLOOK_NEUTRAL;
                    // set artkit
                    SetBannerArtKit(m_TowerBannerOverlookGUID, GO_ARTKIT_BANNER_NEUTRAL);
                    SetBannerArtKit(m_TowerPointOverlookGUID, GO_ARTKIT_OVERLOOK_NEUTRAL);
                }
                if (GetData(TYPE_OVERLOOK_STATE) == PROGRESS)
                {
                    m_uiOverlookWorldState = WORLD_STATE_OVERLOOK_HORDE;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_OVERLOOK_CONTROLLER))
                    {
                        ++m_uiTowersHorde;

                        // set artkit
                        SetBannerArtKit(m_TowerBannerOverlookGUID, GO_ARTKIT_BANNER_HORDE);
                        SetBannerArtKit(m_TowerPointOverlookGUID, GO_ARTKIT_OVERLOOK_HORDE);
                    }
                }
            }

            // set controller only for progress and neutral
            if (GetData(TYPE_OVERLOOK_STATE) == PROGRESS)
                m_uiOverlookController = GetData(TYPE_OVERLOOK_STATE) == PROGRESS ? uiData : 0;

            break;
        case TYPE_STADIUM_CONTROLLER:
            if (uiData == ALLIANCE)
            {
                if (GetData(TYPE_STADIUM_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersHorde;
                    m_uiStadiumWorldState = WORLD_STATE_STADIUM_NEUTRAL;
                    // set artkit
                    SetBannerArtKit(m_TowerBannerOverlookGUID, GO_ARTKIT_BANNER_NEUTRAL);
                    SetBannerArtKit(m_TowerPointOverlookGUID, GO_ARTKIT_STADIUM_NEUTRAL);
                }
                else if (GetData(TYPE_STADIUM_STATE) == PROGRESS)
                {
                    m_uiStadiumWorldState = WORLD_STATE_STADIUM_ALY;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_STADIUM_CONTROLLER))
                    {
                        ++m_uiTowersAlly;

                        // set artkit
                        SetBannerArtKit(m_TowerBannerOverlookGUID, GO_ARTKIT_BANNER_ALLIANCE);
                        SetBannerArtKit(m_TowerPointOverlookGUID, GO_ARTKIT_STADIUM_ALY);
                    }
                }
            }
            else if (uiData == HORDE)
            {
                if (GetData(TYPE_STADIUM_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersAlly;
                    m_uiStadiumWorldState = WORLD_STATE_STADIUM_NEUTRAL;
                    // set artkit
                    SetBannerArtKit(m_TowerBannerOverlookGUID, GO_ARTKIT_BANNER_NEUTRAL);
                    SetBannerArtKit(m_TowerPointOverlookGUID, GO_ARTKIT_STADIUM_NEUTRAL);
                }
                if (GetData(TYPE_STADIUM_STATE) == PROGRESS)
                {
                    m_uiStadiumWorldState = WORLD_STATE_STADIUM_HORDE;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_STADIUM_CONTROLLER))
                    {
                        ++m_uiTowersHorde;

                        // set artkit
                        SetBannerArtKit(m_TowerBannerOverlookGUID, GO_ARTKIT_BANNER_HORDE);
                        SetBannerArtKit(m_TowerPointOverlookGUID, GO_ARTKIT_STADIUM_HORDE);
                    }
                }
            }

            // set controller only for progress and neutral
            if (GetData(TYPE_STADIUM_STATE) == PROGRESS)
                m_uiStadiumController = GetData(TYPE_STADIUM_STATE) == PROGRESS ? uiData : 0;

            break;
        case TYPE_BROKEN_HILL_CONTROLLER:
            if (uiData == ALLIANCE)
            {
                if (GetData(TYPE_BROKEN_HILL_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersHorde;
                    m_uiBrokenHillWorldState = WORLD_STATE_BROKEN_HILL_NEUTRAL;
                    // set artkit
                    SetBannerArtKit(m_TowerBannerOverlookGUID, GO_ARTKIT_BANNER_NEUTRAL);
                    SetBannerArtKit(m_TowerPointOverlookGUID, GO_ARTKIT_BROKEN_HILL_NEUTRAL);
                }
                else if (GetData(TYPE_BROKEN_HILL_STATE) == PROGRESS)
                {
                    m_uiBrokenHillWorldState = WORLD_STATE_BROKEN_HILL_ALY;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_BROKEN_HILL_CONTROLLER))
                    {
                        ++m_uiTowersAlly;

                        // set artkit
                        SetBannerArtKit(m_TowerBannerOverlookGUID, GO_ARTKIT_BANNER_ALLIANCE);
                        SetBannerArtKit(m_TowerPointOverlookGUID, GO_ARTKIT_BROKEN_HILL_ALY);
                    }
                }
            }
            else if (uiData == HORDE)
            {
                if (GetData(TYPE_BROKEN_HILL_STATE) == NEUTRAL)
                {
                    // in neutral case the team id is the opposite team
                    // the team who captured the tower and set it to neutral
                    --m_uiTowersAlly;
                    m_uiBrokenHillWorldState = WORLD_STATE_BROKEN_HILL_NEUTRAL;
                    // set artkit
                    SetBannerArtKit(m_TowerBannerOverlookGUID, GO_ARTKIT_BANNER_NEUTRAL);
                    SetBannerArtKit(m_TowerPointOverlookGUID, GO_ARTKIT_BROKEN_HILL_NEUTRAL);
                }
                if (GetData(TYPE_BROKEN_HILL_STATE) == PROGRESS)
                {
                    m_uiBrokenHillWorldState = WORLD_STATE_BROKEN_HILL_HORDE;
                    // increase tower count only if the controller is changed
                    if (uiData != GetData(TYPE_BROKEN_HILL_CONTROLLER))
                    {
                        ++m_uiTowersHorde;

                        // set artkit
                        SetBannerArtKit(m_TowerBannerOverlookGUID, GO_ARTKIT_BANNER_HORDE);
                        SetBannerArtKit(m_TowerPointOverlookGUID, GO_ARTKIT_BROKEN_HILL_HORDE);
                    }
                }
            }

            // set controller only for progress and neutral
            if (GetData(TYPE_BROKEN_HILL_STATE) == PROGRESS)
                m_uiBrokenHillController = GetData(TYPE_BROKEN_HILL_STATE) == PROGRESS ? uiData : 0;

            break;
        case TYPE_OVERLOOK_STATE:
            m_uiOverlookState = uiData;
            return;
        case TYPE_STADIUM_STATE:
            m_uiStadiumState = uiData;
            return;
        case TYPE_BROKEN_HILL_STATE:
            m_uiBrokenHillState = uiData;
            return;
    }

    // buff players
    if (m_uiTowersAlly == MAX_HP_TOWERS)
        DoProcessTeamBuff(ALLIANCE, SPELL_HELLFIRE_SUPERIORITY_ALY);
    else if (m_uiTowersHorde == MAX_HP_TOWERS)
        DoProcessTeamBuff(HORDE, SPELL_HELLFIRE_SUPERIORITY_HORDE);

    // debuff players if towers == 0; spell to remove will be always the first
    if (m_uiTowersHorde < MAX_HP_TOWERS)
        DoProcessTeamBuff(HORDE, SPELL_HELLFIRE_SUPERIORITY_ALY, true);
    if (m_uiTowersAlly < MAX_HP_TOWERS)
        DoProcessTeamBuff(ALLIANCE, SPELL_HELLFIRE_SUPERIORITY_HORDE, true);

    // update states counters
    // the map tower states are updated in the ProcessCaptureEvent function
    UpdateWorldState();
}

uint32 WorldPvPHP::GetData(uint32 uiType)
{
    switch (uiType)
    {
        case TYPE_OVERLOOK_CONTROLLER:
            return m_uiOverlookController;
        case TYPE_STADIUM_CONTROLLER:
            return m_uiStadiumController;
        case TYPE_BROKEN_HILL_CONTROLLER:
            return m_uiBrokenHillController;
        case TYPE_OVERLOOK_STATE:
            return m_uiOverlookState;
        case TYPE_STADIUM_STATE:
            return m_uiStadiumState;
        case TYPE_BROKEN_HILL_STATE:
            return m_uiBrokenHillState;
    }

    return 0;
}

void WorldPvPHP::ProcessCaptureEvent(uint32 uiCaptureType, uint32 uiTeam, uint32 uiTower)
{
    switch(uiTower)
    {
        case TOWER_ID_OVERLOOK:
             // remove old tower state
            SendUpdateWorldState(m_uiOverlookWorldState, 0);
            // update data
            SetData(TYPE_OVERLOOK_STATE, uiCaptureType);
            SetData(TYPE_OVERLOOK_CONTROLLER, uiTeam);
            // send new tower state
            SendUpdateWorldState(m_uiOverlookWorldState, 1);
            break;
        case TOWER_ID_STADIUM:
            // remove old tower state
            SendUpdateWorldState(m_uiStadiumWorldState, 0);
            // update data
            SetData(TYPE_STADIUM_STATE, uiCaptureType);
            SetData(TYPE_STADIUM_CONTROLLER, uiTeam);
            // send new tower state
            SendUpdateWorldState(m_uiStadiumWorldState, 1);
            break;
        case TOWER_ID_BROKEN_HILL:
            // remove old tower state
            SendUpdateWorldState(m_uiBrokenHillWorldState, 0);
            // update data
            SetData(TYPE_BROKEN_HILL_STATE, uiCaptureType);
            SetData(TYPE_BROKEN_HILL_CONTROLLER, uiTeam);
            // send new tower state
            SendUpdateWorldState(m_uiBrokenHillWorldState, 1);
            break;
    }
}

void WorldPvPHP::SetBannerArtKit(ObjectGuid BannerGuid, uint32 uiArtkit)
{
    // neet to use a player as anchor for the map
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    if (GameObject* pBanner = pPlayer->GetMap()->GetGameObject(BannerGuid))
        pBanner->SetGoArtKit(uiArtkit);
}
