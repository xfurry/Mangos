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

#ifndef WORLD_PVP_HP
#define WORLD_PVP_HP

enum
{
    MAX_HP_TOWERS                           = 3,

    TYPE_OVERLOOK_CONTROLLER                = 1,
    TYPE_STADIUM_CONTROLLER                 = 2,
    TYPE_BROKEN_HILL_CONTROLLER             = 3,

    TYPE_OVERLOOK_STATE                     = 4,
    TYPE_STADIUM_STATE                      = 5,
    TYPE_BROKEN_HILL_STATE                  = 6,

    // tower ids
    // they are used in the process event functions
    TOWER_ID_OVERLOOK                       = 1,
    TOWER_ID_STADIUM                        = 2,
    TOWER_ID_BROKEN_HILL                    = 3,

    // spells
    SPELL_HELLFIRE_TOWER_TOKEN_ALY          = 32155,
    SPELL_HELLFIRE_TOWER_TOKEN_HORDE        = 32158,
    SPELL_HELLFIRE_SUPERIORITY_ALY          = 32071,
    SPELL_HELLFIRE_SUPERIORITY_HORDE        = 32049,

    // zone ids
    ZONE_ID_HELLFIRE_PENINSULA              = 3483,
    ZONE_ID_HELLFIRE_CITADEL                = 3563,
    ZONE_ID_HELLFIRE_RAMPARTS               = 3562,
    ZONE_ID_BLOOD_FURNACE                   = 3713,
    ZONE_ID_SHATTERED_HALLS                 = 3714,
    ZONE_ID_MAGTHERIDON_LAIR                = 3836,

    // npcs
    NPC_CAPTURE_CREDIT_OVERLOOK             = 19028,
    NPC_CAPTURE_CREDIT_STADIUM              = 19029,
    NPC_CAPTURE_CREDIT_BROKEN_HILL          = 19032,
    HONOR_REWARD_HELLFIRE                   = 18,

    // gameobjects
    GO_TOWER_BANNER_OVERLOOK                = 182525,
    GO_TOWER_BANNER_STADIUM                 = 183515,
    GO_TOWER_BANNER_BROKEN_HILL             = 183514,

    // capture points
    GO_HELLFIRE_BANNER_OVERLOOK             = 182174,
    GO_HELLFIRE_BANNER_STADIUM              = 182173,
    GO_HELLFIRE_BANNER_BROKEN_HILL          = 182175,

    // events
    //EVENT_OVERLOOK_WIN_ALLIANCE           = 11398,
    //EVENT_OVERLOOK_WIN_HORDE              = 11397,
    //EVENT_OVERLOOK_CONTEST_ALLIANCE       = 11392,
    //EVENT_OVERLOOK_CONTEST_HORDE          = 11391,
    EVENT_OVERLOOK_PROGRESS_ALLIANCE        = 11396,
    EVENT_OVERLOOK_PROGRESS_HORDE           = 11395,
    EVENT_OVERLOOK_NEUTRAL_ALLIANCE         = 11394,
    EVENT_OVERLOOK_NEUTRAL_HORDE            = 11393,

    //EVENT_STADIUM_WIN_ALLIANCE            = 11390,
    //EVENT_STADIUM_WIN_HORDE               = 11389,
    //EVENT_STADIUM_CONTEST_ALLIANCE        = 11384,
    //EVENT_STADIUM_CONTEST_HORDE           = 11383,
    EVENT_STADIUM_PROGRESS_ALLIANCE         = 11388,
    EVENT_STADIUM_PROGRESS_HORDE            = 11387,
    EVENT_STADIUM_NEUTRAL_ALLIANCE          = 11386,
    EVENT_STADIUM_NEUTRAL_HORDE             = 11385,

    //EVENT_BROKEN_HILL_WIN_ALLIANCE        = 11406,
    //EVENT_BROKEN_HILL_WIN_HORDE           = 11405,
    //EVENT_BROKEN_HILL_CONTEST_ALLIANCE    = 11400,
    //EVENT_BROKEN_HILL_CONTEST_HORDE       = 11399,
    EVENT_BROKEN_HILL_PROGRESS_ALLIANCE     = 11404,
    EVENT_BROKEN_HILL_PROGRESS_HORDE        = 11403,
    EVENT_BROKEN_HILL_NEUTRAL_ALLIANCE      = 11402,
    EVENT_BROKEN_HILL_NEUTRAL_HORDE         = 11401,

    // tower artkits
    GO_ARTKIT_BROKEN_HILL_ALY               = 65,
    GO_ARTKIT_BROKEN_HILL_HORDE             = 64,
    GO_ARTKIT_BROKEN_HILL_NEUTRAL           = 66,

    GO_ARTKIT_OVERLOOK_ALY                  = 62,
    GO_ARTKIT_OVERLOOK_HORDE                = 61,
    GO_ARTKIT_OVERLOOK_NEUTRAL              = 63,

    GO_ARTKIT_STADIUM_ALY                   = 67,
    GO_ARTKIT_STADIUM_HORDE                 = 68,
    GO_ARTKIT_STADIUM_NEUTRAL               = 69,

    // world states
    WORLD_STATE_TOWER_COUNT_HP_ALY          = 2476,
    WORLD_STATE_TOWER_COUNT_HP_HORDE        = 2478,

    WORLD_STATE_BROKEN_HILL_ALY             = 2483,
    WORLD_STATE_BROKEN_HILL_HORDE           = 2484,
    WORLD_STATE_BROKEN_HILL_NEUTRAL         = 2485,

    WORLD_STATE_OVERLOOK_ALY                = 2480,
    WORLD_STATE_OVERLOOK_HORDE              = 2481,
    WORLD_STATE_OVERLOOK_NEUTRAL            = 2482,

    WORLD_STATE_STADIUM_ALY                 = 2471,
    WORLD_STATE_STADIUM_HORDE               = 2470,
    WORLD_STATE_STADIUM_NEUTRAL             = 2472,
};

class WorldPvPHP : public WorldPvP
{
    public:
        WorldPvPHP();

        bool InitOutdoorPvPArea();

        void OnGameObjectCreate(GameObject* pGo);
        void ProcessEvent(GameObject* pGo, Player* pPlayer, uint32 uiEventId);

        void HandlePlayerEnterZone(Player* pPlayer);
        void HandlePlayerLeaveZone(Player* pPlayer);
        void HandleObjectiveComplete(PlayerSet m_sPlayersSet, uint32 uiEventId);

        void SetData(uint32 uiType, uint32 uiData);
        uint32 GetData(uint32 uiType);

        void FillInitialWorldStates(WorldPacket& data, uint32& count);
        void SendRemoveWorldStates(Player* pPlayer);
        void UpdateWorldState();

    private:
        // process capture events
        void ProcessCaptureEvent(uint32 uiCaptureType, uint32 uiTeam, uint32 uiTower);

        // set banners artkit
        void SetBannerArtKit(ObjectGuid BannerGuid, uint32 uiArtKit);

        uint32 m_uiBrokenHillController;
        uint32 m_uiStadiumController;
        uint32 m_uiOverlookController;
        uint32 m_uiBrokenHillState;
        uint32 m_uiStadiumState;
        uint32 m_uiOverlookState;
        uint32 m_uiBrokenHillWorldState;
        uint32 m_uiStadiumWorldState;
        uint32 m_uiOverlookWorldState;
        uint32 m_uiTowersAlly;
        uint32 m_uiTowersHorde;

        ObjectGuid m_TowerBannerOverlookGUID;
        ObjectGuid m_TowerBannerStadiumGUID;
        ObjectGuid m_TowerBannerBrokenHillGUID;
        ObjectGuid m_TowerPointOverlookGUID;
        ObjectGuid m_TowerPointStadiumGUID;
        ObjectGuid m_TowerPointBrokenHillGUID;
};

#endif
