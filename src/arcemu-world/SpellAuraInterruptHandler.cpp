#include "StdAfx.h"

#define REGISTER_HANDLER( spell ) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->AuraInterruptHandler == 0 ); \
	sp->AuraInterruptHandler = &AIH_##spell;
#define REGISTER_SAMEHANDLER(spell,copy_from) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->AuraInterruptHandler == 0 ); \
	sp->AuraInterruptHandler = &AIH_##copy_from;

//If interrupt flag is matched and function has an additional test condition registered before it gets removed
//you can scripts the same way using charges ! But charges cannot be instant removed, most of the time stack multiple times ..
//ex: fear gets removed on dmg, But it gets removed after a certain amount of dmg and not instantly
bool AIH_1856( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg )
{
	//do not remove these on poison ticks made by us on other target
	if( skip_casted )
	{
		if ( flag & ( AURA_INTERRUPT_ON_HOSTILE_SPELL_INFLICTED | AURA_INTERRUPT_ON_ENTER_COMBAT | AURA_INTERRUPT_ON_START_ATTACK | AURA_INTERRUPT_ON_CAST_SPELL | AURA_INTERRUPT_ON_PVP_ENTER ) )
		{
			SpellEntry *sp = dbcSpell.LookupEntryForced( skip_casted );
			if( sp )
			{
				if( sp->c_is_flags & SPELL_FLAG_IS_POISON )	//crap, we should know if this is "flag" is comming from other people on us or our speels are ticking on others
					return false;
				//hmmm, i wonder if this is ok for spells that also have an instant dmg part
				if( sp->quick_tickcount > 1 )
					return false;
			}
		}
	}
	return true;
}

bool AIH_5782( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg )
{
	//this has a small random chance to not break on damage
	//some say this is based on a dmg pool instead random chance
	if( ( flag & ( AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN | AURA_INTERRUPT_ON_DIRECT_DAMAGE ) ) )
	{
		//get health that changed since we feared the target
/*		int32 health_when_feared = -1;
		for( uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
		{
			if( a->m_modList[i].m_AuraName == SPELL_AURA_MOD_FEAR )
				health_when_feared = a->m_modList[i].fixed_amount[0];
			else if( a->m_modList[i].m_AuraName == SPELL_AURA_MOD_ROOT && a->m_modList[i].fixed_amount[0] != 0 )
				health_when_feared = a->m_modList[i].fixed_amount[0];
		}
		int32 health_change;
		if( health_when_feared == -1 )
			health_change = owner->GetMaxHealth();
		else
			health_change = (int32)health_when_feared - (int32)owner->GetHealth();
		health_change += dmg; */
		int32 health_change = dmg;
		for( uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
		{
			if( a->m_modList[i].m_AuraName == SPELL_AURA_MOD_FEAR )
			{
				health_change += a->m_modList[i].fixed_amount[1];
				a->m_modList[i].fixed_amount[1] += dmg;
			}
			else if( a->m_modList[i].m_AuraName == SPELL_AURA_MOD_ROOT )
			{
				health_change += a->m_modList[i].fixed_amount[1];
				a->m_modList[i].fixed_amount[1] += dmg;
			}
		}
		int32 allowed_change = owner->GetMaxHealth() * 5 / 100;
		if( allowed_change > health_change ) 
		{
			return false;
		}
		else
		{
			SpellEntry *sp = dbcSpell.LookupEntryForced( skip_casted );
			//dots have a high chance to not break dmg
			if( sp && sp->quick_tickcount > 1 )
				return RandChance( 10 );
			//high chance on direct damage to break aura
			return RandChance( 80 );
		}
	}
	return true;
}
/*
bool AIH_20216( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted )
{
	if( skiphash_casted != SPELL_HASH_FLASH_OF_LIGHT 
		&& skiphash_casted != SPELL_HASH_HOLY_LIGHT 
		&& skiphash_casted != SPELL_HASH_HOLY_SHOCK 
		)
	{ 
		return false;
	}
	return true;
}*/
/*
bool AIH_51209( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted )
{
	if( skip_casted == 55095 || skip_casted == 55078 )
	{ 
		return false;
	}
	return true;
}*/

bool AIH_34936( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg )
{
	if( skiphash_casted == 0 && skip_casted )
	{
		SpellEntry *sp = dbcSpell.LookupEntryForced( skip_casted );
		if( sp )
			skiphash_casted = sp->NameHash;
	}

	if( skiphash_casted != SPELL_HASH_SHADOW_BOLT && skiphash_casted != SPELL_HASH_INCINERATE )
	{ 
		return false;
	}
	return true;
}

bool AIH_17941( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg )
{
	if( skiphash_casted == 0 && skip_casted )
	{
		SpellEntry *sp = dbcSpell.LookupEntryForced( skip_casted );
		if( sp )
			skiphash_casted = sp->NameHash;
	}
	if( skiphash_casted != SPELL_HASH_SHADOW_BOLT )
	{ 
		return false;
	}
	return true;
}

bool AIH_49203( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg )
{
	if( skip_casted == 55095 || skip_casted == 55078 )
	{ 
		return false;
	}
	if( skiphash_casted == 0 && skip_casted )
	{
		SpellEntry *sp = dbcSpell.LookupEntryForced( skip_casted );
		if( sp )
			skiphash_casted = sp->NameHash;
	}
	if( skiphash_casted == SPELL_HASH_FROST_FEVER || skiphash_casted == SPELL_HASH_BLOOD_PLAGUE )
	{ 
		return false;
	}
	return true;
}

bool AIH_31661( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg )
{
	//only break on direct dmg
	SpellEntry *sp = dbcSpell.LookupEntryForced( skip_casted );
	if( sp && sp->quick_tickcount > 1 )
	{
		return false;
	}
	if( skiphash_casted == 0 && sp != NULL )
		skiphash_casted = sp->NameHash;
	if( skiphash_casted == SPELL_HASH_DRAGON_S_BREATH //you can spam dragon brath without self remove ?
		|| skiphash_casted == SPELL_HASH_WRATH_OF_TARECGOSA 
		) 
	{ 
		return false;
	}
	return true;
}

bool AIH_122( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg )
{
	//this has a small random chance to not break on damage
	//some say this is based on a dmg pool instead random chance
	if( ( flag & ( AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN | AURA_INTERRUPT_ON_DIRECT_DAMAGE ) ) )
	{
		//get health that changed since we feared the target
/*		uint32 health_when_casted = owner->GetHealth();
		for( uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
			if( a->m_modList[i].m_AuraName == SPELL_AURA_MOD_ROOT )
				health_when_casted = a->m_modList[i].fixed_amount[0];
		int32 health_now = MIN( health_when_casted, owner->GetHealth() );
		int32 health_change = health_when_casted - owner->GetHealth(); */
		int32 health_change = dmg;
		for( uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
		{
			if( a->m_modList[i].m_AuraName == SPELL_AURA_MOD_FEAR )
			{
				health_change += a->m_modList[i].fixed_amount[1];
				a->m_modList[i].fixed_amount[1] += dmg;
			}
			else if( a->m_modList[i].m_AuraName == SPELL_AURA_MOD_ROOT )
			{
				health_change += a->m_modList[i].fixed_amount[1];
				a->m_modList[i].fixed_amount[1] += dmg;
			}
		}
		Unit *u = a->GetUnitCaster();
		int32 allowed_change;
		if( u && u->IsPlayer() && SafePlayerCast( u )->HasGlyphWithID( GLYPH_MAGE_FROST_NOVA ) )
//			allowed_change = owner->GetMaxHealth() * 15 / 100;
			allowed_change = owner->GetMaxHealth() * 20 / 100;
		else
			allowed_change = owner->GetMaxHealth() * 10 / 100;
//		if( Rand( 30 ) == true )
		if( allowed_change > health_change ) 
			return false;
	}
	return true;
}

bool AIH_96294( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg )
{
	if( ( flag & ( AURA_INTERRUPT_ON_ANY_DAMAGE_TAKEN | AURA_INTERRUPT_ON_DIRECT_DAMAGE ) ) 
//		&& ( skiphash_casted == SPELL_HASH_GLYPH_OF_CHAINS_OF_ICE || skiphash_casted == SPELL_HASH_FROST_FEVER )
		)
		return false;
	return true;
}

bool AIH_20066( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg )
{
	if( skip_casted == 31803 )
	{ 
		return false;
	}
	if( skiphash_casted == 0 && skip_casted )
	{
		SpellEntry *sp = dbcSpell.LookupEntryForced( skip_casted );
		if( sp )
			skiphash_casted = sp->NameHash;
	}
	if( skiphash_casted == SPELL_HASH_CENSURE )
	{ 
		return false;
	}
	//someone said it breaks only on direct dmg. Not aoe or dot ?
	return true;
}

bool AIH_1776( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg )
{
	//if Gouge caster has Sanguinary veins then caster bleeds should not break Gouge
	if( skip_casted && skiphash_casted == 0 )
			skiphash_casted = dbcSpell.LookupEntryForced( skip_casted )->NameHash;
	if( skiphash_casted == SPELL_HASH_RUPTURE || skiphash_casted == SPELL_HASH_GARROTE || skiphash_casted == SPELL_HASH_GLYPH_OF_HEMORRHAGE || skiphash_casted == SPELL_HASH_HEMORRHAGE )
	{
		Unit *ACaster = a->GetUnitCaster();
		if( ACaster )
		{
			uint64 *NoBreakChance = (uint64 *)ACaster->GetExtension( EXTENSION_ID_SANGUINARY_VEIN_CHANCE );
			if( NoBreakChance && RandChance( (uint32)*NoBreakChance ) )
				return false;
		}
	}
	return true;
}

bool AIH_MountedCastables( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg )
{
	if( skip_casted )
	{
		SpellEntry *sp = dbcSpell.LookupEntryForced( skip_casted );
		if( sp->Attributes & ATTRIBUTES_MOUNT_CASTABLE )
			return false;
		//daze always dismounts us 
		if( sp->MechanicsTypeFlags & ( 1 << ( MECHANIC_DAZED | MECHANIC_POLYMORPHED ) ) && 
			//except if we fly
			( owner->IsPlayer() == false || SafePlayerCast( owner )->GetSession() == NULL || ( SafePlayerCast( owner )->GetSession()->GetMovementInfo()->flags & ( MOVEFLAG_FLYING14333 | MOVEFLAG_AIR_SUSPENSION14333 ) ) == 0 )
			)
			return true;
	}
	//if we perform the interrupt action
	if( flag & ( AURA_INTERRUPT_ON_START_ATTACK | AURA_INTERRUPT_ON_CAST_SPELL | AURA_INTERRUPT_ON_MOUNT | AURA_INTERRUPT_ON_STEALTH ) )
		return true;
	//if others perform action on us
	return false;
	//hmm, anything that is not damaging ?
	//Patch 3.3.0 (08-Dec-2009): Knockbacks no longer dismount players. If on a flying mount, you will be knocked back a short distance before being able to resume flying.
//	if( flag & AURA_INTERRUPT_ON_HOSTILE_SPELL_INFLICTED )
/*	if( ( flag & ( AURA_INTERRUPT_ON_HOSTILE_SPELL_INFLICTED | AURA_INTERRUPT_ON_START_ATTACK ) ) != 0 && dmg == 0 )
	{
		if( ( dbcSpell.LookupEntryForced( skip_casted )->c_is_flags & SPELL_FLAG_IS_DAMAGING ) == false )
			return false;
	} */
	return true;
}

bool AIH_339( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg )
{
	//may interrupt the effect
	if( RandChance( 25 ) )
		return false;
	//only interrupt on direct dmg, not on dots
	if( skip_casted )
	{
		//only break on direct dmg
		SpellEntry *sp = dbcSpell.LookupEntryForced( skip_casted );
		if( sp && sp->quick_tickcount > 1 )
		{
			for( uint32 i=0;i<MAX_SPELL_EFFECT_COUNT;i++)
				if( IsDirectDamagingSpell( sp, i ) )
					return true;
			return false;
		}
	}
	return true;
}
/*
bool AIH_53476( Unit *owner, Aura *a, uint32 flag, uint32 skip_casted, uint32 skiphash_casted, uint32 dmg )
{
	//do not break on AOE or DOTs
	if( skip_casted )
	{
		SpellEntry *sp = dbcSpell.LookupEntryForced( skip_casted );
		if( sp && ( sp->quick_tickcount > 1 || ( sp->c_is_flags2 & SPELL_FLAG2_IS_ENEMY_AOE_TARGETTING ) ) )
			return false;
	}
	return true;
}*/

void RegisterAuraInterruptHandlers()
{
	SpellEntry *sp;

	//all the mount spells
	uint32 cnt = dbcSpell.GetNumRows();
	for(uint32 x=0; x < cnt; x++)
	{
		SpellEntry * sp = dbcSpell.LookupRow(x);
//		if( sp->Attributes & ATTRIBUTES_MOUNT_CASTABLE )
		if( sp->eff[0].EffectApplyAuraName == SPELL_AURA_MOUNTED )
			sp->AuraInterruptHandler = AIH_MountedCastables;
	}
	//Intervene - only remove on direct spell
//	REGISTER_HANDLER( 53476 );
	//Entangling Roots - some chance to not break
	REGISTER_HANDLER( 339 );
	REGISTER_SAMEHANDLER( 16689, 339 ); //Nature's Grasp
	//Gouge
	REGISTER_HANDLER( 1776 );
	//Repentance
	REGISTER_HANDLER( 20066 );
	//Chains of ice
	REGISTER_HANDLER( 96294 );
	REGISTER_SAMEHANDLER( 96293, 96294 );
	//frost nova
	REGISTER_HANDLER( 122 );
	REGISTER_SAMEHANDLER( 82691, 122 ); //ring of frost
	//Dragon's breath
	REGISTER_HANDLER( 31661 );
	//Hungering Cold
	REGISTER_HANDLER( 49203 );
	//Shadow Trance
	REGISTER_HANDLER( 17941 );
	//Backlash
	REGISTER_HANDLER( 34936 );
	//DK : Frost Fever and blood plague does not remove Hungering Cold
//	REGISTER_HANDLER( 51209 );
	//paladin divine favor
//	REGISTER_HANDLER( 20216 );
	//vanish
	REGISTER_HANDLER( 1856 );
	REGISTER_SAMEHANDLER( 11327, 1856 ); //stealth is not getting removed on poison ticks on target
	REGISTER_SAMEHANDLER( 1784, 1856 ); //stealth is not getting removed on poison ticks on target
	REGISTER_SAMEHANDLER( 5215, 1856 ); //prowl
	//fear
	REGISTER_HANDLER( 5782 );
	REGISTER_SAMEHANDLER( 8122, 5782 );		//Psychic Scream
	REGISTER_SAMEHANDLER( 5484, 5782 );		//Howl of Terror
	REGISTER_SAMEHANDLER( 33395, 5782 );	//Freeze ( water elemental )
	REGISTER_SAMEHANDLER( 51514, 5782 );	//Hex
	REGISTER_SAMEHANDLER( 93974, 5782 );	//Aura of Foreboding
	REGISTER_SAMEHANDLER( 93987, 5782 );	//Aura of Foreboding
	REGISTER_SAMEHANDLER( 83301, 5782 );	//Improved Cone of Cold
	REGISTER_SAMEHANDLER( 83302, 5782 );	//Improved Cone of Cold
	REGISTER_SAMEHANDLER( 4167, 5782 );		//Web
	REGISTER_SAMEHANDLER( 10326, 5782 );	//Turn Evil
	REGISTER_SAMEHANDLER( 5246, 5782 );		//Intimidating Shout
	REGISTER_SAMEHANDLER( 1513, 5782 );		//Scare Beast
}