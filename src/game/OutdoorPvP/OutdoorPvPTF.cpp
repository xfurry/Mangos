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

#include "OutdoorPvPTF.h"
#include "WorldPacket.h"
#include "../World.h"
#include "../Object.h"
#include "../Creature.h"
#include "../GameObject.h"
#include "../Player.h"

OutdoorPvPTF::OutdoorPvPTF() : OutdoorPvP(),
    m_zoneWorldState(WORLD_STATE_TF_TOWERS_CONTROLLED),
    m_zoneOwner(TEAM_NONE),
    m_zoneUpdateTimer(TIMER_TF_UPDATE_TIME),
    m_zoneLockTimer(0),
    m_towersAlliance(0),
    m_towersHorde(0)
{
    m_towerWorldState[0] = WORLD_STATE_TOWER_1_NEUTRAL;
    m_towerWorldState[1] = WORLD_STATE_TOWER_2_NEUTRAL;
    m_towerWorldState[2] = WORLD_STATE_TOWER_3_NEUTRAL;
    m_towerWorldState[3] = WORLD_STATE_TOWER_4_NEUTRAL;
    m_towerWorldState[4] = WORLD_STATE_TOWER_5_NEUTRAL;

    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
        m_towerOwner[i] = TEAM_NONE;
}

bool OutdoorPvPTF::InitOutdoorPvPArea()
{
    RegisterZone(ZONE_ID_TEROKKAR);
    RegisterZone(ZONE_ID_SHADOW_LABYRINTH);
    RegisterZone(ZONE_ID_AUCHENAI_CRYPTS);
    RegisterZone(ZONE_ID_MANA_TOMBS);
    RegisterZone(ZONE_ID_SETHEKK_HALLS);

    return true;
}

void OutdoorPvPTF::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    FillInitialWorldState(data, count, m_zoneWorldState, WORLD_STATE_ADD);
    if (m_zoneWorldState == WORLD_STATE_TF_TOWERS_CONTROLLED)
    {
        FillInitialWorldState(data, count, WORLD_STATE_TF_TOWER_COUNT_H, m_towersHorde);
        FillInitialWorldState(data, count, WORLD_STATE_TF_TOWER_COUNT_A, m_towersAlliance);
    }
    else
        UpdateTimerWorldState();

    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
        FillInitialWorldState(data, count, m_towerWorldState[i], WORLD_STATE_ADD);
}

void OutdoorPvPTF::SendRemoveWorldStates(Player* player)
{
    player->SendUpdateWorldState(m_zoneWorldState, WORLD_STATE_REMOVE);

    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
        player->SendUpdateWorldState(m_towerWorldState[i], WORLD_STATE_REMOVE);
}

void OutdoorPvPTF::UpdateWorldState(uint32 value)
{
    // update only tower count; tower states are sent in the process event
    SendUpdateWorldState(m_zoneWorldState, value);
    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
        SendUpdateWorldState(m_towerWorldState[i], value);
}

void OutdoorPvPTF::HandlePlayerEnterZone(Player* player)
{
    OutdoorPvP::HandlePlayerEnterZone(player);

    // remove the buff from the player first because there are some issues at relog
    player->RemoveAurasDueToSpell(SPELL_AUCHINDOUN_BLESSING);

    // Handle the buffs
    if (m_zoneOwner == player->GetTeam())
        player->CastSpell(player, SPELL_AUCHINDOUN_BLESSING, true);
}

void OutdoorPvPTF::HandlePlayerLeaveZone(Player* player)
{
    // remove the buff from the player
    player->RemoveAurasDueToSpell(SPELL_AUCHINDOUN_BLESSING);

    OutdoorPvP::HandlePlayerLeaveZone(player);
}

void OutdoorPvPTF::OnGameObjectCreate(GameObject* go)
{
    switch (go->GetEntry())
    {
        case GO_TEROKKAR_BANNER_1:
            m_towerBanners[0] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[0], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            break;
        case GO_TEROKKAR_BANNER_2:
            m_towerBanners[1] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[1], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            break;
        case GO_TEROKKAR_BANNER_3:
            m_towerBanners[2] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[2], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            break;
        case GO_TEROKKAR_BANNER_4:
            m_towerBanners[3] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[3], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            break;
        case GO_TEROKKAR_BANNER_5:
            m_towerBanners[4] = go->GetObjectGuid();
            go->SetGoArtKit(GetBannerArtKit(m_towerOwner[4], CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ARTKIT_HORDE, CAPTURE_ARTKIT_NEUTRAL));
            break;
    }
}

void OutdoorPvPTF::HandleObjectiveComplete(uint32 eventId, std::list<Player*> players, Team team)
{
    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
    {
        for (uint8 j = 0; j < 4; ++j)
        {
            if (TEROKKAR_TOWER_EVENTS[i][j].eventEntry == eventId)
            {
                for (std::list<Player*>::iterator itr = players.begin(); itr != players.end(); ++itr)
                {
                    if ((*itr) && (*itr)->GetTeam() == team)
                        (*itr)->AreaExploredOrEventHappens(team == ALLIANCE ? QUEST_SPIRITS_OF_AUCHINDOUM_ALLIANCE : QUEST_SPIRITS_OF_AUCHINDOUM_HORDE);
                }
                return;
            }
        }
    }
}

// process the capture events
void OutdoorPvPTF::ProcessEvent(uint32 eventId, GameObject* go)
{
    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
    {
        if (TEROKKAR_TOWERS[i] == go->GetEntry())
        {
            for (uint8 j = 0; j < 4; ++j)
            {
                if (TEROKKAR_TOWER_EVENTS[i][j].eventEntry == eventId)
                {
                    if (TEROKKAR_TOWER_EVENTS[i][j].team != m_towerOwner[i])
                    {
                        sWorld.SendDefenseMessage(ZONE_ID_TEROKKAR, TEROKKAR_TOWER_EVENTS[i][j].zoneText);

                        ProcessCaptureEvent(go, i, TEROKKAR_TOWER_EVENTS[i][j].team, TEROKKAR_TOWER_EVENTS[i][j].worldState);
                    }
                    return;
                }
            }
            return;
        }
    }
}

void OutdoorPvPTF::ProcessCaptureEvent(GameObject* go, uint32 towerId, Team team, uint32 newWorldState)
{
    if (team != TEAM_NONE)
    {
        if (team == ALLIANCE)
        {
            SetBannerVisual(go, CAPTURE_ARTKIT_ALLIANCE, CAPTURE_ANIM_ALLIANCE);
            ++m_towersAlliance;
        }
        else
        {
            SetBannerVisual(go, CAPTURE_ARTKIT_HORDE, CAPTURE_ANIM_HORDE);
            ++m_towersHorde;
        }

        // if all towers are captured then process event
        if (m_towersAlliance == MAX_TF_TOWERS || m_towersHorde == MAX_TF_TOWERS)
        {
            SendUpdateWorldState(m_zoneWorldState, WORLD_STATE_REMOVE);
            m_zoneWorldState = team == ALLIANCE ? WORLD_STATE_TF_LOCKED_ALLIANCE : WORLD_STATE_TF_LOCKED_HORDE;
            SendUpdateWorldState(m_zoneWorldState, WORLD_STATE_ADD);

            m_zoneLockTimer = TIMER_TF_LOCK_TIME;
            UpdateTimerWorldState();

            m_zoneOwner = team;
            BuffTeam(team, SPELL_AUCHINDOUN_BLESSING);

            // lock the towers
            LockTowers(go);

            sWorld.SendDefenseMessage(ZONE_ID_TEROKKAR, team == ALLIANCE ? LANG_OPVP_TF_CAPTURE_ALL_TOWERS_A : LANG_OPVP_TF_CAPTURE_ALL_TOWERS_H);
        }
    }
    else
    {
        SetBannerVisual(go, CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);

        if (m_towerOwner[towerId] == ALLIANCE)
            --m_towersAlliance;
        else
            --m_towersHorde;
    }

    // update tower state
    SendUpdateWorldState(m_towerWorldState[towerId], WORLD_STATE_REMOVE);
    m_towerWorldState[towerId] = newWorldState;
    SendUpdateWorldState(m_towerWorldState[towerId], WORLD_STATE_ADD);

    // update tower count
    SendUpdateWorldState(WORLD_STATE_TF_TOWER_COUNT_A, m_towersAlliance);
    SendUpdateWorldState(WORLD_STATE_TF_TOWER_COUNT_H, m_towersHorde);

    // update capture point owner
    m_towerOwner[towerId] = team;

}

void OutdoorPvPTF::Update(uint32 diff)
{
    if (m_zoneLockTimer)
    {
        if (m_zoneLockTimer < diff)
        {
            // remove buffs
            BuffTeam(m_zoneOwner, SPELL_AUCHINDOUN_BLESSING, true);

            // reset world states and towers
            UpdateWorldState(WORLD_STATE_REMOVE);
            m_zoneOwner = TEAM_NONE;
            m_zoneWorldState = WORLD_STATE_TF_TOWERS_CONTROLLED;
            m_towersAlliance = 0;
            m_towersHorde = 0;
            m_towerWorldState[0] = WORLD_STATE_TOWER_1_NEUTRAL;
            m_towerWorldState[1] = WORLD_STATE_TOWER_2_NEUTRAL;
            m_towerWorldState[2] = WORLD_STATE_TOWER_3_NEUTRAL;
            m_towerWorldState[3] = WORLD_STATE_TOWER_4_NEUTRAL;
            m_towerWorldState[4] = WORLD_STATE_TOWER_5_NEUTRAL;
            UpdateWorldState(WORLD_STATE_ADD);

            // update tower count
            SendUpdateWorldState(WORLD_STATE_TF_TOWER_COUNT_A, m_towersAlliance);
            SendUpdateWorldState(WORLD_STATE_TF_TOWER_COUNT_H, m_towersHorde);

            for (PlayerSet::iterator itr = m_zonePlayers.begin(); itr != m_zonePlayers.end(); ++itr)
            {
                if ((*itr) && (*itr)->isAlive())
                {
                    ResetTowers((*itr));
                    break;
                }
            }

            m_zoneLockTimer = 0;
        }
        else
        {
            if (m_zoneUpdateTimer < diff)
            {
                // update timer
                UpdateTimerWorldState();

                m_zoneUpdateTimer = TIMER_TF_UPDATE_TIME;
            }
            else
                m_zoneUpdateTimer -= diff;

            m_zoneLockTimer -= diff;
        }
    }
}

void OutdoorPvPTF::UpdateTimerWorldState()
{
    // Calculate time
    uint32 minutesLeft = m_zoneLockTimer / 60000;
    uint32 hoursLeft = minutesLeft / 60;
    minutesLeft -= hoursLeft * 60;
    uint32 firstDigit = minutesLeft / 10;

    SendUpdateWorldState(WORLD_STATE_TF_TIME_MIN_FIRST_DIGIT, firstDigit);
    SendUpdateWorldState(WORLD_STATE_TF_TIME_MIN_SECOND_DIGIT, minutesLeft - firstDigit * 10);
    SendUpdateWorldState(WORLD_STATE_TF_TIME_HOURS, hoursLeft);
}

void OutdoorPvPTF::LockTowers(const WorldObject* objRef)
{
    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
    {
        if (GameObject* go = objRef->GetMap()->GetGameObject(m_towerBanners[i]))
            go->SetLootState(GO_JUST_DEACTIVATED);

        sOutdoorPvPMgr.SetCapturePointSlider(TEROKKAR_TOWERS[i], m_zoneOwner == ALLIANCE ? CAPTURE_SLIDER_ALLIANCE_LOCKED : CAPTURE_SLIDER_HORDE_LOCKED);
    }
}

void OutdoorPvPTF::ResetTowers(const WorldObject* objRef)
{
    for (uint8 i = 0; i < MAX_TF_TOWERS; ++i)
    {
        if (GameObject* go = objRef->GetMap()->GetGameObject(m_towerBanners[i]))
        {
            go->SetCapturePointSlider(CAPTURE_SLIDER_NEUTRAL);
            SetBannerVisual(go, CAPTURE_ARTKIT_NEUTRAL, CAPTURE_ANIM_NEUTRAL);
        }
        else
            // if grid is unloaded, resetting the slider value is enough
            sOutdoorPvPMgr.SetCapturePointSlider(TEROKKAR_TOWERS[i], CAPTURE_SLIDER_NEUTRAL);
    }
}
