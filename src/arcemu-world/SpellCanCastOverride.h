/*
 * ArcEmu MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2008 <http://www.ArcEmu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _SPELL_CANCAST_OVERRIDE_H
#define _SPELL_CANCAST_OVERRIDE_H

class Spell;

class SpellCanCastScript
{
public:
	SpellCanCastScript() { OwnerSpell = NULL; CasterGUID = 0; CanCastFilterHandler = NULL; CasterGroupId = (uint32)-1;}
	SpellEntry	*OwnerSpell;			//the spell that created this effect
	uint64		CasterGUID;
	uint32		CasterGroupId;			//-1 if caster had no group !
//	float		CastX,CastY,CastZ;
//	int32		mod_amount[3];			//mod effect with this flat value
	uint32		(*CanCastFilterHandler)(Spell *sp, SpellCanCastScript *ccs );			//caster can have filters to cast new spells
};

class SpellCanTargetedScript
{
public:
	SpellCanTargetedScript() 
	{ 
		OwnerSpell = NULL; CasterGUID = 0; CanTargetedFilterHandler = NULL;
		CasterGroupId = (uint32)-1;
//		CastX = CastY = CastZ = Radius = 0.0f; 
	}
	SpellEntry	*OwnerSpell;		//the spell that created this effect
	uint64		CasterGUID;			//the person casting this aura(filter) on us
	uint32		CasterGroupId;		//-1 if caster had no group !
	uint64		OwnerGuid;			//we are the owner of the filter
//	float		CastX,CastY,CastZ;	//this spell was created for smoke bomb
//	float		Radius;
	uint32		(*CanTargetedFilterHandler)( Spell *sp, Object *Caster, Object *FilterOwner, SpellCanTargetedScript *cts );	//can we get targeted by some spells ? immunity handlers
};


#endif