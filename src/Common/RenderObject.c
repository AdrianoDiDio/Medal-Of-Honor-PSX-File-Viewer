// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com
/*
===========================================================================
    Copyright (C) 2018-2024 Adriano Di Dio.
    
    Medal-Of-Honor-PSX-File-Viewer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Medal-Of-Honor-PSX-File-Viewer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Medal-Of-Honor-PSX-File-Viewer.  If not, see <http://www.gnu.org/licenses/>.
===========================================================================
*/

#include "RenderObject.h"

const char *RenderObjectGetStringFromType(RenderObjectType_t RenderObjectType)
{
    switch( RenderObjectType ) {
        case RENDER_OBJECT_CARRY_AUX_ELEMENTS:
            return "Carry Aux Elements";
        case RENDER_OBJECT_PICKUP_AND_EXPLOSIVE:
            return "Pickup And Explosive";
        case RENDER_OBJECT_ENEMY:
            return "Enemy Render Object";
        case RENDER_OBJECT_PLANE:
            return "Airplane";
        case RENDER_OBJECT_MG42:
            return "MG42";
        case RENDER_OBJECT_DOOR:
            return "Door";
        case RENDER_OBJECT_UNKNOWN1:
            return "Unknown1";
        case RENDER_OBJECT_DESTRUCTIBLE_WINDOW:
            return "Destructible Window";
        case RENDER_OBJECT_VALVE:
            return "Valve";
        case RENDER_OBJECT_RADIO:
            return "Radio";
        case RENDER_OBJECT_EXPLOSIVE_CHARGE:
            return "Explosive Charge";
        default:
            return "Unknown";
    }
}

const char *RenderObjectGetWeaponNameFromId(int RenderObjectId)
{
    switch( RenderObjectId ) {
        case RENDER_OBJECT_WEAPON_PISTOL_TYPE_1:
            return "Pistol Type 1";
        case RENDER_OBJECT_WEAPON_SMG_TYPE_1:
            return "SubMachineGun Type 1";
        case RENDER_OBJECT_WEAPON_BAZOOKA:
            return "Bazooka";
        case RENDER_OBJECT_WEAPON_AMERICAN_GRENADE:
            return "American Grenade";
        case RENDER_OBJECT_WEAPON_SHOTGUN:
            return "Shotgun";
        case RENDER_OBJECT_WEAPON_SNIPER_RIFLE:
            return "Sniper Rifle";
        case RENDER_OBJECT_WEAPON_SMG_TYPE_2:
            return "SubMachineGun Type 2";
        case RENDER_OBJECT_WEAPON_DOCUMENT_PAPERS:
            return "Document Papers";
        case RENDER_OBJECT_WEAPON_PISTOL_TYPE_2:
            return "Pistol Type 2";
        case RENDER_OBJECT_WEAPON_PISTOL_TYPE_3:
            return "Pistol Type 3";
        case RENDER_OBJECT_WEAPON_GERMAN_GRENADE:
            return "German Grenade";
        case RENDER_OBJECT_WEAPON_SMG_TYPE_3:
            return "SubMachineGun Type 3";
        case RENDER_OBJECT_WEAPON_M1_GARAND:
            return "M1 Garand";
        default:
            //Should never happen!
            return "Unknown";
    }
}

