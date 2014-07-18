/////////////////////////////////////////////////////////
// Unit.cpp - line - 1801
/////////////////////////////////////////////////////////
						case 4110:
							{
								//deplete charges
								Item *it = static_cast< Player*>(this)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_BODY);
//								if( !it || it->GetDurability() < (uint32)ospinfo->EffectDicePerLevel[0] )
//									continue;
//								else if( Rand( ospinfo->EffectBaseDice[0] ) )
//									it->SetDurability( it->GetDurability() - (uint32)ospinfo->EffectDicePerLevel[0] );
								//rage conversion
								if( ospinfo->EffectBasePoints[0] )
								{
									uint32 cur = GetUInt32Value(UNIT_FIELD_POWER2);
									uint32 add = dmg * ospinfo->EffectBasePoints[0] / 100 ;
									if( cur + add > GetUInt32Value(UNIT_FIELD_MAXPOWER2) )
										add = GetUInt32Value(UNIT_FIELD_MAXPOWER2) - GetUInt32Value(UNIT_FIELD_POWER2);
									ModUnsigned32Value(UNIT_FIELD_POWER2,add);
								}
								//health conversion
								if( ospinfo->EffectBasePoints[1] )
								{
									uint32 cur = GetUInt32Value(UNIT_FIELD_HEALTH);
									uint32 add = dmg * ospinfo->EffectBasePoints[1] / 100;
									if( cur + add > GetUInt32Value(UNIT_FIELD_MAXHEALTH) )
										add = GetUInt32Value(UNIT_FIELD_MAXHEALTH) - GetUInt32Value(UNIT_FIELD_HEALTH);
									ModUnsigned32Value(UNIT_FIELD_HEALTH,add);
								}
								//attack power bonus
								if( ospinfo->EffectBasePoints[2] )
								{
									SpellEntry *spellInfo = dbcSpell.LookupEntry( ospinfo->EffectTriggerSpell[0] );
									Spell *spell = SpellPool.PooledNew();
									spell->Init(this, spellInfo ,true, NULL);
									if( spell )
									{
										spell->forced_basepoints[0] = dmg * ospinfo->EffectBasePoints[2] / 100;
										SpellCastTargets targets2;
										targets2.m_unitTarget = GetGUID();
										spell->prepare(&targets2);
									}
								}
								continue;
							}break;
						case 4051:
							{
								//deplete charges
								Item *it = static_cast< Player*>(this)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_BODY);
//								if( !it || it->GetDurability() < (uint32)ospinfo->EffectDicePerLevel[0] )
//									continue;
//								else if( Rand( ospinfo->EffectBaseDice[0] ) )
//									it->SetDurability( it->GetDurability() - (uint32)ospinfo->EffectDicePerLevel[0] );

								//avoid loop here
								if( m_extraAttackCounter_lock )
									continue;

								//exra attacks
								m_extraAttackCounter_lock = true;
								for(int i=0;i<ospinfo->EffectBasePoints[0];i++)
									Strike( victim, MELEE, NULL, 0, ospinfo->EffectBasePoints[1], 0, false, false);
								m_extraAttackCounter_lock = false;

								//energy conversion
								if( ospinfo->EffectBasePoints[2] )
								{
									uint32 cur = GetUInt32Value(UNIT_FIELD_POWER4);
									uint32 add = GetUInt32Value(UNIT_FIELD_MAXPOWER4) * ospinfo->EffectBasePoints[2] / 100;
									if( cur + add > GetUInt32Value(UNIT_FIELD_MAXPOWER4) )
										add = GetUInt32Value(UNIT_FIELD_MAXPOWER4) - GetUInt32Value(UNIT_FIELD_POWER4);
									ModUnsigned32Value(UNIT_FIELD_POWER4,add);
								}
								continue;
							}break;
						case 4025:
						case 4026:
						case 4027:
						case 4028:
							{
								//deplete charges
								Item *it = static_cast< Player*>(this)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_TRINKET1);
								if( !it )
									it = static_cast< Player*>(this)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_TRINKET2);
//								if( !it || it->GetDurability() < (uint32)ospinfo->EffectDicePerLevel[0] )
//									continue;
//								else if( Rand( ospinfo->EffectBaseDice[0] ) )
//									it->SetDurability( it->GetDurability() - (uint32)ospinfo->EffectDicePerLevel[0] );
							}break;
						case 4033:
							{

								if( !IsPlayer() )
									continue;

//								int cost_to_function = (int)ospinfo->EffectDicePerLevel[1];
//								if( (int)static_cast< Player*>(this)->GetUInt32Value( PLAYER_FIELD_COINAGE ) < cost_to_function )
//									continue;

								//cast power word shield
								if( flag & PROC_ON_ANY_DAMAGE_VICTIM && itr2->procCharges < GetTickCount() )
								{
									uint32 spell_levels[12] = { 17, 592, 600, 3747, 6065, 6066, 10898, 10899, 10900, 10901, 25217, 25218 };
									uint32 selector = GetUInt32Value( UNIT_FIELD_LEVEL ) / 6;
									if( selector > 11 )
										selector = 11;
									//based on level select a power word shield
									SpellEntry *spellInfo = dbcSpell.LookupEntry( spell_levels[ selector ] );
									itr2->procCharges = GetTickCount() + 45000;
									Spell *spell = SpellPool.PooledNew();
									spell->Init(this, spellInfo ,true, NULL);
									if( spell )
									{
										SpellCastTargets targets2;
										targets2.m_unitTarget = GetGUID();
										spell->prepare(&targets2);
									}
//									static_cast< Player*>(this)->ModUnsigned32Value( PLAYER_FIELD_COINAGE, - cost_to_function );
								}
								//clear cooldowns
//								if( ospinfo->EffectBaseDice[1] && (flag & PROC_ON_CAST_SPELL) )
								{
									PlayerCooldownMap::iterator itr2,itr = static_cast< Player*>(this)->m_cooldownMap[COOLDOWN_TYPE_SPELL].begin();
									for(; itr != static_cast< Player*>(this)->m_cooldownMap[COOLDOWN_TYPE_SPELL].end(); )
									{
										itr2 = itr;
										++itr;
										WorldPacket data(12);
										data.SetOpcode(SMSG_CLEAR_COOLDOWN);
										data << itr2->second.SpellId << GetGUID();
										static_cast< Player*>(this)->GetSession()->SendPacket(&data);
										static_cast< Player*>(this)->m_cooldownMap[COOLDOWN_TYPE_SPELL].erase( itr2 );
									}
									itr = static_cast< Player*>(this)->m_cooldownMap[COOLDOWN_TYPE_CATEGORY].begin();
									for(; itr != static_cast< Player*>(this)->m_cooldownMap[COOLDOWN_TYPE_CATEGORY].end(); )
									{
										itr2 = itr;
										++itr;
										WorldPacket data(12);
										data.SetOpcode(SMSG_CLEAR_COOLDOWN);
										data << itr2->second.SpellId << GetGUID();
										static_cast< Player*>(this)->GetSession()->SendPacket(&data);
										static_cast< Player*>(this)->m_cooldownMap[COOLDOWN_TYPE_CATEGORY].erase( itr2 );
									}
//									static_cast< Player*>(this)->ModUnsigned32Value( PLAYER_FIELD_COINAGE, - cost_to_function );
								}
							}break;
						case 4084:
							{
								//deplete charges
								Item *it = static_cast< Player*>(this)->GetItemInterface()->GetInventoryItem(EQUIPMENT_SLOT_BODY);
//								if( !it || it->GetDurability() < (uint32)ospinfo->EffectDicePerLevel[0] )
//									continue;
//								else if( Rand( ospinfo->EffectBaseDice[0] ) )
//									it->SetDurability( it->GetDurability() - (uint32)ospinfo->EffectDicePerLevel[0] );

//								if( ospinfo->EffectBaseDice[1] && (flag & PROC_ON_MELEE_ATTACK_VICTIM) )
								{
									//cast chain lightning
									uint32 spell_levels[11] = { 421, 930, 2860, 10605, 25439, 25442, 45298, 45300, 45302, 49269, 49271 };
									//lvl 70 = 10 -> 7
									uint32 selector = GetUInt32Value( UNIT_FIELD_LEVEL ) / 7;
									if( selector >= 11 )
										selector = 10;
									//based on level select a power word shield
									SpellEntry *spellInfo = dbcSpell.LookupEntry( spell_levels[ selector ] );
									Spell *spell = SpellPool.PooledNew();
									spell->Init(this, spellInfo ,true, NULL);
									if( spell )
									{
										SpellCastTargets targets2;
										targets2.m_unitTarget = victim->GetGUID();
										spell->prepare(&targets2);
									}
								}

								//avoid loop here
//								if( ospinfo->EffectBaseDice[1] && (flag & PROC_ON_MELEE_ATTACK_VICTIM) )
								{
									if( m_extraAttackCounter_lock )
										continue;
	
									//exra attacks
									m_extraAttackCounter_lock = true;
									for(int i=0;i<ospinfo->EffectBasePoints[0];i++)
										Strike( victim, MELEE, NULL, 0, 0, 0, false, false);
									m_extraAttackCounter_lock = false;
								}

								continue;
							}break;
						case 4020:
							{
								//clear agro on attacker
								if( victim->GetAIInterface() && IsPlayer() && static_cast<Player*>(this)->GetGroup() )
									victim->GetAIInterface()->RemoveThreatByPtr( this );
								continue;
							}break;
						//shield2
						case 4118:
							{
								if( victim == this )
									continue;

								SpellNonMeleeDamageLog(victim, 20230, dmg * 50 / 100, true);

								continue;
							}break;
						//nova colar
						case 4120:
							{
								if( victim == this )
									continue;
								static uint32 spell_ids[] = { 18099,55081,52960,15237,30852,35261,36225,38728,40333,47772,68958,781 };
								static uint32 index_count = sizeof( spell_ids ) / sizeof( uint32 );

								uint32 index_now = RandomUInt() % index_count;
								spellId = spell_ids[ index_now ];

								dmg_overwrite = 500 + RandomUInt() % 500;
								dmg_overwrite1 = 10;
								dmg_overwrite2 = 10;

							}break;
