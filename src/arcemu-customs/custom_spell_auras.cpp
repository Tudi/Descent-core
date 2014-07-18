/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// SpellAuras.cpp - line - 1310
/////////////////////////////////////////////////////////
	//Healbot - running after you and healing it pretty good
	case 1588:
		{
			if( !m_target->IsPlayer() )
				return;

			Player * ptarget = static_cast< Player* >( m_target );


			if( apply )
			{
				SetPositive();

				ptarget->RemoveAura( 1588, ptarget->GetGUID(), AURA_SEARCH_ALL, MAX_PASSIVE_AURAS  );

				uint8 race = ptarget->getRace();
				uint32 Entry;

				if( race == RACE_HUMAN || race == RACE_DWARF || race == RACE_NIGHTELF || race == RACE_GNOME || race == RACE_DRAENEI )
					Entry = 1826;
				else
//					Entry = 5473;
					Entry = 1826;

				CreatureProto * pTemplate = CreatureProtoStorage.LookupEntry(Entry);
				CreatureInfo * pCreatureInfo = CreatureNameStorage.LookupEntry(Entry);
				if(!pTemplate || !pCreatureInfo)
					return;
//				Unit *newguard = ptarget->create_guardian(Entry,GetDuration() ,float(-M_PI*2), ptarget->getLevel() );
				Unit *newguard = ptarget->create_guardian( Entry, GetDuration() );
				AiAgentHealSupport *new_interface = new AiAgentHealSupport;
				new_interface->Init(newguard,AITYPE_PET,MOVEMENTTYPE_NONE,ptarget);
				newguard->ReplaceAIInterface( (AIInterface *) new_interface );

				uint64 *t = (uint64 *)&mod->fixed_amount[0];
				*t = newguard->GetGUID();
			}
			else if( ptarget->GetMapMgr() )
			{
				uint64 *t = (uint64 *)&mod->fixed_amount[0];
				Unit *guard = ptarget->GetMapMgr()->GetUnit( *t );
				if( guard && guard->IsCreature() )
					((Creature *)guard)->SummonExpire();
			}
		}break;
	//Universal Translator   
/*	case 36764:
		{
			if( !m_target->IsPlayer() )
				return;

			Player * ptarget = static_cast< Player* >( m_target );

			uint8 race = ptarget->getRace();
			uint8 side;

#define OWNER_SIDE_HORDE 0
#define OWNER_SIDE_ALIANCE 1

			if( race == RACE_HUMAN || race == RACE_DWARF || race == RACE_NIGHTELF || race == RACE_GNOME || race == RACE_DRAENEI )
				side = OWNER_SIDE_ALIANCE;
			else
				side = OWNER_SIDE_HORDE;

			if( apply )
			{
				if( side == OWNER_SIDE_ALIANCE )
				{
					ptarget->_AddSkillLine( SKILL_LANG_ORCISH,300,300 );
//					ptarget->_AddSkillLine( SKILL_LANG_DARNASSIAN,300,300 );
//					ptarget->_AddSkillLine( SKILL_LANG_GUTTERSPEAK,300,300 );
//					ptarget->_AddSkillLine( SKILL_LANG_TAURAHE,300,300 );
//					ptarget->_AddSkillLine( SKILL_LANG_TROLL,300,300 );
				}
				else
				{
					ptarget->_AddSkillLine( SKILL_LANG_COMMON,300,300 );
//					ptarget->_AddSkillLine( SKILL_LANG_DWARVEN,300,300 );
//					ptarget->_AddSkillLine( SKILL_LANG_GNOMISH,300,300 );
//					ptarget->_AddSkillLine( SKILL_LANG_DRAENEI,300,300 );
				}
			}
			else
			{
				if( side == OWNER_SIDE_ALIANCE )
				{
					ptarget->_RemoveSkillLine( SKILL_LANG_ORCISH );
//					ptarget->_RemoveSkillLine( SKILL_LANG_DARNASSIAN );
//					ptarget->_RemoveSkillLine( SKILL_LANG_GUTTERSPEAK );
//					ptarget->_RemoveSkillLine( SKILL_LANG_TAURAHE );
//					ptarget->_RemoveSkillLine( SKILL_LANG_TROLL );
				}
				else
				{
					ptarget->_RemoveSkillLine( SKILL_LANG_COMMON );
//					ptarget->_RemoveSkillLine( SKILL_LANG_DWARVEN );
//					ptarget->_RemoveSkillLine( SKILL_LANG_GNOMISH );
//					ptarget->_RemoveSkillLine( SKILL_LANG_DRAENEI );
				}
			}		
		}break;*/
	//nuclear waste
	case 260:
		{
			if( !GetUnitCaster() || !GetUnitCaster()->IsPlayer() )
				return;
			if( apply )
			{
				SetPositive();

				mod->fixed_amount[0] = (int)(GetUnitCaster()->GetFloatValue( OBJECT_FIELD_SCALE_X ) * 10000.0f);

				int32 selectedrow = 100 + RandomUInt( 24764 - 100 );
				UnitModelSizeEntry  *newmodelhalfsize = UnitModelSizeStorage.LookupEntry( selectedrow );

				//make sure we get one
				while( !newmodelhalfsize && selectedrow>0 )
				{
					selectedrow--;
					if( selectedrow == 0 )
					{
						selectedrow = 100 + RandomUInt( 24764 - 100 );
						return;	//might block in case sql table was empty
					}
					newmodelhalfsize = UnitModelSizeStorage.LookupEntry( selectedrow );
				}

				//get the model size for this creature looks
				//max alowed size is 4
				float size_ratio;
				if( newmodelhalfsize->HalfSize < 4.0f )
					size_ratio = 1.0f;
				else size_ratio = 4.0f / newmodelhalfsize->HalfSize;
				GetUnitCaster()->SetUInt32Value( UNIT_FIELD_DISPLAYID, newmodelhalfsize->DisplayId );
				GetUnitCaster()->SetFloatValue( OBJECT_FIELD_SCALE_X, size_ratio );
				GetUnitCaster()->EventModelChange();
			}
			else
			{
				GetUnitCaster()->SetUInt32Value( UNIT_FIELD_DISPLAYID, GetUnitCaster()->GetUInt32Value( UNIT_FIELD_NATIVEDISPLAYID ) );
				GetUnitCaster()->SetFloatValue( OBJECT_FIELD_SCALE_X, mod->fixed_amount[0] / 10000.0f );
			}
		}break;
	//Pet shop boys
	case 68:
		{
			if( apply )
			{
				SetPositive();

				if( !GetUnitCaster() || !GetUnitCaster()->IsPlayer() )
					return;
				
				Unit *target=static_cast<Player*>(GetUnitCaster())->GetSummon();
				if( !target )
					return;

				int32 selectedrow = 100 + RandomUInt( 24764 - 100 );
				UnitModelSizeEntry  *newmodelhalfsize = UnitModelSizeStorage.LookupEntry( selectedrow );

				//make sure we get one
				while( !newmodelhalfsize )
				{
					selectedrow--;
					if( selectedrow == 0 )
						selectedrow = 100 + RandomUInt( 24764 - 100 );
					newmodelhalfsize = UnitModelSizeStorage.LookupEntry( selectedrow );
				}

				//get the model size for this creature looks
				//max alowed size is 4
				float size_ratio;
				if( newmodelhalfsize->HalfSize < 4.0f )
					size_ratio = 1.0f;
				else size_ratio = 4.0f / newmodelhalfsize->HalfSize;
				target->SetUInt32Value( UNIT_FIELD_DISPLAYID, newmodelhalfsize->DisplayId );
				mod->fixed_amount[0] = (int)(target->GetFloatValue( OBJECT_FIELD_SCALE_X ) * 10000.0f);
				target->SetFloatValue( OBJECT_FIELD_SCALE_X, size_ratio );
				target->EventModelChange();
			}
			else
			{
				if( !GetUnitCaster() || !GetUnitCaster()->IsPlayer() )
					return;

				Unit *target=static_cast<Player*>(GetUnitCaster())->GetSummon();
				if( !target )
					return;

				target->SetUInt32Value( UNIT_FIELD_DISPLAYID, target->GetUInt32Value( UNIT_FIELD_NATIVEDISPLAYID ) );
				target->SetFloatValue( OBJECT_FIELD_SCALE_X, mod->fixed_amount[0] / 10000.0f );
			}
		}break;

