#ifndef _SPELL_PROC_HANDLER_GENERIC_
#define _SPELL_PROC_HANDLER_GENERIC_

struct SpellEntry;
struct ProcTriggerSpell;
class Unit;

enum ProcHandlerReturnCodes
{
	PROC_HANDLER_CONTINUE_EXECUTION,				//default execution leads to a spell cast
	PROC_HANDLER_CONTINUE_TO_NEXT,					//we fully handled this event and we need to jump to the next event
};

//caller initializes structure to be shared with handler
//all comunication is made through this structure (input / output)
struct ProcHandlerContextShare
{
	Unit						*in_Caller;				//this is handling the proc event
	Unit						*in_Victim;				//realted to the event, ex: spell casted on target
	SpellEntry					*in_CastingSpell;		//in case there is a spell cast this is target
	SpellEntry					*in_OwnerSpell;			//this proc was created by this spell
	int32						in_dmg;					//raw dmg does not include absorbed dmg
	int32						in_abs;					//the ammount was reduced from base dmg
	int32						*in_dmg_loc;				//pointer to memory location called by event, raw dmg does not include absorbed dmg
	int32						*in_abs_loc;				//pointer to memory location called by event,the ammount was reduced from base dmg
	uint32						in_events_filter_flags;	//we are triggering events filtered by these flags, ex : dmg done
	ProcTriggerSpell			*in_event;				//in the list of events 
	int32						out_dmg_absorb;			//reduce this amount from dmg before we apply the actual dmg (last minute save)
	int32						out_dmg_overwrite[3];	//in case we want to change the value of the casted spell
	ProcHandlerReturnCodes		out_handler_result;		//like break loop or continue loop
	SpellEntry					*out_CastSpell;			//in case we change the actuall proc spell
};

//this is just a demo how the handlers will look like
void DefaultSpellProcHandlerPrototype( ProcHandlerContextShare *context );

//actually add our proc handler functions to owner spells
void	RegisterProcHandlers();
void	RegisterChargeHandlers();

#endif