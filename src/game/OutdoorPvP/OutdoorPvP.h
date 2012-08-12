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

#ifndef OUTDOOR_PVP_H
#define OUTDOOR_PVP_H

#include "OutdoorPvPMgr.h"
#include "../Language.h"
#include "../World.h"
#include "../Player.h"
#include "Common.h"
#include "Policies/Singleton.h"
#include "../SharedDefines.h"
#include "../GameObject.h"
#include "../ObjectMgr.h"

enum CapturePointArtKits
{
    CAPTURE_ARTKIT_ALLIANCE = 2,
    CAPTURE_ARTKIT_HORDE    = 1,
    CAPTURE_ARTKIT_NEUTRAL  = 21
};

enum CapturePointAnimations
{
    CAPTURE_ANIM_ALLIANCE   = 1,
    CAPTURE_ANIM_HORDE      = 0,
    CAPTURE_ANIM_NEUTRAL    = 2
};

typedef std::set<Player*> PlayerSet;

class OutdoorPvP
{
    friend class OutdoorPvPMgr;

    public:
        OutdoorPvP() {}
        ~OutdoorPvP() {}

        // Called when a creature or gameobject is created
        virtual void OnCreatureCreate(Creature* ) {}
        virtual void OnGameObjectCreate(GameObject* ) {}

        // Called on creature death or respawn
        virtual void OnCreatureDeath(Creature* /*creature*/) {}
        virtual void OnCreatureRespawn(Creature* /*creature*/) {}

        // Process Capture event
        virtual void ProcessEvent(uint32 uiEventId, GameObject* pGo) {}

        // called when the zone is initialized
        virtual void FillInitialWorldStates(WorldPacket& /*data*/, uint32& /*count*/) {}

        // called when a player triggers an areatrigger
        virtual bool HandleAreaTrigger(Player* /*player*/, uint32 /*triggerId*/) { return false; }

        // called when a playerd drops a flag
        virtual bool HandleDropFlag(Player* /*player*/, uint32 /*spellId*/) { return false; }

        // called when a playerd uses a gameobject related to outdoor pvp events
        virtual bool HandleObjectUse(Player* /*player*/, GameObject* /*go*/) { return false; }

        // handle npc/player kill
        virtual void HandlePlayerKillInsideArea(Player* /*killer*/, Unit* /*victim*/) {}
        virtual void HandlePlayerKill(Player* killer, Unit* victim);

        // handle capture objective complete
        virtual void HandleObjectiveComplete(uint32 /*eventId*/, std::list<Player*> /*players*/, Team /*team*/) {}

        // init all the outdoor pvp area relates stuff
        virtual bool InitOutdoorPvPArea() { return false; }

        // update - called by the OutdoorPvPMgr
        virtual void Update(uint32 diff) {}

        // send world state update to all players present
        void SendUpdateWorldState(uint32 field, uint32 value);

        // applies buff to a team inside the specific zone
        void BuffTeam(Team team, uint32 spellId, bool remove = false);

        // get banner artkit based on controlling team
        uint32 GetBannerArtKit(Team team, uint32 artKitAlliance, uint32 artKitHorde, uint32 artKitNeutral);

        // set banner visual
        void SetBannerVisual(const WorldObject* objRef, ObjectGuid goGuid, uint32 artKit, uint32 animId);
        void SetBannerVisual(GameObject* go, uint32 artKit, uint32 animId);

    protected:

        // Player related stuff
        virtual void HandlePlayerEnterZone(Player* player);
        virtual void HandlePlayerLeaveZone(Player* player);

        // remove world states
        virtual void SendRemoveWorldStates(Player* player) {}

        void RegisterZone(uint32 zoneId);
        void SetCapturePointSliderValue(uint32 entry, CapturePointSlider value);

        // store the players inside the area depending on the team
        PlayerSet m_zonePlayers;
};

#endif
