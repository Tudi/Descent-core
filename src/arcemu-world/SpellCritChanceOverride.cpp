#include "StdAfx.h"

#define REGISTER_HANDLER( spell ) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->CritHandler == 0 ); \
	sp->CritHandler = &CCHANCE_##spell;
#define REGISTER_SAMEHANDLER(spell,copy_from) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->CritHandler == 0 ); \
	sp->CritHandler = &CCHANCE_##copy_from;

void CCHANCE_8092(ProcHandlerContextShare *context)
{
	if( context->in_Victim )
	{
		Aura *p = context->in_Victim->HasAuraWithNameHash( SPELL_HASH_MIND_SPIKE, 0, AURA_SEARCH_NEGATIVE );
		if( p && p->m_visualSlot < MAX_VISIBLE_AURAS )
		{
			uint32 c = context->in_Victim->m_auraStackCount[ p->m_visualSlot ];
			if( c )
				context->out_dmg_overwrite[0] = context->in_dmg + c * 30;
		}
	}
}

void CCHANCE_51505(ProcHandlerContextShare *context)
{
	if( context->in_Victim && context->in_Victim->HasAuraWithNameHash( SPELL_HASH_FLAME_SHOCK, 0, AURA_SEARCH_NEGATIVE ) )
		context->out_dmg_overwrite[0] = 200;	//well this should proc for sure
}

void CCHANCE_879(ProcHandlerContextShare *context)
{
	if( context->in_Victim )
	{
		if( context->in_Victim->getRace() == UNDEAD )
			context->out_dmg_overwrite[0] = 200;	//well this should proc for sure
		else if( context->in_Victim->IsCreature() 
			&& SafeCreatureCast( context->in_Victim )->GetCreatureInfo() 
			&& SafeCreatureCast( context->in_Victim )->GetCreatureInfo()->Type == DEMON
			)
			context->out_dmg_overwrite[0] = 200;	//well this should proc for sure
	}
}

void CCHANCE_56641(ProcHandlerContextShare *context)
{
	if( context->in_Caller 
		&& context->in_Victim
		&& context->in_Victim->GetHealthPct() >= 90 //targets above 80% hp
		&& context->in_Caller->IsPlayer() )
	{
		int64 *mod_crit_chance = (int64 *)context->in_Caller->GetExtension( EXTENSION_ID_CAREFUL_AIM_CRITCHANCE );
		if( mod_crit_chance )
			context->out_dmg_overwrite[0] += (int32)(*mod_crit_chance);	
	}
}

void CCHANCE_22568(ProcHandlerContextShare *context)
{
	if( context->in_Caller 
		&& context->in_Victim
		&& context->in_Caller->IsPlayer() 
		&& context->in_Victim->HasAuraStateFlag( AURASTATE_BLEED )
		)
	{
		Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_REND_AND_TEAR, 0, AURA_SEARCH_PASSIVE );
		if( a )
			context->out_dmg_overwrite[0] += a->GetSpellProto()->eff[1].EffectBasePoints;	
	}
}

void CCHANCE_6785(ProcHandlerContextShare *context)
{
	if( context->in_Caller 
		&& context->in_Victim
		&& context->in_Caller->IsPlayer() 
		&& context->in_Victim->GetHealthPct() >= 80
		)
	{
		Aura *a = context->in_Caller->HasAuraWithNameHash( SPELL_HASH_PREDATORY_STRIKES, 0, AURA_SEARCH_PASSIVE );
		if( a )
			context->out_dmg_overwrite[0] += a->GetSpellProto()->eff[0].EffectBasePoints;	
	}
}

void CCHANCE_5676(ProcHandlerContextShare *context)
{
	void *CritMod = context->in_Caller->GetExtension( EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_TARGET ); 
	if( CritMod && *(uint64*)(CritMod) == context->in_Victim->GetGUID() && *(context->in_Caller->GetCreateIn64Extension(EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_SPELL)) == context->in_CastingSpell->Id )
	{
		context->out_dmg_overwrite[0] += *(context->in_Caller->GetCreateIn64Extension( EXTENSION_ID_ONETIME_TARGETSPECIFIC_CRIT_MOD_VAL ));
		*(uint64*)(CritMod) = 0;
	}
	void *CritMod2 = context->in_Caller->GetExtension( EXTENSION_ID_SEARING_PAIN_CRITMOD ); 
	if( CritMod2 )
	{
		uint32 CurHealthPCT = context->in_Victim->GetHealthPct();
		void *NeedPCT = context->in_Caller->GetExtension( EXTENSION_ID_SEARING_PAIN_HP_LIMIT ); 
		if( CurHealthPCT < *(int64*)NeedPCT )
			context->out_dmg_overwrite[0] += *(int64*)CritMod2;
	}
}

void CCHANCE_30455(ProcHandlerContextShare *context)
{
	Unit *caster = context->in_Caller;
	//avoid double mod
	if( caster->IsPlayer() && 
		SafePlayerCast( caster )->m_FrozenCritChanceMultiplier > 0 //comes from talent Shatter ( active buff )
		&& context->in_Victim->HasAuraStateFlag( AURASTATE_FROZEN ) == false )
	{
		//mod them if we have fingers of frost
		if( SafePlayerCast( caster )->m_ForceTargetFrozen > 0 && ( context->in_CastingSpell->NameHash == SPELL_HASH_ICE_LANCE || context->in_CastingSpell->NameHash == SPELL_HASH_DEEP_FREEZE ) )
			context->out_dmg_overwrite[0] = context->in_dmg * SafePlayerCast( caster )->m_FrozenCritChanceMultiplier;
	}
}

void RegisterCritChanceHandlers()
{
	SpellEntry *sp;
	//Ice Lance 
	REGISTER_HANDLER( 30455 );
	REGISTER_SAMEHANDLER( 44572, 30455 ); //Deep Freez
	//Searing Pain 
	REGISTER_HANDLER( 5676 );
	//ravage 
	REGISTER_HANDLER( 6785 );
	//Rend and Tear - Ferocious Bite 
	REGISTER_HANDLER( 22568 );
	//steady shot
	REGISTER_HANDLER( 56641 );
	//cobra shot
	REGISTER_SAMEHANDLER( 77767, 56641 );
	//aimed shot
	REGISTER_SAMEHANDLER( 19434, 56641 );
	REGISTER_SAMEHANDLER( 82928, 56641 );
	//Exorcism
	REGISTER_HANDLER( 879 );
	//Mind blast crit chance should be moded by some target aura :(
	REGISTER_HANDLER( 8092 );
	//Lava Burst
	REGISTER_HANDLER( 51505 );
}
