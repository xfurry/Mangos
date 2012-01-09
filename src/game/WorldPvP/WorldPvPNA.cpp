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
            lAllianceSoldiers.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_RESEARCHER_KARTOS:
        case NPC_QUARTERMASTER_DAVIAN:
        case NPC_MERCHANT_ALDRAAN:
        case NPC_VENDOR_CENDRII:
        case NPC_AMMUNITIONER_BANRO:
            lAllianceVendors.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_HORDE_HALAANI_GUARD:
            lHordeSoldiers.push_back(pCreature->GetObjectGuid());
            break;
        case NPC_RESEARCHER_AMERELDINE:
        case NPC_QUARTERMASTER_NORELIQE:
        case NPC_MERCHANT_COREIEL:
        case NPC_VENDOR_EMBELAR:
        case NPC_AMMUNITIONER_TASALDAN:
            lHordeVendors.push_back(pCreature->GetObjectGuid());
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
    // ToDo: handle guards death
}

void WorldPvPNA::OnGameObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_HALAA_BANNER:
            m_HalaaBanerGuid = pGo->GetObjectGuid();
            pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
            break;
    }
}

// process the capture events
void WorldPvPNA::ProcessEvent(GameObject* pGo, Player* pPlayer, uint32 uiEventId)
{
    // If we are not using the lighthouse return
    if (pGo->GetEntry() != GO_HALAA_BANNER)
        return;

    switch (uiEventId)
    {
        case EVENT_HALAA_BANNER_PROGRESS_ALLIANCE:
        case EVENT_HALAA_BANNER_PROGRESS_HORDE:
            ProcessCaptureEvent(PROGRESS, pPlayer->GetTeam());
            break;
        case EVENT_HALAA_BANNER_WIN_ALLIANCE:
        case EVENT_HALAA_BANNER_WIN_HORDE:
            ProcessCaptureEvent(WIN, pPlayer->GetTeam());
            break;
    }
}

void WorldPvPNA::ProcessCaptureEvent(uint32 uiCaptureType, uint32 uiTeam)
{
    switch (uiCaptureType)
    {
        case WIN:
            // Spawn the npcs only when the tower is fully controlled
            DoRespawnSoldiers(uiTeam);
            // ToDo: spawn gameobjects
            break;
        case PROGRESS:
            SetBannerArtKit(uiTeam == ALLIANCE ? GO_ARTKIT_BANNER_ALLIANCE : GO_ARTKIT_BANNER_HORDE);
            break;
    }
}

void WorldPvPNA::DoRespawnSoldiers(uint32 uiFaction)
{
    // neet to use a player as anchor for the map
    Player* pPlayer = GetPlayerInZone();
    if (!pPlayer)
        return;
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
