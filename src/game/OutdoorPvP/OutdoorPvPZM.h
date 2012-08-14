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

#ifndef WORLD_PVP_ZM
#define WORLD_PVP_ZM

#include "Common.h"
#include "OutdoorPvP.h"
#include "../Language.h"

enum
{
    MAX_ZM_TOWERS                           = 2,

    // npcs
    NPC_ALLIANCE_FIELD_SCOUT                = 18581,
    NPC_HORDE_FIELD_SCOUT                   = 18564,

    // these 2 npcs act as an artkit
    NPC_PVP_BEAM_RED                        = 18757,
    NPC_PVP_BEAM_BLUE                       = 18759,

    // gameobjects
    GO_ZANGA_BANNER_WEST                    = 182522,
    GO_ZANGA_BANNER_EAST                    = 182523,
    GO_ZANGA_BANNER_CENTER_ALLIANCE         = 182527,
    GO_ZANGA_BANNER_CENTER_HORDE            = 182528,
    GO_ZANGA_BANNER_CENTER_NEUTRAL          = 182529,

    // spells
    SPELL_TWIN_SPIRE_BLESSING               = 33779,
    SPELL_BATTLE_STANDARD_ALLIANCE          = 32430,
    SPELL_BATTLE_STANDARD_HORDE             = 32431,

    SPELL_ZANGA_TOWER_TOKEN_ALLIANCE        = 32155,
    SPELL_ZANGA_TOWER_TOKEN_HORDE           = 32158,

    SPELL_BEAM_RED                          = 32839,
    SPELL_BEAM_BLUE                         = 32840,

    // misc
    GRAVEYARD_ID_TWIN_SPIRE                 = 969,
    GRAVEYARD_ZONE_TWIN_SPIRE               = 3521,

    // events
    //EVENT_EAST_BEACON_CONTEST_ALLIANCE    = 11816,
    //EVENT_EAST_BEACON_CONTEST_HORDE       = 11817,
    EVENT_EAST_BEACON_PROGRESS_ALLIANCE     = 11807,
    EVENT_EAST_BEACON_PROGRESS_HORDE        = 11806,
    EVENT_EAST_BEACON_NEUTRAL_ALLIANCE      = 11814,
    EVENT_EAST_BEACON_NEUTRAL_HORDE         = 11815,

    //EVENT_WEST_BEACON_CONTEST_ALLIANCE    = 11813,
    //EVENT_WEST_BEACON_CONTEST_HORDE       = 11812,
    EVENT_WEST_BEACON_PROGRESS_ALLIANCE     = 11805,
    EVENT_WEST_BEACON_PROGRESS_HORDE        = 11804,
    EVENT_WEST_BEACON_NEUTRAL_ALLIANCE      = 11808,
    EVENT_WEST_BEACON_NEUTRAL_HORDE         = 11809,

    // world states
    WORLD_STATE_BEACON_EAST_ALLIANCE        = 2558,
    WORLD_STATE_BEACON_EAST_HORDE           = 2559,
    WORLD_STATE_BEACON_EAST_NEUTRAL         = 2560,

    WORLD_STATE_BEACON_WEST_ALLIANCE        = 2555,
    WORLD_STATE_BEACON_WEST_HORDE           = 2556,
    WORLD_STATE_BEACON_WEST_NEUTRAL         = 2557,

    // map world states
    WORLD_STATE_TOWER_EAST_ALLIANCE         = 2650,
    WORLD_STATE_TOWER_EAST_HORDE            = 2651,
    WORLD_STATE_TOWER_EAST_NEUTRAL          = 2652,

    WORLD_STATE_TOWER_WEST_ALLIANCE         = 2644,
    WORLD_STATE_TOWER_WEST_HORDE            = 2645,
    WORLD_STATE_TOWER_WEST_NEUTRAL          = 2646,

    WORLD_STATE_GRAVEYARD_ALLIANCE          = 2648,
    WORLD_STATE_GRAVEYARD_HORDE             = 2649,
    WORLD_STATE_GRAVEYARD_NEUTRAL           = 2647,

    WORLD_STATE_HORDE_FLAG_READY            = 2658,
    WORLD_STATE_HORDE_FLAG_NOT_READY        = 2657,
    WORLD_STATE_ALLIANCE_FLAG_READY         = 2655,
    WORLD_STATE_ALLIANCE_FLAG_NOT_READY     = 2656,

    //WORLD_STATE_UNK                       = 2653,
};

struct ZangaTowerEvent
{
    uint32  eventEntry;
    Team    team;
    uint32  zoneText;
    uint32  worldState;
    uint32  mapState;
};

static const ZangaTowerEvent ZANGA_TOWER_EVENTS[MAX_ZM_TOWERS][4] =
{
    {
        {EVENT_EAST_BEACON_PROGRESS_ALLIANCE,   ALLIANCE,   LANG_OPVP_ZM_CAPTURE_EAST_BEACON_A, WORLD_STATE_TOWER_EAST_ALLIANCE,    WORLD_STATE_BEACON_EAST_ALLIANCE},
        {EVENT_EAST_BEACON_PROGRESS_HORDE,      HORDE,      LANG_OPVP_ZM_CAPTURE_EAST_BEACON_H, WORLD_STATE_TOWER_EAST_HORDE,       WORLD_STATE_BEACON_EAST_HORDE},
        {EVENT_EAST_BEACON_NEUTRAL_HORDE,       TEAM_NONE,  0,                                  WORLD_STATE_TOWER_EAST_NEUTRAL,     WORLD_STATE_BEACON_EAST_NEUTRAL},
        {EVENT_EAST_BEACON_NEUTRAL_ALLIANCE,    TEAM_NONE,  0,                                  WORLD_STATE_TOWER_EAST_NEUTRAL,     WORLD_STATE_BEACON_EAST_NEUTRAL},
    },
    {
        {EVENT_WEST_BEACON_PROGRESS_ALLIANCE,   ALLIANCE,   LANG_OPVP_ZM_CAPTURE_WEST_BEACON_A, WORLD_STATE_TOWER_WEST_ALLIANCE,    WORLD_STATE_BEACON_WEST_ALLIANCE},
        {EVENT_WEST_BEACON_PROGRESS_HORDE,      HORDE,      LANG_OPVP_ZM_CAPTURE_WEST_BEACON_H, WORLD_STATE_TOWER_WEST_HORDE,       WORLD_STATE_BEACON_WEST_HORDE},
        {EVENT_WEST_BEACON_NEUTRAL_HORDE,       TEAM_NONE,  0,                                  WORLD_STATE_TOWER_WEST_NEUTRAL,     WORLD_STATE_BEACON_WEST_NEUTRAL},
        {EVENT_WEST_BEACON_NEUTRAL_ALLIANCE,    TEAM_NONE,  0,                                  WORLD_STATE_TOWER_WEST_NEUTRAL,     WORLD_STATE_BEACON_WEST_NEUTRAL},
    },
};

static const uint32 ZANGA_TOWERS[MAX_ZM_TOWERS] = {GO_ZANGA_BANNER_EAST, GO_ZANGA_BANNER_WEST};

class OutdoorPvPZM : public OutdoorPvP
{
    public:
        OutdoorPvPZM();

        void OnCreatureCreate(Creature* creature);
        void OnGameObjectCreate(GameObject* go);
        void ProcessEvent(uint32 eventId, GameObject* go);

        void HandlePlayerEnterZone(Player* player, bool isMainZone);
        void HandlePlayerLeaveZone(Player* player, bool isMainZone);
        void HandlePlayerKillInsideArea(Player* player, Unit* victim);

        void FillInitialWorldStates(WorldPacket& data, uint32& count);
        void SendRemoveWorldStates(Player* player);

        bool HandleObjectUse(Player* player, GameObject* go);

    private:
        // process capture events
        void ProcessCaptureEvent(GameObject* go, uint32 towerId, Team team, uint32 newWorldState, uint32 newMapState);

        // handles scout world states and gossip - ToDo: implement gossip based on condition
        void PrepareFactionScouts(const WorldObject* objRef, Team team);
        void ResetScouts(const WorldObject* objRef, Team team, bool includeWorldStates = true);

        // handle graveyard team banners
        void SetGraveyardArtKit(const WorldObject* objRef, ObjectGuid goGuid, bool respawn);

        // respawn npcs which act as an artkit visual
        void SetBeaconArtKit(const WorldObject* objRef, ObjectGuid creatureGuid, uint32 auraId);

        uint32 m_towerWorldState[MAX_ZM_TOWERS];
        uint32 m_towerMapState[MAX_ZM_TOWERS];

        Team m_towerOwner[MAX_ZM_TOWERS];
        Team m_graveyardOwner;

        uint32 m_graveyardWorldState;
        uint32 m_scoutWorldStateAlliance;
        uint32 m_scoutWorldStateHorde;
        uint8 m_towersAlliance;
        uint8 m_towersHorde;

        ObjectGuid m_towerBanners[MAX_ZM_TOWERS];
        ObjectGuid m_graveyardBannerAlliance;
        ObjectGuid m_graveyardBannerHorde;
        ObjectGuid m_graveyardBannerNeutral;

        ObjectGuid m_allianceScout;
        ObjectGuid m_hordeScout;
        ObjectGuid m_beamTowerBlue[MAX_ZM_TOWERS];
        ObjectGuid m_beamTowerRed[MAX_ZM_TOWERS];
        ObjectGuid m_beamGraveyardBlue;
        ObjectGuid m_beamGraveyardRed;
};

#endif
