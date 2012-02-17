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
#include "WorldPvPTF.h"


WorldPvPTF::WorldPvPTF() : WorldPvP(),
    m_uiControllerWorldState(WORLD_STATE_TF_TOWERS_CONTROLLED),
    m_uiZoneOwner(TEAM_NONE),
    m_uiZoneUpdateTimer(TIMER_TF_UPDATE_TIME),
    m_uiZoneLockTimer(0),
    m_uiTowersAlliance(0),
    m_uiTowersHorde(0)
{
    m_uiTypeId = WORLD_PVP_TYPE_TF;

    m_uiTowerWorldState[0] = WORLD_STATE_TOWER_1_NEUTRAL;
    m_uiTowerWorldState[1] = WORLD_STATE_TOWER_2_NEUTRAL;
    m_uiTowerWorldState[2] = WORLD_STATE_TOWER_3_NEUTRAL;
    m_uiTowerWorldState[3] = WORLD_STATE_TOWER_4_NEUTRAL;
    m_uiTowerWorldState[4] = WORLD_STATE_TOWER_5_NEUTRAL;

    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
        m_uiTowerOwner[i] = TEAM_NONE;
}

bool WorldPvPTF::InitWorldPvPArea()
{
    RegisterZone(ZONE_ID_TEROKKAR);
    RegisterZone(ZONE_ID_SHADOW_LABYRINTH);
    RegisterZone(ZONE_ID_AUCHENAI_CRYPTS);
    RegisterZone(ZONE_ID_MANA_TOMBS);
    RegisterZone(ZONE_ID_SETHEKK_HALLS);

    return true;
}

void WorldPvPTF::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, m_uiControllerWorldState, WORLD_STATE_ADD);
    if (m_uiControllerWorldState == WORLD_STATE_TF_TOWERS_CONTROLLED)
    {
        FillInitialWorldState(data, count, WORLD_STATE_TF_TOWER_COUNT_H, m_uiTowersHorde);
        FillInitialWorldState(data, count, WORLD_STATE_TF_TOWER_COUNT_A, m_uiTowersAlliance);
    }
    else
        UpdateTimerWorldState();

    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
        FillInitialWorldState(data, count, m_uiTowerWorldState[i], WORLD_STATE_ADD);
}

void WorldPvPTF::SendRemoveWorldStates(Player* pPlayer)
{
    pPlayer->SendUpdateWorldState(m_uiControllerWorldState, WORLD_STATE_REMOVE);

    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
        pPlayer->SendUpdateWorldState(m_uiTowerWorldState[i], WORLD_STATE_REMOVE);
}

void WorldPvPTF::UpdateWorldState(uint8 uiValue)
{
    // update only tower count; tower states are sent in the process event
    SendUpdateWorldState(m_uiControllerWorldState, uiValue);
    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
        SendUpdateWorldState(m_uiTowerWorldState[i], uiValue);
}

void WorldPvPTF::HandlePlayerEnterZone(Player* pPlayer)
{
    WorldPvP::HandlePlayerEnterZone(pPlayer);

    // remove the buff from the player first because there are some issues at relog
    pPlayer->RemoveAurasDueToSpell(SPELL_AUCHINDOUN_BLESSING);

    // Handle the buffs
    if (m_uiZoneOwner == pPlayer->GetTeam() && m_uiZoneOwner != TEAM_NONE)
        pPlayer->CastSpell(pPlayer, SPELL_AUCHINDOUN_BLESSING, true);
}

void WorldPvPTF::HandlePlayerLeaveZone(Player* pPlayer)
{
    // remove the buff from the player
    pPlayer->RemoveAurasDueToSpell(SPELL_AUCHINDOUN_BLESSING);

    WorldPvP::HandlePlayerLeaveZone(pPlayer);
}

void WorldPvPTF::OnGameObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_TEROKKAR_BANNER_1:
            m_TowerBannerGUID[0] = pGo->GetObjectGuid();
            if (m_uiTowerOwner[0] == TEAM_NONE)
                pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
            else
                pGo->SetGoArtKit(m_uiTowerOwner[0] == ALLIANCE ? GO_ARTKIT_BANNER_ALLIANCE : GO_ARTKIT_BANNER_HORDE);
            break;
        case GO_TEROKKAR_BANNER_2:
            m_TowerBannerGUID[1] = pGo->GetObjectGuid();
            if (m_uiTowerOwner[1] == TEAM_NONE)
                pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
            else
                pGo->SetGoArtKit(m_uiTowerOwner[1] == ALLIANCE ? GO_ARTKIT_BANNER_ALLIANCE : GO_ARTKIT_BANNER_HORDE);
            break;
        case GO_TEROKKAR_BANNER_3:
            m_TowerBannerGUID[2] = pGo->GetObjectGuid();
            if (m_uiTowerOwner[2] == TEAM_NONE)
                pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
            else
                pGo->SetGoArtKit(m_uiTowerOwner[2] == ALLIANCE ? GO_ARTKIT_BANNER_ALLIANCE : GO_ARTKIT_BANNER_HORDE);
            break;
        case GO_TEROKKAR_BANNER_4:
            m_TowerBannerGUID[3] = pGo->GetObjectGuid();
            if (m_uiTowerOwner[2] == TEAM_NONE)
                pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
            else
                pGo->SetGoArtKit(m_uiTowerOwner[2] == ALLIANCE ? GO_ARTKIT_BANNER_ALLIANCE : GO_ARTKIT_BANNER_HORDE);
            break;
        case GO_TEROKKAR_BANNER_5:
            m_TowerBannerGUID[4] = pGo->GetObjectGuid();
            if (m_uiTowerOwner[2] == TEAM_NONE)
                pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
            else
                pGo->SetGoArtKit(m_uiTowerOwner[2] == ALLIANCE ? GO_ARTKIT_BANNER_ALLIANCE : GO_ARTKIT_BANNER_HORDE);
            break;
        default:
            return;
    }

    pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
}

void WorldPvPTF::HandleObjectiveComplete(uint32 uiEventId, std::list<Player*> players, Team faction)
{
    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
    {
        for (uint8 j = 0; j < 4; ++j)
        {
            if (uiEventId == aTerokkarTowerEvents[i][j].uiEventEntry)
            {
                for (std::list<Player*>::iterator itr = players.begin(); itr != players.end(); ++itr)
                {
                    if ((*itr) && (*itr)->GetTeam() == faction)
                        (*itr)->AreaExploredOrEventHappens(faction == ALLIANCE ? QUEST_SPIRITS_OF_AUCHINDOUM_ALLIANCE : QUEST_SPIRITS_OF_AUCHINDOUM_HORDE);
                }
            }
        }
    }
}

// process the capture events
void WorldPvPTF::ProcessEvent(uint32 uiEventId, GameObject* pGo)
{
    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
    {
        if (pGo->GetEntry() == aTerokkarTowers[i])
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (uiEventId == aTerokkarTowerEvents[i][j].uiEventEntry && aTerokkarTowerEvents[i][j].faction != m_uiTowerOwner[i])
                {
                    ProcessCaptureEvent(aTerokkarTowerEvents[i][j].faction, aTerokkarTowerEvents[i][j].uiWorldState, i);
                    sWorld.SendZoneText(ZONE_ID_TEROKKAR, sObjectMgr.GetMangosStringForDBCLocale(aTerokkarTowerEvents[i][j].uiZoneText));
                    break;
                }
            }
        }
    }
}

void WorldPvPTF::ProcessCaptureEvent(Team faction, uint32 uiNewWorldState, uint32 uiTower)
{
    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
    {
        if (uiTower == i)
        {
            // remove old tower state
            SendUpdateWorldState(m_uiTowerWorldState[i], WORLD_STATE_REMOVE);

            if (faction != TEAM_NONE)
            {
                SetBannerArtKit(m_TowerBannerGUID[i], faction == ALLIANCE ? GO_ARTKIT_BANNER_ALLIANCE : GO_ARTKIT_BANNER_HORDE);

                if (faction == ALLIANCE)
                    ++m_uiTowersAlliance;
                else
                    ++m_uiTowersHorde;
            }
            else
            {
                SetBannerArtKit(m_TowerBannerGUID[i], GO_ARTKIT_BANNER_NEUTRAL);

                if (m_uiTowerOwner[i] == ALLIANCE)
                    --m_uiTowersAlliance;
                else
                    --m_uiTowersHorde;
            }

            // update tower state
            m_uiTowerOwner[i] = faction;
            m_uiTowerWorldState[i] = uiNewWorldState;
            SendUpdateWorldState(m_uiTowerWorldState[i], WORLD_STATE_ADD);
        }
    }

    // update towers count
    SendUpdateWorldState(WORLD_STATE_TF_TOWER_COUNT_A, m_uiTowersAlliance);
    SendUpdateWorldState(WORLD_STATE_TF_TOWER_COUNT_H, m_uiTowersHorde);

    // If all towers are captured then process event
    if (m_uiTowersAlliance == MAX_TF_TOWERS)
    {
        SendUpdateWorldState(m_uiControllerWorldState, WORLD_STATE_REMOVE);
        m_uiControllerWorldState = WORLD_STATE_TF_LOCKED_ALLIANCE;
        SendUpdateWorldState(m_uiControllerWorldState, WORLD_STATE_ADD);

        m_uiZoneLockTimer = TIMER_TF_LOCK_TIME;
        UpdateTimerWorldState();

        m_uiZoneOwner = ALLIANCE;
        DoProcessTeamBuff(ALLIANCE, SPELL_AUCHINDOUN_BLESSING);

        // lock the towers
        for (uint8 i = 0; i < MAX_TF_TOWERS; i++)
            LockTower(aTerokkarTowers[i], ALLIANCE);
    }
    else if (m_uiTowersHorde == MAX_TF_TOWERS)
    {
        SendUpdateWorldState(m_uiControllerWorldState, WORLD_STATE_REMOVE);
        m_uiControllerWorldState = WORLD_STATE_TF_LOCKED_HORDE;
        SendUpdateWorldState(m_uiControllerWorldState, WORLD_STATE_ADD);

        m_uiZoneLockTimer = TIMER_TF_LOCK_TIME;
        UpdateTimerWorldState();

        m_uiZoneOwner = HORDE;
        DoProcessTeamBuff(HORDE, SPELL_AUCHINDOUN_BLESSING);

        // lock the towers
        for (uint8 i = 0; i < MAX_TF_TOWERS; i++)
            LockTower(aTerokkarTowers[i], HORDE);
    }
}

void WorldPvPTF::Update(uint32 diff)
{
    if (m_uiZoneLockTimer)
    {
        if (m_uiZoneLockTimer < diff)
        {
            // remove buffs
            DoProcessTeamBuff(ALLIANCE, SPELL_AUCHINDOUN_BLESSING, true);
            DoProcessTeamBuff(HORDE, SPELL_AUCHINDOUN_BLESSING, true);

            // reset world states and towers
            UpdateWorldState(0);
            m_uiZoneOwner = TEAM_NONE;
            m_uiControllerWorldState = WORLD_STATE_TF_TOWERS_CONTROLLED;
            m_uiTowersAlliance = 0;
            m_uiTowersHorde = 0;
            m_uiTowerWorldState[0] = WORLD_STATE_TOWER_1_NEUTRAL;
            m_uiTowerWorldState[1] = WORLD_STATE_TOWER_2_NEUTRAL;
            m_uiTowerWorldState[2] = WORLD_STATE_TOWER_3_NEUTRAL;
            m_uiTowerWorldState[3] = WORLD_STATE_TOWER_4_NEUTRAL;
            m_uiTowerWorldState[4] = WORLD_STATE_TOWER_5_NEUTRAL;
            UpdateWorldState(1);

            // update towers count
            SendUpdateWorldState(WORLD_STATE_TF_TOWER_COUNT_A, m_uiTowersAlliance);
            SendUpdateWorldState(WORLD_STATE_TF_TOWER_COUNT_H, m_uiTowersHorde);

            for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
                ResetTower(i);

            m_uiZoneLockTimer = 0;
        }
        else
        {
            if (m_uiZoneUpdateTimer < diff)
            {
                // update timer
                UpdateTimerWorldState();

                m_uiZoneUpdateTimer = TIMER_TF_UPDATE_TIME;
            }
            else
                m_uiZoneUpdateTimer -= diff;

            m_uiZoneLockTimer -= diff;
        }
    }
}

void WorldPvPTF::UpdateTimerWorldState()
{
    // Calculate time
    uint32 minutesLeft = m_uiZoneLockTimer / 60000;
    uint32 hoursLeft = minutesLeft / 60;
    minutesLeft -= hoursLeft * 60;
    uint32 firstDigit = minutesLeft / 10;

    SendUpdateWorldState(WORLD_STATE_TF_TIME_MIN_FIRST_DIGIT, firstDigit);
    SendUpdateWorldState(WORLD_STATE_TF_TIME_MIN_SECOND_DIGIT, minutesLeft - firstDigit * 10);
    SendUpdateWorldState(WORLD_STATE_TF_TIME_HOURS, hoursLeft);
}

void WorldPvPTF::SetBannerArtKit(ObjectGuid BannerGuid, uint32 uiArtkit)
{
    // neet to use a player as anchor for the map
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    if (GameObject* pBanner = pPlayer->GetMap()->GetGameObject(BannerGuid))
    {
        pBanner->SetGoArtKit(uiArtkit);
        pBanner->Refresh();
    }
}

void WorldPvPTF::LockTower(uint8 uiTowerId, Team faction)
{
    // neet to use a player as anchor for the map
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    if (GameObject* pBanner = pPlayer->GetMap()->GetGameObject(m_TowerBannerGUID[uiTowerId]))
        pBanner->SetLootState(GO_JUST_DEACTIVATED);

    SetCapturePointSliderValue(aTerokkarTowers[uiTowerId], faction == ALLIANCE ? CAPTURE_SLIDER_ALLIANCE_LOCKED : CAPTURE_SLIDER_HORDE_LOCKED);
}

void WorldPvPTF::ResetTower(uint8 uiTowerId)
{
    // neet to use a player as anchor for the map
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    if (GameObject* pBanner = pPlayer->GetMap()->GetGameObject(m_TowerBannerGUID[uiTowerId]))
    {
        pBanner->SetCapturePointSlider(CAPTURE_SLIDER_RESET);
        pBanner->Refresh();
    }
    else
        // if grid is unloaded, resetting the slider value is enough
        SetCapturePointSliderValue(aTerokkarTowers[uiTowerId], CAPTURE_SLIDER_RESET);
}
