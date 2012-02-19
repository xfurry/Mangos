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
    m_uiZoneMapState(WORLD_STATE_NA_HALAA_NEUTRAL),
    m_uiZoneWorldState(0),
    m_uiZoneOwner(TEAM_NONE),
    m_uiGuardsLeft(0)
{
}

bool WorldPvPNA::InitWorldPvPArea()
{
    RegisterZone(ZONE_ID_NAGRAND);

    return true;
}

void WorldPvPNA::FillInitialWorldStates(WorldPacket& data, uint32& count)
{
    if (m_uiZoneOwner != TEAM_NONE)
    {
        FillInitialWorldState(data, count, m_uiZoneWorldState, WORLD_STATE_ADD);
        FillInitialWorldState(data, count, WORLD_STATE_NA_GUARDS_MAX, MAX_NA_GUARDS);
        FillInitialWorldState(data, count, WORLD_STATE_NA_GUARDS_LEFT, m_uiGuardsLeft);

        // map states
        for (uint8 i = 0; i < MAX_NA_ROOSTS; ++i)
            FillInitialWorldState(data, count, m_uiRoostWorldState[i], WORLD_STATE_ADD);
    }

    FillInitialWorldState(data, count, m_uiZoneMapState, WORLD_STATE_ADD);
}

void WorldPvPNA::SendRemoveWorldStates(Player* pPlayer)
{
    pPlayer->SendUpdateWorldState(m_uiZoneWorldState, WORLD_STATE_REMOVE);
    pPlayer->SendUpdateWorldState(m_uiZoneMapState, WORLD_STATE_REMOVE);

    for (uint8 i = 0; i < MAX_NA_ROOSTS; ++i)
        pPlayer->SendUpdateWorldState(m_uiRoostWorldState[i], WORLD_STATE_REMOVE);
}

void WorldPvPNA::HandlePlayerEnterZone(Player* pPlayer)
{
    WorldPvP::HandlePlayerEnterZone(pPlayer);

    // remove the buff from the player first because there are some issues at relog
    pPlayer->RemoveAurasDueToSpell(SPELL_STRENGTH_HALAANI);

    // buff the player if same faction is controlling the zone
    if (pPlayer->GetTeam() == m_uiZoneOwner)
        pPlayer->CastSpell(pPlayer, SPELL_STRENGTH_HALAANI, true);
}

void WorldPvPNA::HandlePlayerLeaveZone(Player* pPlayer)
{
    // remove the buff from the player
    pPlayer->RemoveAurasDueToSpell(SPELL_STRENGTH_HALAANI);

    WorldPvP::HandlePlayerLeaveZone(pPlayer);
}

void WorldPvPNA::HandleObjectiveComplete(uint32 uiEventId, std::list<Player*> players, Team faction)
{
    if (uiEventId == EVENT_HALAA_BANNER_WIN_ALLIANCE || uiEventId == EVENT_HALAA_BANNER_WIN_HORDE)
    {
        for (std::list<Player*>::iterator itr = players.begin(); itr != players.end(); ++itr)
        {
            if ((*itr) && (*itr)->GetTeam() == faction)
                (*itr)->KilledMonsterCredit(NPC_HALAA_COMBATANT);
        }
    }
}

// Cast player spell on opponent kill
void WorldPvPNA::HandlePlayerKillInsideArea(Player* pPlayer, Unit* pVictim)
{
    if (GameObject* capturePoint = pPlayer->GetMap()->GetGameObject(m_HalaaBannerGuid))
    {
        // check capture point range
        GameObjectInfo const* info = capturePoint->GetGOInfo();
        if (info && pPlayer->IsWithinDistInMap(capturePoint, info->capturePoint.radius))
        {
            // check capture point faction
            if (pPlayer->GetTeam() == m_uiZoneOwner)
                pPlayer->CastSpell(pPlayer, pPlayer->GetTeam() == ALLIANCE ? SPELL_NAGRAND_TOKEN_ALLIANCE : SPELL_NAGRAND_TOKEN_HORDE, true);

            return;
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
            if (m_uiZoneOwner == ALLIANCE)
                return;
            break;
        case NPC_HORDE_HALAANI_GUARD:
        case NPC_RESEARCHER_AMERELDINE:
        case NPC_QUARTERMASTER_NORELIQE:
        case NPC_MERCHANT_COREIEL:
        case NPC_VENDOR_EMBELAR:
        case NPC_AMMUNITIONER_TASALDAN:
            lHordeSoldiers.push_back(pCreature->GetObjectGuid());
            if (m_uiZoneOwner == HORDE)
                return;
            break;

        default:
            return;
    }

    // Despawn creatures on create - will be spawned later in script
    pCreature->SetRespawnDelay(7 * DAY);
    pCreature->ForcedDespawn();
}

void WorldPvPNA::OnCreatureDeath(Creature* pCreature)
{
    if (pCreature->GetEntry() != NPC_HORDE_HALAANI_GUARD && pCreature->GetEntry() != NPC_ALLIANCE_HANAANI_GUARD)
        return;

    --m_uiGuardsLeft;
    SendUpdateWorldState(WORLD_STATE_NA_GUARDS_LEFT, m_uiGuardsLeft);

    if (m_uiGuardsLeft == 0)
    {
        // make capturable
        UnlockHalaa(pCreature, m_uiZoneOwner);

        // update world state
        SendUpdateWorldState(m_uiZoneMapState, WORLD_STATE_REMOVE);
        m_uiZoneMapState = m_uiZoneOwner == ALLIANCE ? WORLD_STATE_NA_HALAA_NEUTRAL_A : WORLD_STATE_NA_HALAA_NEUTRAL_H;
        SendUpdateWorldState(m_uiZoneMapState, WORLD_STATE_ADD);
    }
}

void WorldPvPNA::OnCreatureRespawn(Creature* pCreature)
{
    if (pCreature->GetEntry() != NPC_HORDE_HALAANI_GUARD && pCreature->GetEntry() != NPC_ALLIANCE_HANAANI_GUARD)
        return;

    if (m_uiGuardsLeft == 0)
    {
        LockHalaa(pCreature, m_uiZoneOwner);

        // update world state
        SendUpdateWorldState(m_uiZoneMapState, WORLD_STATE_REMOVE);
        m_uiZoneMapState = m_uiZoneOwner == ALLIANCE ? WORLD_STATE_NA_HALAA_ALLIANCE : WORLD_STATE_NA_HALAA_HORDE;
        SendUpdateWorldState(m_uiZoneMapState, WORLD_STATE_ADD);
    }

    ++m_uiGuardsLeft;
    SendUpdateWorldState(WORLD_STATE_NA_GUARDS_LEFT, m_uiGuardsLeft);
}

void WorldPvPNA::OnGameObjectCreate(GameObject* pGo)
{
    switch (pGo->GetEntry())
    {
        case GO_HALAA_BANNER:
            m_HalaaBannerGuid = pGo->GetObjectGuid();
            pGo->SetGoArtKit(GO_ARTKIT_BANNER_NEUTRAL);
            break;

        case GO_WYVERN_ROOST_ALLIANCE_SOUTH:
            m_AllianceRoost[0] = pGo->GetObjectGuid();
            break;
        case GO_WYVERN_ROOST_ALLIANCE_NORTH:
            m_AllianceRoost[1] = pGo->GetObjectGuid();
            break;
        case GO_WYVERN_ROOST_ALLIANCE_EAST:
            m_AllianceRoost[2] = pGo->GetObjectGuid();
            break;
        case GO_WYVERN_ROOST_ALLIANCE_WEST:
            m_AllianceRoost[3] = pGo->GetObjectGuid();
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

        case GO_DESTROYED_ROOST_ALLIANCE_SOUTH:
            m_AllianceBrokenRoost[0] = pGo->GetObjectGuid();
            break;
        case GO_DESTROYED_ROOST_ALLIANCE_NORTH:
            m_AllianceBrokenRoost[1] = pGo->GetObjectGuid();
            break;
        case GO_DESTROYED_ROOST_ALLIANCE_EAST:
            m_AllianceBrokenRoost[2] = pGo->GetObjectGuid();
            break;
        case GO_DESTROYED_ROOST_ALLIANCE_WEST:
            m_AllianceBrokenRoost[3] = pGo->GetObjectGuid();
            break;

        case GO_WYVERN_ROOST_HORDE_SOUTH:
            m_HordeRoost[0] = pGo->GetObjectGuid();
            break;
        case GO_WYVERN_ROOST_HORDE_NORTH:
            m_HordeRoost[1] = pGo->GetObjectGuid();
            break;
        case GO_WYVERN_ROOST_HORDE_EAST:
            m_HordeRoost[2] = pGo->GetObjectGuid();
            break;
        case GO_WYVERN_ROOST_HORDE_WEST:
            m_HordeRoost[3] = pGo->GetObjectGuid();
            break;

        case GO_BOMB_WAGON_ALLIANCE_SOUTH:
            m_AllianceWagons[0] = pGo->GetObjectGuid();
            break;
        case GO_BOMB_WAGON_ALLIANCE_NORTH:
            m_AllianceWagons[1] = pGo->GetObjectGuid();
            break;
        case GO_BOMB_WAGON_ALLIANCE_EAST:
            m_AllianceWagons[2] = pGo->GetObjectGuid();
            break;
        case GO_BOMB_WAGON_ALLIANCE_WEST:
            m_AllianceWagons[3] = pGo->GetObjectGuid();
            break;

        case GO_DESTROYED_ROOST_HORDE_SOUTH:
            m_HordeBrokenRoost[0] = pGo->GetObjectGuid();
            break;
        case GO_DESTROYED_ROOST_HORDE_NORTH:
            m_HordeBrokenRoost[1] = pGo->GetObjectGuid();
            break;
        case GO_DESTROYED_ROOST_HORDE_EAST:
            m_HordeBrokenRoost[2] = pGo->GetObjectGuid();
            break;
        case GO_DESTROYED_ROOST_HORDE_WEST:
            m_HordeBrokenRoost[3] = pGo->GetObjectGuid();
            break;
    }
}

void WorldPvPNA::UpdateWorldState(uint8 uiValue)
{
    SendUpdateWorldState(m_uiZoneWorldState, uiValue);
    SendUpdateWorldState(m_uiZoneMapState, uiValue);

    // Update guards only for positive states
    if (uiValue)
    {
        SendUpdateWorldState(WORLD_STATE_NA_GUARDS_MAX, MAX_NA_GUARDS);
        SendUpdateWorldState(WORLD_STATE_NA_GUARDS_LEFT, m_uiGuardsLeft);
    }

    UpdateWyvernsWorldState(uiValue);
}

void WorldPvPNA::UpdateWyvernsWorldState(uint8 uiValue)
{
    for (uint8 i = 0; i < MAX_NA_ROOSTS; ++i)
        SendUpdateWorldState(m_uiRoostWorldState[i], uiValue);
}

// process the capture events
void WorldPvPNA::ProcessEvent(uint32 uiEventId, GameObject* pGo)
{
    // If we are not using the Halaa banner return
    if (pGo->GetEntry() != GO_HALAA_BANNER)
        return;

    switch (uiEventId)
    {
        case EVENT_HALAA_BANNER_WIN_ALLIANCE:
            ProcessCaptureEvent(pGo, ALLIANCE);
            break;
        case EVENT_HALAA_BANNER_WIN_HORDE:
            ProcessCaptureEvent(pGo, HORDE);
            break;
    }
}

void WorldPvPNA::ProcessCaptureEvent(GameObject* pGo, Team faction)
{
    if (m_uiZoneOwner != TEAM_NONE)
    {
        SetGraveyard(m_uiZoneOwner, true);
        DoProcessTeamBuff(m_uiZoneOwner, SPELL_STRENGTH_HALAANI, true);
        sWorld.SendZoneText(ZONE_ID_NAGRAND, sObjectMgr.GetMangosStringForDBCLocale(m_uiZoneOwner == ALLIANCE ? LANG_OPVP_NA_LOOSE_A: LANG_OPVP_NA_LOOSE_H));
    }

    UpdateWorldState(WORLD_STATE_REMOVE);
    RespawnSoldiers(pGo, faction);
    SetGraveyard(faction);

    if (faction == ALLIANCE)
    {
        m_uiZoneWorldState = WORLD_STATE_NA_GUARDS_ALLIANCE;
        m_uiZoneMapState = WORLD_STATE_NA_HALAA_ALLIANCE;

        SetCapturePointVisual(pGo, GO_ARTKIT_BANNER_ALLIANCE, CAPTURE_ANIM_ALLIANCE);
    }
    else
    {
        m_uiZoneWorldState = WORLD_STATE_NA_GUARDS_HORDE;
        m_uiZoneMapState = WORLD_STATE_NA_HALAA_HORDE;

        SetCapturePointVisual(pGo, GO_ARTKIT_BANNER_HORDE, CAPTURE_ANIM_HORDE);
    }

    HandleFactionObjects(pGo, faction);
    m_uiZoneOwner = faction;
    UpdateWorldState(WORLD_STATE_ADD);

    DoProcessTeamBuff(m_uiZoneOwner, SPELL_STRENGTH_HALAANI);
    sWorld.SendZoneText(ZONE_ID_NAGRAND, sObjectMgr.GetMangosStringForDBCLocale(faction == ALLIANCE ? LANG_OPVP_NA_CAPTURE_A: LANG_OPVP_NA_CAPTURE_H));
}

void WorldPvPNA::SetGraveyard(Team faction, bool bRemove)
{
    if (bRemove)
        sObjectMgr.RemoveGraveYardLink(GRAVEYARD_ID_HALAA, GRAVEYARD_ZONE_ID_HALAA, faction, false);
    else
        sObjectMgr.AddGraveYardLink(GRAVEYARD_ID_HALAA, GRAVEYARD_ZONE_ID_HALAA, faction, false);
}

void WorldPvPNA::HandleFactionObjects(const WorldObject* objRef, Team faction)
{
    if (faction == ALLIANCE)
    {
        for (uint8 i = 0; i < MAX_NA_ROOSTS; ++i)
        {
            RespawnObjects(objRef, m_HordeWagons[i], false);
            RespawnObjects(objRef, m_AllianceBrokenRoost[i], false);
            RespawnObjects(objRef, m_AllianceRoost[i], false);

            RespawnObjects(objRef, m_AllianceWagons[i], true);
            RespawnObjects(objRef, m_HordeBrokenRoost[i], true);

            m_uiRoostWorldState[i] = aHordeNeutralStates[i];
        }
    }
    else if (faction == HORDE)
    {
        for (uint8 i = 0; i < MAX_NA_ROOSTS; ++i)
        {
            RespawnObjects(objRef, m_AllianceWagons[i], false);
            RespawnObjects(objRef, m_HordeBrokenRoost[i], false);
            RespawnObjects(objRef, m_HordeRoost[i], false);

            RespawnObjects(objRef, m_HordeWagons[i], true);
            RespawnObjects(objRef, m_AllianceBrokenRoost[i], true);

            m_uiRoostWorldState[i] = aAllianceNeutralStates[i];
        }
    }
}

void WorldPvPNA::RespawnSoldiers(const WorldObject* objRef, Team faction)
{
    if (faction == ALLIANCE)
    {
        // despawn all horde vendors
        for (std::list<ObjectGuid>::const_iterator itr = lHordeSoldiers.begin(); itr != lHordeSoldiers.end(); ++itr)
        {
            if (Creature* pSoldier = objRef->GetMap()->GetCreature(*itr))
            {
                // reset respawn time
                pSoldier->SetRespawnDelay(7 * DAY);
                pSoldier->ForcedDespawn();
            }
        }

        // spawn all alliance soldiers and vendors
        for (std::list<ObjectGuid>::const_iterator itr = lAllianceSoldiers.begin(); itr != lAllianceSoldiers.end(); ++itr)
        {
            if (Creature* pSoldier = objRef->GetMap()->GetCreature(*itr))
            {
                // lower respawn time
                pSoldier->SetRespawnDelay(HOUR);
                pSoldier->Respawn();
            }
        }
    }
    else if (faction == HORDE)
    {
        // despawn all alliance vendors
        for (std::list<ObjectGuid>::const_iterator itr = lAllianceSoldiers.begin(); itr != lAllianceSoldiers.end(); ++itr)
        {
            if (Creature* pSoldier = objRef->GetMap()->GetCreature(*itr))
            {
                // reset respawn time
                pSoldier->SetRespawnDelay(7 * DAY);
                pSoldier->ForcedDespawn();
            }
        }

        // spawn all horde soldiers and vendors
        for (std::list<ObjectGuid>::const_iterator itr = lHordeSoldiers.begin(); itr != lHordeSoldiers.end(); ++itr)
        {
            if (Creature* pSoldier = objRef->GetMap()->GetCreature(*itr))
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
    bool bReturnStatus = false;
    UpdateWyvernsWorldState(WORLD_STATE_REMOVE);

    if (pPlayer->GetTeam() == ALLIANCE)
    {
        for (uint8 i = 0; i < MAX_NA_ROOSTS; ++i)
        {
            if (pGo->GetEntry() == aAllianceWagons[i])
            {
                m_uiRoostWorldState[i] = aHordeNeutralStates[i];
                RespawnObjects(pGo, m_HordeRoost[i], false);
                RespawnObjects(pGo, m_HordeBrokenRoost[i], true);
                bReturnStatus = true;
            }
            else if (pGo->GetEntry() == aAllianceBrokenRoosts[i])
            {
                m_uiRoostWorldState[i] = aAllianceRoostStates[i];
                RespawnObjects(pGo, m_AllianceBrokenRoost[i], false);
                RespawnObjects(pGo, m_AllianceRoost[i], true);
                bReturnStatus = true;
            }
            else if (pGo->GetEntry() == aAllianceRoosts[i])
            {
                // ###### This is hacked in Gameobject.cpp because of the missing custom spells support #####
                // if we can't add any bombs don't do anything
                if (!AddBombsToInventory(pPlayer))
                    return false;

                // makr player as pvp first
                pPlayer->UpdatePvP(true, true);
                pPlayer->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP);

                // Send taxi
                bReturnStatus = HandlePlayerTaxiDrive(pPlayer, i);
            }
        }
    }
    else if (pPlayer->GetTeam() == HORDE)
    {
        for (uint8 i = 0; i < MAX_NA_ROOSTS; ++i)
        {
            if (pGo->GetEntry() == aHordeWagons[i])
            {
                m_uiRoostWorldState[i] = aAllianceNeutralStates[i];
                RespawnObjects(pGo, m_AllianceRoost[i], false);
                RespawnObjects(pGo, m_AllianceBrokenRoost[i], true);
                bReturnStatus = true;
            }
            else if (pGo->GetEntry() == aHordeBrokenRoosts[i])
            {
                m_uiRoostWorldState[i] = aHordeRoostStates[i];
                RespawnObjects(pGo, m_HordeBrokenRoost[i], false);
                RespawnObjects(pGo, m_HordeRoost[i], true);
                bReturnStatus = true;
            }
            else if (pGo->GetEntry() == aHordeRoosts[i])
            {
                // ###### This is hacked in Gameobject.cpp because of the missing custom spells support #####
                // if we can't add any bombs don't do anything
                if (!AddBombsToInventory(pPlayer))
                    return false;

                // makr player as pvp first
                pPlayer->UpdatePvP(true, true);
                pPlayer->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_IN_PVP);

                // Send taxi
                bReturnStatus = HandlePlayerTaxiDrive(pPlayer, i);
            }
        }
    }

    UpdateWyvernsWorldState(WORLD_STATE_ADD);

    return bReturnStatus;
}

// Handle Taxi for Halaa bombing run
bool WorldPvPNA::HandlePlayerTaxiDrive(Player* pPlayer, uint8 uiPos)
{
    std::vector<uint32> vTaxiNodes;
    vTaxiNodes.reserve(2);

    vTaxiNodes[0] = aFlightPathStartNodes[uiPos];
    vTaxiNodes[1] = aFlightPathEndNodes[uiPos];

    // Send taxi
    if (pPlayer->ActivateTaxiPathTo(vTaxiNodes))
        return true;

    return false;
}

// Add fire bombs to player inventory
bool WorldPvPNA::AddBombsToInventory(Player* pPlayer)
{
    uint32 uiSpaceForItems = 0;
    ItemPosCountVec m_Destination;
    uint32 uiBombCount = MAX_FIRE_BOMBS;

    // get the free slots from inventory
    uint8 msg = pPlayer->CanStoreNewItem(NULL_BAG, NULL_SLOT, m_Destination, ITEM_ID_FIRE_BOMB, uiBombCount, &uiSpaceForItems);
    if (msg != EQUIP_ERR_OK)
        uiBombCount -= uiSpaceForItems;

    if (uiBombCount == 0 || m_Destination.empty())
        return false;

    Item* item = pPlayer->StoreNewItem(m_Destination, ITEM_ID_FIRE_BOMB, true);

    if (uiBombCount > 0 && item)
        pPlayer->SendNewItem(item, uiBombCount, true, false);

    return true;
}

void WorldPvPNA::RespawnObjects(const WorldObject* objRef, ObjectGuid goGuid, bool bRespawn)
{
    if (GameObject* pBanner = objRef->GetMap()->GetGameObject(goGuid))
    {
        if (bRespawn)
        {
            pBanner->SetRespawnTime(7 * DAY);
            pBanner->Refresh();
        }
        else if (pBanner->isSpawned())
            pBanner->Delete();
    }
}

void WorldPvPNA::LockHalaa(const WorldObject* objRef, Team faction)
{
    if (GameObject* pBanner = objRef->GetMap()->GetGameObject(m_HalaaBannerGuid))
        pBanner->SetLootState(GO_JUST_DEACTIVATED);

    SetCapturePointSliderValue(m_HalaaBannerGuid, faction == ALLIANCE ? CAPTURE_SLIDER_ALLIANCE_LOCKED : CAPTURE_SLIDER_HORDE_LOCKED);
}

void WorldPvPNA::UnlockHalaa(const WorldObject* objRef, Team faction)
{
    if (GameObject* pBanner = objRef->GetMap()->GetGameObject(m_HalaaBannerGuid))
        pBanner->SetCapturePointSlider(faction == ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE);
        // no banner refresh needed because it already has the correct artkit
    else
        // if grid is unloaded, resetting the slider value is enough
        SetCapturePointSliderValue(m_HalaaBannerGuid, faction == ALLIANCE ? CAPTURE_SLIDER_ALLIANCE : CAPTURE_SLIDER_HORDE);
}
