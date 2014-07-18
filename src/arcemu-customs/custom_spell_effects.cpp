/////////////////////////////////////////////////////////
// SpellEffects.cpp - line - 468
/////////////////////////////////////////////////////////
	case 4011:
	case 4012:
	case 4013:
	case 4014:
		{
			if( !p_caster )
				return;

			int cost_to_function = m_spellInfo->eff[0].EffectBasePoints;

			//regenerate ammo if we can
/*			if( m_spellInfo->EffectBaseDice[0] && (int)p_caster->GetUInt32Value( PLAYER_FIELD_COINAGE ) >= cost_to_function * m_spellInfo->EffectBaseDice[0] )
			{
				Item *dst = p_caster->GetItemInterface()->FindItemLessMax( p_caster->GetUInt32Value( PLAYER_AMMO_ID ), m_spellInfo->EffectBaseDice[0], false);
				if( dst )
				{
					dst->SetCount( dst->GetUInt32Value(ITEM_FIELD_STACK_COUNT) + m_spellInfo->EffectBaseDice[0] );
					p_caster->ModUnsigned32Value( PLAYER_FIELD_COINAGE, -cost_to_function * (int)m_spellInfo->EffectBaseDice[0] );
				}
			}*/

			//heal pet if possible
			if( m_spellInfo->eff[1].EffectBasePoints && (int)p_caster->GetUInt32Value( PLAYER_FIELD_COINAGE ) >= cost_to_function )
			{
				Pet *p = p_caster->GetSummon();
				if( p && p->GetUInt32Value(UNIT_FIELD_HEALTH) < p->GetUInt32Value(UNIT_FIELD_MAXHEALTH) )
				{
					uint32 cur = p->GetUInt32Value(UNIT_FIELD_MAXHEALTH);
					uint32 add = cur * m_spellInfo->eff[1].EffectBasePoints / 100;
					if( cur + add > p->GetUInt32Value(UNIT_FIELD_MAXHEALTH) )
						add = p->GetUInt32Value(UNIT_FIELD_MAXHEALTH) - p->GetUInt32Value(UNIT_FIELD_HEALTH);
					p->ModUnsigned32Value(UNIT_FIELD_HEALTH,add);
					p_caster->ModUnsigned32Value( PLAYER_FIELD_COINAGE, -cost_to_function );
				}
			}

			//add mana to self if possible
			if( m_spellInfo->eff[2].EffectBasePoints && (int)p_caster->GetUInt32Value( PLAYER_FIELD_COINAGE ) >= cost_to_function )
			{
				uint32 cur = p_caster->GetUInt32Value(UNIT_FIELD_MAXPOWER1);
				uint32 add = cur * m_spellInfo->eff[2].EffectBasePoints / 100;
				if( cur + add > p_caster->GetUInt32Value(UNIT_FIELD_MAXPOWER1) )
					add = p_caster->GetUInt32Value(UNIT_FIELD_MAXPOWER1) - p_caster->GetUInt32Value(UNIT_FIELD_POWER1);
				if( add )
				{
					p_caster->ModUnsigned32Value(UNIT_FIELD_POWER1,add);
					p_caster->ModUnsigned32Value( PLAYER_FIELD_COINAGE, -cost_to_function );
				}
			}

		}break;
