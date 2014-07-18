#include "StdAfx.h"

#define REGISTER_HANDLER( spell ) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->EffRedirectHandler == 0 ); \
	sp->EffRedirectHandler = &ER_##spell;
#define REGISTER_SAMEHANDLER(spell,copy_from) sp = dbcSpell.LookupEntryForced( spell ); \
	ASSERT( sp->EffRedirectHandler == 0 ); \
	sp->EffRedirectHandler = &ER_##copy_from;

/*
void ER_30482( Spell *sp )
{
	for( uint32 i = 0; i < MAX_SPELL_EFFECT_COUNT; i++ )
		if( sp->GetProto()->eff[i].EffectApplyAuraName == SPELL_AURA_REDUCE_ATTACKER_CRICTICAL_HIT_CHANCE_PCT )
		{
			//if we have the talent Firestarter then the actuall effect will come from spell 86941
			if( sp->m_caster->IsUnit() && SafeUnitCast( sp->m_caster )->HasAuraWithNameHash( SPELL_HASH_FIRESTARTER, 0, AURA_SEARCH_PASSIVE ) )
			{
				//skip using old effect 
				sp->redirected_effect[ i ] = SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS;
				//cast the new spell. Maybe we are supposed to replace just the effect ID ?
				//casting new spell might help us in proc chains
				SafeUnitCast( sp->m_caster )->CastSpell( sp->m_caster, 86941, true );
			}
		}
}*/

//in this file you can define SPELL_EFFECT_REDIRECT_FLAG_REDIRECT_ALL_TARGETS flags before knowing spelll targets and after cancast result
void RegisterEffectRedirectHandlers()
{
//	SpellEntry *sp;
	//Molten Armor
//	REGISTER_HANDLER( 30482 );
}
