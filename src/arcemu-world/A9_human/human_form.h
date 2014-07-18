#ifndef _HUMN_FORM_
#define _HUMN_FORM_

//!!! this is only for debug purpuses ! File is inherited form parser
extern char object_type_lookup_table[20][30];

struct fielddesc
{
	char *name;
	char type;	//can be float...
};

extern fielddesc player_field_lookup_table[PLAYER_END+1][1];
extern fielddesc GO_field_lookup_table[GAMEOBJECT_END+1][1];

#endif