/////////////////////////////////////////////////////////
// spellfixes.cpp - line -> end of file
/////////////////////////////////////////////////////////

		//warrior
		sp = dbcSpell.LookupEntryForced(4098);
		if(sp != NULL)
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
		sp = dbcSpell.LookupEntryForced(4099);
		if(sp != NULL)
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
		sp = dbcSpell.LookupEntryForced(4103);
		if(sp != NULL)
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
		sp = dbcSpell.LookupEntryForced(4106);
		if(sp != NULL)
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;

		//rogue
		sp = dbcSpell.LookupEntryForced(4034);
		if(sp != NULL)
			sp->procFlags = PROC_ON_MELEE_ATTACK;
		sp = dbcSpell.LookupEntryForced(4035);
		if(sp != NULL)
			sp->procFlags = PROC_ON_MELEE_ATTACK;
		sp = dbcSpell.LookupEntryForced(4046);
		if(sp != NULL)
			sp->procFlags = PROC_ON_MELEE_ATTACK;
		sp = dbcSpell.LookupEntryForced(4047);
		if(sp != NULL)
			sp->procFlags = PROC_ON_MELEE_ATTACK;

		//paladin
		sp = dbcSpell.LookupEntryForced(4021);
		if(sp != NULL)
		{
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM ;
			sp->procFlags2 |= PROC2_TARGET_SELF;
		}
		sp = dbcSpell.LookupEntryForced(4022);
		if(sp != NULL)
		{
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM ;
			sp->procFlags2 |= PROC2_TARGET_SELF;
		}
		sp = dbcSpell.LookupEntryForced(4023);
		if(sp != NULL)
		{
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM ;
			sp->procFlags2 |= PROC2_TARGET_SELF;
		}
		sp = dbcSpell.LookupEntryForced(4024);
		if(sp != NULL)
		{
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM ;
			sp->procFlags2 |= PROC2_TARGET_SELF;
		}

		//priest
		sp = dbcSpell.LookupEntryForced(4029);
		if(sp != NULL)
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
		sp = dbcSpell.LookupEntryForced(4030);
		if(sp != NULL)
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
		sp = dbcSpell.LookupEntryForced(4031);
		if(sp != NULL)
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
		sp = dbcSpell.LookupEntryForced(4032);
		if(sp != NULL)
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM | PROC_ON_CAST_SPELL;

		//shaman
		sp = dbcSpell.LookupEntryForced(4053);
		if(sp != NULL)
			sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_ON_MELEE_ATTACK_VICTIM;
		sp = dbcSpell.LookupEntryForced(4059);
		if(sp != NULL)
			sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_ON_MELEE_ATTACK_VICTIM;
		sp = dbcSpell.LookupEntryForced(4076);
		if(sp != NULL)
			sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_ON_MELEE_ATTACK_VICTIM;
		sp = dbcSpell.LookupEntryForced(4082);
		if(sp != NULL)
			sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_ON_MELEE_ATTACK_VICTIM;	

		//mage
		sp = dbcSpell.LookupEntryForced(4019);
		if(sp != NULL)
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;

		//shield
		sp = dbcSpell.LookupEntryForced(4004);
		if(sp != NULL)
		{
			sp->procFlags = PROC_ON_ANY_DAMAGE_VICTIM;
			sp->proc_interval = 1000;
		}
		//sword
		sp = dbcSpell.LookupEntryForced(4005);
		if(sp != NULL)
		{
			sp->procFlags = PROC_ON_MELEE_ATTACK | PROC_ON_RANGED_ATTACK;
			sp->proc_interval = 1000;
		}
		//shield 2
		sp = dbcSpell.LookupEntryForced(4117);
		if(sp != NULL)
		{
			sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_ABSORB | PROC_ON_DODGE_VICTIM | PROC_ON_BLOCK_VICTIM;
			sp->proc_interval = 1000;
			sp->ThreatForSpellCoef = 10;
		}
		//nova colar
		sp = dbcSpell.LookupEntryForced(4119);
		if(sp != NULL)
		{
			sp->procFlags = PROC_ON_MELEE_ATTACK_VICTIM | PROC_ON_DODGE_VICTIM | PROC_ON_BLOCK_VICTIM;
			sp->procFlags2 = PROC2_TARGET_SELF;
			sp->proc_interval = 6000;
		}
