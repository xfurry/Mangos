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
#include "WorldPvPNA.h"


WorldPvPNA::WorldPvPNA() : WorldPvP(),
    m_uiControllerMapState(WORLD_STATE_NA_HALAA_NEUTRAL),
    m_uiControllerWorldState(0),
    m_uiZoneController(NEUTRAL),
    m_bCanCaptureHalaa(true),
    m_uiGuardsLeft(0)
{
    m_uiTypeId = WORLD_PVP_TYPE_NA;
}

bool WorldPvPNA::InitWorldPvPArea()
{
    RegisterZone(ZONE_ID_NAGRAND);

    return true;
}

void WorldPvPNA::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    if (m_uiZoneController != NEUTRAL)
    {
        FillInitialWorldState(data, count, m_uiControllerWorldState,    1);
        FillInitialWorldState(data, count, WORLD_STATE_NA_GUARDS_LEFT,  m_uiGuardsLeft);
        FillInitialWorldState(data, count, WORLD_STATE_NA_GUARDS_MAX,   MAX_NA_GUARDS);

        // map states
        FillInitialWorldState(data, count, m_uiWyvernSouthWorldState,   1);
        FillInitialWorldState(data, count, m_uiWyvernNorthWorldState,   1);
        FillInitialWorldState(data, count, m_uiWyvernEastWorldState,    1);
        FillInitialWorldState(data, count, m_uiWyvernWestWorldState,    1);
    }

    FillInitialWorldState(data, count, m_uiControllerMapState,      1);
}

void WorldPvPNA::SendRemoveWorldStates(Player* pPlayer)
{
    pPlayer->SendUpdateWorldState(m_uiControllerWorldState,     0);
    pPlayer->SendUpdateWorldState(m_uiControllerMapState,       0);

    pPlayer->SendUpdateWorldState(m_uiWyvernSouthWorldState,    0);
    pPlayer->SendUpdateWorldState(m_uiWyvernNorthWorldState,    0);
    pPlayer->SendUpdateWorldState(m_uiWyvernEastWorldState,     0);
    pPlayer->SendUpdateWorldState(m_uiWyvernWestWorldState,     0);
}

void WorldPvPNA::HandlePlayerEnterZone(Player* pPlayer)
{
    WorldPvP::HandlePlayerEnterZone(pPlayer);

    // remove the buff from the player first because there are some issues at relog
    pPlayer->RemoveAurasDueToSpell(SPELL_STRENGTH_HALAANI);

    // Handle the buffs
    if (m_uiZoneController == NEUTRAL)
        return;
    else if (pPlayer->GetTeam() == m_uiZoneController)
        pPlayer->CastSpell(pPlayer, SPELL_STRENGTH_HALAANI, true);
}

void WorldPvPNA::HandlePlayerLeaveZone(Player* pPlayer)
{
    // remove the buff from the player
    pPlayer->RemoveAurasDueToSpell(SPELL_STRENGTH_HALAANI);

    WorldPvP::HandlePlayerLeaveZone(pPlayer);
}

void WorldPvPNA::HandleObjectiveComplete(PlayerSet m_sPlayersSet, uint32 uiEventId)
{
    if (uiEventId == EVENT_HALAA_BANNER_WIN_ALLIANCE || uiEventId == EVENT_HALAA_BANNER_WIN_HORDE)
    {
        for (PlayerSet::iterator itr = m_sPlayersSet.begin(); itr != m_sPlayersSet.end(); ++itr)
        {
            if (!(*itr))
                continue;

            (*itr)->KilledMonsterCredit(NPC_HALAA_COMBATANT);
        }
    }
}

void WorldPvPNA::OnCreatureCreate(Creature* pCreature)
{
    switch (pCreature->GetEntry())
    {
        case NPC_ALLIANCE_HANAANI_GUARD:
        case NPC_RESEARCHER_KARTOS:
        case NPC_QUARTERMASTER_DAVIAN:
        case NPC_MERCHANT_ALDRAAN:
        case NPC_VENDOR_CENDRII:
        case NPC_AMMUNITIONER_BANRO:
            lAllianceSoldiers.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_HORDE_HALAANI_GUARD:
        case NPC_RESEARCHER_AMERELDINE:
        case NPC_QUARTERMASTER_NORELIQE:
        case NPC_MERCHANT_COREIEL:
        case NPC_VENDOR_EMBELAR:
        case NPC_AMMUNITIONER_TASALDAN:
            lHordeSoldiers.push_back(pCreature->GetObjectGuid());
            break;

        default:
            return;
    }

    // Despawn creatures on create - will be spawned later in script
    pCreature->SetRespawnDelay(7*DAY);
    pCreature->ForcedDespawn();
}

void WorldPvPNA::OnCreatureDeath(Creature* pCreature)
{
    if (pCreature->GetEntry() != NPC_HORDE_HALAANI_GUARD && pCreature->GetEntry() != NPC_ALLIANCE_HANAANI_GUARD)
        return;

    --m_uiGuardsLeft;
    SendUpdateWorldState(WORLD_STATE_NA_GUARDS_LEFT,  m_uiGuardsLeft);

    if (m_uiGuardsLeft == 0)
    {
        // make capturable
        m_bCanCaptureHalaa = true;
        SendUpdateWorldState(m_uiControllerMapState, 0);
        m_uiControllerMapState = m_uiZoneController == ALLIANCE ? WORLD_STATE_NA_HALAA_NEU_A : WORLD_STATE_NA_HALAA_NEU_H;
        SendUpdateWorldState(m_uiControllerMapState, 1);
    }
}

void WorldPvPNA::OnCreatureRespawn(Creature* pCreature)
{
    if (pCreature->GetEntry() != NPC_HORDE_HALAANI_GUARD && pCreature->GetEntry() != NPC_ALLIANCE_HANAANI_GUARD)
        return;

    ++m_uiGuardsLeft;
    SendUpdateWorldState(WORLD_STATE_NA_GUARDS_LEFT,  m_uiGuardsLeft);

    if (m_uiGuardsLeft > 0)
        m_bCanCaptureHalaa = false;

    if (m_uiGuardsLeft == MAX_NA_GUARDS)
    {
        SendUpdateWorldState(m_uiControllerMapState, 0);
        m_uiControllerMapState = m_uiZoneController == ALLIANCE ? WORLD_STATE_NA_HALAA_ALLIANCE : WORLD_STATE_NA_HALAA_HORDE;
        SendUpdateWorldState(m_uiControllerMapState, 1);
    }
}

void WorldPvPNA::OnGameObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_HALAA_BANNER:
            m_HalaaBanerGuid = pGo->GetObjectGuid();
            pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
            break;

        case GO_WYVERN_ROOST_ALY_SOUTH:
            m_AllianceRooster[0] = pGo->GetObjectGuid();
            break;
        case GO_WYVERN_ROOST_ALY_NORTH:
            m_AllianceRooster[1] = pGo->GetObjectGuid();
            break;
        case GO_WYVERN_ROOST_ALY_EAST:
            m_AllianceRooster[2] = pGo->GetObjectGuid();
            break;
        case GO_WYVERN_ROOST_ALY_WEST:
            m_AllianceRooster[3] = pGo->GetObjectGuid();
            break;

        case GO_BOMB_WAGON_HORDE_SOUTH:
            m_HordeWagons[0] = pGo->GetObjectGuid();
            break;
        case GO_BOMB_WAGON_HORDE_NORTH:
            m_HordeWagons[1] = pGo->GetObjectGuid();
            break;
        case GO_BOMB_WAGON_HORDE_EAST:
            m_HordeWagons[2] = pGo->GetObjectGuid();
            break;
        case GO_BOMB_WAGON_HORDE_WEST:
            m_HordeWagons[3] = pGo->GetObjectGuid();
            break;

        case GO_DESTROYED_ROOST_ALY_SOUTH:
            m_AllianceBrokenRooster[0] = pGo->GetObjectGuid();
            break;
        case GO_DESTROYED_ROOST_ALY_NORTH:
            m_AllianceBrokenRooster[1] = pGo->GetObjectGuid();
            break;
        case GO_DESTROYED_ROOST_ALY_EAST:
            m_AllianceBrokenRooster[2] = pGo->GetObjectGuid();
            break;
        case GO_DESTROYED_ROOST_ALY_WEST:
            m_AllianceBrokenRooster[3] = pGo->GetObjectGuid();
            break;

        case GO_WYVERN_ROOST_HORDE_SOUTH:
            m_HordeRooster[0] = pGo->GetObjectGuid();
            break;
        case GO_WYVERN_ROOST_HORDE_NORTH:
            m_HordeRooster[1] = pGo->GetObjectGuid();
            break;
        case GO_WYVERN_ROOST_HORDE_EAST:
            m_HordeRooster[2] = pGo->GetObjectGuid();
            break;
        case GO_WYVERN_ROOST_HORDE_WEST:
            m_HordeRooster[3] = pGo->GetObjectGuid();
            break;

        case GO_BOMB_WAGON_ALY_SOUTH:
            m_AllianceWagons[0] = pGo->GetObjectGuid();
            break;
        case GO_BOMB_WAGON_ALY_NORTH:
            m_AllianceWagons[1] = pGo->GetObjectGuid();
            break;
        case GO_BOMB_WAGON_ALY_EAST:
            m_AllianceWagons[2] = pGo->GetObjectGuid();
            break;
        case GO_BOMB_WAGON_ALY_WEST:
            m_AllianceWagons[3] = pGo->GetObjectGuid();
            break;

        case GO_DESTROYED_ROOST_HORDE_SOUTH:
            m_HordeBrokenRooster[0] = pGo->GetObjectGuid();
            break;
        case GO_DESTROYED_ROOST_HORDE_NORTH:
            m_HordeBrokenRooster[1] = pGo->GetObjectGuid();
            break;
        case GO_DESTROYED_ROOST_HORDE_EAST:
            m_HordeBrokenRooster[2] = pGo->GetObjectGuid();
            break;
        case GO_DESTROYED_ROOST_HORDE_WEST:
            m_HordeBrokenRooster[3] = pGo->GetObjectGuid();
            break;
    }
}

void WorldPvPNA::UpdateWorldState(uint8 uiValue)
{
    SendUpdateWorldState(m_uiControllerWorldState,  uiValue);
    SendUpdateWorldState(m_uiControllerMapState,    uiValue);

    // Update guards only for positive states
    if (uiValue)
    {
        SendUpdateWorldState(WORLD_STATE_NA_GUARDS_MAX,   MAX_NA_GUARDS);
        SendUpdateWorldState(WORLD_STATE_NA_GUARDS_LEFT,  m_uiGuardsLeft);
    }

    UpdateWyvernsWorldState(uiValue);
}

void WorldPvPNA::UpdateWyvernsWorldState(uint8 uiValue)
{
    SendUpdateWorldState(m_uiWyvernSouthWorldState,   uiValue);
    SendUpdateWorldState(m_uiWyvernNorthWorldState,   uiValue);
    SendUpdateWorldState(m_uiWyvernEastWorldState,    uiValue);
    SendUpdateWorldState(m_uiWyvernWestWorldState,    uiValue);
}

// process the capture events
void WorldPvPNA::ProcessEvent(GameObject* pGo, Player* pPlayer, uint32 uiEventId)
{
    // don't process when Halaa is not available
    if (!m_bCanCaptureHalaa)
        return;

    // If we are not using the Halaa banner return
    if (pGo->GetEntry() != GO_HALAA_BANNER)
        return;

    switch (uiEventId)
    {
        case EVENT_HALAA_BANNER_WIN_ALLIANCE:
        case EVENT_HALAA_BANNER_WIN_HORDE:
            ProcessCaptureEvent(WIN, pPlayer->GetTeam());
            break;
    }
}

void WorldPvPNA::ProcessCaptureEvent(uint32 uiCaptureType, uint32 uiTeam)
{
    if (uiCaptureType == WIN)
    {
        if (m_uiZoneController != NEUTRAL)
        {
            DoProcessTeamBuff((Team)m_uiZoneController, SPELL_STRENGTH_HALAANI, true);
            sWorld.SendZoneText(ZONE_ID_NAGRAND, sObjectMgr.GetMangosStringForDBCLocale(uiTeam == ALLIANCE ? LANG_OPVP_NA_LOOSE_H: LANG_OPVP_NA_LOOSE_A));
        }

        UpdateWorldState(0);
        m_uiZoneController = uiTeam;
        m_bCanCaptureHalaa = false;
        DoRespawnSoldiers(uiTeam);
        m_uiControllerWorldState = uiTeam == ALLIANCE ? WORLD_STATE_NA_GUARDS_ALLIANCE : WORLD_STATE_NA_GUARDS_HORDE;
        m_uiControllerMapState = uiTeam == ALLIANCE ? WORLD_STATE_NA_HALAA_ALLIANCE : WORLD_STATE_NA_HALAA_HORDE;

        SetBannerArtKit(uiTeam == ALLIANCE ? GO_ARTKIT_BANNER_ALLIANCE : GO_ARTKIT_BANNER_HORDE);
        DoHandleFactionObjects(uiTeam);
        UpdateWorldState(1);

        DoProcessTeamBuff((Team)m_uiZoneController, SPELL_STRENGTH_HALAANI);
        sWorld.SendZoneText(ZONE_ID_NAGRAND, sObjectMgr.GetMangosStringForDBCLocale(uiTeam == ALLIANCE ? LANG_OPVP_NA_CAPTURE_A: LANG_OPVP_NA_CAPTURE_H));
    }
}

void WorldPvPNA::DoHandleFactionObjects(uint32 uiFaction)
{
    if (uiFaction == ALLIANCE)
    {
        for (uint8 i = 0; i < MAX_NA_ROOSTS; ++i)
        {
            DoRespawnObjects(m_HordeWagons[i], false);
            DoRespawnObjects(m_AllianceBrokenRooster[i], false);
            DoRespawnObjects(m_AllianceRooster[i], false);

            DoRespawnObjects(m_AllianceWagons[i], true);
            DoRespawnObjects(m_HordeBrokenRooster[i], true);
        }

        m_uiWyvernSouthWorldState = WORLD_STATE_NA_WYVERN_SOUTH_NEU_H;
        m_uiWyvernNorthWorldState = WORLD_STATE_NA_WYVERN_NORTH_NEU_H;
        m_uiWyvernEastWorldState = WORLD_STATE_NA_WYVERN_EAST_NEU_H;
        m_uiWyvernWestWorldState = WORLD_STATE_NA_WYVERN_WEST_NEU_H;
    }
    else if (uiFaction == HORDE)
    {
        for (uint8 i = 0; i < MAX_NA_ROOSTS; ++i)
        {
            DoRespawnObjects(m_AllianceWagons[i], false);
            DoRespawnObjects(m_HordeBrokenRooster[i], false);
            DoRespawnObjects(m_HordeRooster[i], false);

            DoRespawnObjects(m_HordeWagons[i], true);
            DoRespawnObjects(m_AllianceBrokenRooster[i], true);
        }

        m_uiWyvernSouthWorldState = WORLD_STATE_NA_WYVERN_SOUTH_NEU_A;
        m_uiWyvernNorthWorldState = WORLD_STATE_NA_WYVERN_NORTH_NEU_A;
        m_uiWyvernEastWorldState = WORLD_STATE_NA_WYVERN_EAST_NEU_A;
        m_uiWyvernWestWorldState = WORLD_STATE_NA_WYVERN_WEST_NEU_A;
    }
}

void WorldPvPNA::DoRespawnSoldiers(uint32 uiFaction)
{
    // neet to use a player as anchor for the map
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    if (uiFaction == ALLIANCE)
    {
        // despawn all horde vendors
        for (std::list<ObjectGuid>::const_iterator itr = lHordeSoldiers.begin(); itr != lHordeSoldiers.end(); ++itr)
        {
            if (Creature* pSoldier = pPlayer->GetMap()->GetCreature(*itr))
            {
                // reset respawn time
                pSoldier->SetRespawnDelay(7*DAY);
                pSoldier->ForcedDespawn();
            }
        }

        // spawn all alliance soldiers and vendors
        for (std::list<ObjectGuid>::const_iterator itr = lAllianceSoldiers.begin(); itr != lAllianceSoldiers.end(); ++itr)
        {
            if (Creature* pSoldier = pPlayer->GetMap()->GetCreature(*itr))
            {
                // lower respawn time
                pSoldier->SetRespawnDelay(HOUR);
                pSoldier->Respawn();
            }
        }
    }
    else if (uiFaction == HORDE)
    {
        // despawn all alliance vendors
        for (std::list<ObjectGuid>::const_iterator itr = lAllianceSoldiers.begin(); itr != lAllianceSoldiers.end(); ++itr)
        {
            if (Creature* pSoldier = pPlayer->GetMap()->GetCreature(*itr))
            {
                // reset respawn time
                pSoldier->SetRespawnDelay(7*DAY);
                pSoldier->ForcedDespawn();
            }
        }

        // spawn all horde soldiers and vendors
        for (std::list<ObjectGuid>::const_iterator itr = lHordeSoldiers.begin(); itr != lHordeSoldiers.end(); ++itr)
        {
            if (Creature* pSoldier = pPlayer->GetMap()->GetCreature(*itr))
            {
                // lower respawn time
                pSoldier->SetRespawnDelay(HOUR);
                pSoldier->Respawn();
            }
        }
    }
}

bool WorldPvPNA::HandleObjectUse(Player* pPlayer, GameObject* pGo)
{
    // ToDo:
    return false;
}

void WorldPvPNA::SetBannerArtKit(uint32 uiArtkit)
{
    // neet to use a player as anchor for the map
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    if (GameObject* pBanner = pPlayer->GetMap()->GetGameObject(m_HalaaBanerGuid))
    {
        pBanner->SetGoArtKit(uiArtkit);
        pBanner->Refresh();
    }
}

void WorldPvPNA::DoRespawnObjects(ObjectGuid GameObjectGuid, bool bRespawn)
{
    // neet to use a player as anchor for the map
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;

    if (GameObject* pBanner = pPlayer->GetMap()->GetGameObject(GameObjectGuid))
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
