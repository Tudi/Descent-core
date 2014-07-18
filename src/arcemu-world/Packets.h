#ifndef __PACKETS_H
#define __PACKETS_H

#pragma pack(push, 1)

struct packetSMSG_SET_FLAT_SPELL_MODIFIER
{
	uint32 unk1;
	uint32 counter;
	uint8 type;		//smt_XXX
	uint8 group;
	float vf;
};

struct packetSMSG_COOLDOWN_EVENT
{
	uint32 spellid;
	uint64 guid;
};

struct packetSMSG_SET_AURA_DURATION
{
	uint8 slot;
	uint32 duration;
};

struct packetSMSG_ITEM_PUSH_RESULT
{
	uint64 guid;
	uint32 received;
	uint32 created;
	uint32 unk1;
	uint8 destbagslot;
	uint32 destslot;
	uint32 entry;
	uint32 suffix;
	uint32 randomprop;
	uint32 count;
	uint32 stackcount;
};

struct ppacket_packed_guid
{
	uint8 len;
	uint8 data[9];

	void operator = (const WoWGuid& e)
	{
		len = e.GetNewGuidLen() + 1;
		data[0] = e.GetNewGuidMask();
		memcpy( &data[1], e.GetNewGuid(), e.GetNewGuidLen() );
	}

	uint8 sz() { return len; }
};

// PPackets are packets with packed guids in them
// this still has more work to do though (The actual sending)
struct ppacketSMSG_SET_EXTRA_AURA_INFO
{
	ppacket_packed_guid target;
	uint8 visual_slot;
	uint32 spellid;
	uint32 duration;
	uint32 duration2;

	uint32 sz() { return 1+4+4+4+target.sz(); }
};

struct packet_SMSG_LEVELUP_INFO
{
	uint32	level;
	uint32	Hp;
	uint32  power[10];
	uint32	Stat0;
	uint32	Stat1;
	uint32	Stat2;
	uint32	Stat3;
	uint32	Stat4;
};

struct packetSMSG_LOG_XP_GAIN_EXTRA
{
	uint64	guid;                                    // Player guid
	uint32	xp;                                      // Normal XP
	uint8	type;                                    // 0 for xp gained from killing creature's and 1 for xp gained from quests
	uint32	restxp;                                  // "Rest XP", is equal to XP for no rest xp message
	float	unk2; //1.0f                             // static data.. Seems to always be 1.0f
	uint8   unk3;									 // 4.x
};

struct packetSMSG_LOG_XP_GAIN
{
	uint64	guid;                                    // Always 0
	uint32	xp;                                      // Normal XP
	uint8	type;                                    // Unknown.. seems to always be 0		
	uint8   unk;									 // 2.4.0 unknown
};

struct packetSMSG_CASTRESULT_EXTRA
{
	uint8   MultiCast;
	uint32	SpellId;
	uint8	ErrorMessage;
	uint32	Extra;
};

struct packetSMSG_CASTRESULT
{
	uint8   MultiCast;	//cast number
	uint32	SpellId;
	uint8	ErrorMessage;
};

struct packetSMSG_BINDPOINT_UPDATE
{
	float	pos_x;
	float	pos_y;
	float	pos_z;
	uint32	mapid;
	uint32	zoneid;
};

struct packetSMSG_SET_PROFICICENCY
{
	uint8	ItemClass;
	uint32	Profinciency;
};

struct packetSMSG_ENVIRONMENTAL_DAMAGE
{
	uint64	Guid;
	uint8	Type;
	uint32	Damage;
	uint32 unk1;
	uint32 unk2;
};

struct packetSMSG_LOGIN_VERIFY_WORLD
{
	uint32 MapId;
	float  X;
	float  Y;
	float  Z;
	float  O;
};

struct packetSMSG_WORLD_STATE_UPDATE
{
	uint32 State;
	uint32 Value;
};

struct packetSMSG_PLAY_SPELL_VISUAL
{
	uint64 guid;
	uint32 visualid;
};

struct packetSMSG_PLAY_SPELL_IMPACT
{
	uint64 guid;
	uint32 visualid;
};

struct packetSMSG_TRADE_STATUS
{
	uint8	GuidMask[2];	//all flavors of random numbers
	uint8	junk1;
	//go6 might be here
	uint32	trade_status;
	uint32	magic2;			//seems to be AB 2A 00 00 a lot of times can be 0 as well
	//go2 might be here
	//go3 might be here
	//go7 might be here
	uint8	junk2;
	uint32	trade_id;		//this is the same as the one we will be using when updateting the status
	float	junk3;
	//go1 might be here
	uint8	unk2[4+4];		//based on guid mask almost all values are here mixed with other crap
};
#pragma pack(pop)

#endif		// __PACKETS_H
