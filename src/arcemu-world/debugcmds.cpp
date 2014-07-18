/*
 * ArcEmu MMORPG Server
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2008 <http://www.ArcEmu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/////////////////////////////////////////////////
//  Debug Chat Commands
//

#include "StdAfx.h"

bool ChatHandler::HandleDebugInFrontCommand(const char* args, WorldSession *m_session)
{
	Object *obj;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid != 0)
	{
		if((obj = m_session->GetPlayer()->GetMapMgr()->GetUnit(guid)) == 0)
		{
			SystemMessage(m_session, "You should select a character or a creature.");
			return true;
		}
	}
	else
		obj = (Object*)m_session->GetPlayer();

	char buf[256];
	snprintf((char*)buf, 256, "%d", m_session->GetPlayer()->isInFront( obj ) );

	SystemMessage(m_session, buf);

	return true;
}

bool ChatHandler::HandleShowReactionCommand(const char* args, WorldSession *m_session)
{
	Object *obj = NULL;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid != 0)
	{
		obj = m_session->GetPlayer()->GetMapMgr()->GetCreature( guid );
	}

	if(!obj)
	{
		SystemMessage(m_session, "You should select a creature.");
		return true;
	}


	char* pReaction = strtok((char*)args, " ");
	if (!pReaction)
		return false;

	uint32 Reaction  = atoi(pReaction);

	WorldPacket data(SMSG_AI_REACTION, 12);
	data << obj->GetGUID() << uint32(Reaction);
	m_session->SendPacket( &data );

	std::stringstream sstext;
	sstext << "Sent Reaction of " << Reaction << " to " << obj->GetUIdFromGUID() << '\0';

	SystemMessage(m_session,  sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleDistanceCommand(const char* args, WorldSession *m_session)
{
	Object *obj;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid != 0)
	{
		if( (obj = (m_session->GetPlayer()->GetMapMgr()->GetUnit(guid))) == NULL )
		{
			SystemMessage(m_session, "You should select a character or a creature.");
			return true;
		}
	}
	else
		obj = m_session->GetPlayer();

	float dist = m_session->GetPlayer()->CalcDistance(obj);
	std::stringstream sstext;
	sstext << "Distance is: " << dist <<'\0';

	SystemMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleMoveInfoCommand(const char* args, WorldSession *m_session)
{
	Object *obj;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if((obj = m_session->GetPlayer()->GetMapMgr()->GetCreature( guid )) == NULL )
	{
		SystemMessage(m_session, "You should select a character or a creature.");
		return true;
	}

	float dist = m_session->GetPlayer()->CalcDistance(obj);
	bool minfront = obj->isInFront(m_session->GetPlayer());
	bool pinfront = m_session->GetPlayer()->isInFront(obj);
	uint32 movetype = SafeUnitCast(obj)->GetAIInterface()->getMoveType();
	bool run = SafeUnitCast(obj)->GetAIInterface()->getMoveRunFlag();
	uint32 attackerscount = (uint32)SafeUnitCast(obj)->GetAIInterface()->getAITargetsCount();
	uint32 creatureState = SafeUnitCast(obj)->GetAIInterface()->m_creatureState;
	uint32 curwp = SafeUnitCast(obj)->GetAIInterface()->getCurrentWaypoint();
//	Unit* unitToFollow = ((Creature *)obj)->GetAIInterface()->getUnitToFollow();
	uint32 aistate = SafeUnitCast(obj)->GetAIInterface()->getAIState();
	uint32 aitype = SafeUnitCast(obj)->GetAIInterface()->getAIType();
	uint32 aiagent = SafeUnitCast(obj)->GetAIInterface()->getCurrentAgent();
	uint32 lowfollow = 0;
	uint32 highfollow = 0;
	/*if(unitToFollow == NULL)
	{
		lowfollow = 0;
		highfollow = 0;
	}
	else
	{
		lowfollow = unitToFollow->GetGUIDLow();
		highfollow = unitToFollow->GetGUIDHigh();;
	}*/

	std::stringstream sstext;
	sstext << "Following Unit: Low: " << lowfollow << " High: " << highfollow << "\n";
	sstext << "Distance is: " << dist << "\n";
	sstext << "Mob Facing Player: " << minfront << " Player Facing Mob: " << pinfront << "\n";
	sstext << "Attackers Count: " << attackerscount << "\n";
	sstext << "Creature State: " << creatureState << " Run: " << run << "\n";
	sstext << "AIState: " << aistate << " AIType: " << aitype << " AIAgent: " << aiagent << "\n";
	sstext << "MoveType: " << movetype << " Current Waypoint: " << curwp << "\n";

	SendMultilineMessage(m_session, sstext.str().c_str());
	//FillSystemMessageData(&data, sstext.str().c_str());
	//m_session->SendPacket( &data );

	return true;
}

bool ChatHandler::HandleAIMoveCommand(const char* args, WorldSession *m_session)
{
	Object *obj = NULL;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid != 0)
	{
		obj = m_session->GetPlayer()->GetMapMgr()->GetCreature( guid );
	}

	if(obj == NULL)
	{
		SystemMessage(m_session, "You should select a creature.");
		return true;
	}

	//m_session->GetPlayer()->GetOrientation();

	uint32 Move  = 1;
	uint32 Run  = 0;
	uint32 Time = 0;
	uint32 Meth = 0;

	char* pMove = strtok((char*)args, " ");
	if (pMove)
		Move  = atoi(pMove);

	char* pRun = strtok(NULL, " ");
	if (pRun)
		Run  = atoi(pRun);

	char* pTime = strtok(NULL, " ");
	if (pTime)
		Time  = atoi(pTime);

	char* pMeth = strtok(NULL, " ");
	if (pMeth)
		Meth  = atoi(pMeth);

	float x = m_session->GetPlayer()->GetPositionX();
	float y = m_session->GetPlayer()->GetPositionY();
	float z = m_session->GetPlayer()->GetPositionZ();
	float o = m_session->GetPlayer()->GetOrientation();
	SafeUnitCast(obj)->GetAIInterface()->setMoveRunFlag((Run>0?true:false));
	float distance = obj->CalcDistance(x,y,z);
	if(Move == 1)
	{
		if(Meth == 1)
		{
			float q = distance-0.5f;
			x = (obj->GetPositionX()+x*q)/(1+q);
			y = (obj->GetPositionY()+y*q)/(1+q);
			z = (obj->GetPositionZ()+z*q)/(1+q);
		}
		else if(Meth == 2)
		{
			float q = distance-1;
			x = (obj->GetPositionX()+x*q)/(1+q);
			y = (obj->GetPositionY()+y*q)/(1+q);
			z = (obj->GetPositionZ()+z*q)/(1+q);
		}
		else if(Meth == 3)
		{
			float q = distance-2;
			x = (obj->GetPositionX()+x*q)/(1+q);
			y = (obj->GetPositionY()+y*q)/(1+q);
			z = (obj->GetPositionZ()+z*q)/(1+q);
		}
		else if(Meth == 4)
		{
			float q = distance-2.5f;
			x = (obj->GetPositionX()+x*q)/(1+q);
			y = (obj->GetPositionY()+y*q)/(1+q);
			z = (obj->GetPositionZ()+z*q)/(1+q);
		}
		else if(Meth == 5)
		{
			float q = distance-3;
			x = (obj->GetPositionX()+x*q)/(1+q);
			y = (obj->GetPositionY()+y*q)/(1+q);
			z = (obj->GetPositionZ()+z*q)/(1+q);
		}
		else if(Meth == 6)
		{
			float q = distance-3.5f;
			x = (obj->GetPositionX()+x*q)/(1+q);
			y = (obj->GetPositionY()+y*q)/(1+q);
			z = (obj->GetPositionZ()+z*q)/(1+q);
		}
		else
		{
			float q = distance-4;
			x = (obj->GetPositionX()+x*q)/(1+q);
			y = (obj->GetPositionY()+y*q)/(1+q);
			z = (obj->GetPositionZ()+z*q)/(1+q);
		}
		SafeUnitCast(obj)->GetAIInterface()->MoveTo(x,y,z,0);
	}
	else
	{
		uint32 moveTime = 0;
		if(!Time)
		{
			//float dx = x - obj->GetPositionX();
			//float dy = y - obj->GetPositionY();
			//float dz = z - obj->GetPositionZ();

			//float distance = sqrt((dx*dx) + (dy*dy) + (dz*dz));
			if(!distance)
			{
				SystemMessage(m_session, "The Creature is already there.");
				return true;
			}

			float moveSpeed = 1.0f;
			if(!Run)
			{
				moveSpeed = 2.5f*0.001f;
			}
			else
			{
				moveSpeed = 7.0f*0.001f;
			}

			moveTime = uint32(distance / moveSpeed);
		}
		else
		{
			moveTime = Time;
		}
		//((Creature *)obj)->setMovementState(MOVING);
		SafeUnitCast(obj)->GetAIInterface()->SendMoveToPacket(x,y,z,o,moveTime,Run);
	}
	return true;
}

bool ChatHandler::HandleFaceCommand(const char* args, WorldSession *m_session)
{

	Object *obj = NULL;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid != 0)
	{
		obj = m_session->GetPlayer()->GetMapMgr()->GetCreature( guid );
	}

	if(obj == NULL)
	{
		SystemMessage(m_session,  "You should select a creature.");
		return true;
	}

	uint32 Orentation = 0;
	char* pOrentation = strtok((char*)args, " ");
	if (pOrentation)
		Orentation  = atoi(pOrentation);

	/* Convert to Blizzards Format */
	float theOrientation = Orentation/(360/float(6.28));

	obj->SetPosition(obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), theOrientation, false);

	/*
	data.Initialize( SMSG_MONSTER_MOVE );
	data << obj->GetGUID();
	data << obj->GetPositionX() << obj->GetPositionY() << obj->GetPositionZ() << obj->GetOrientation();
	data << uint8(1);
	
	data << uint32(0x100); //run
	data << uint32(0); //time
	data << uint32(2);
	data << obj->GetPositionX() << obj->GetPositionY() << obj->GetPositionZ() << theOrientation;
	*/
	//UpdateData upd;

	// update movment for others
	//obj->BuildMovementUpdateBlock(&upd,0);
	//upd.BuildPacket( &data );
	//GetSession()->SendPacket( &packet );
	//obj->BuildMovementUpdateBlock(data,0)
	//obj->SendMessageToSet(&data,false);
	sLog.outDebug("facing sent");
	return true;
	//((Creature *)obj)->AI_MoveTo(obj->GetPositionX()+0.1,obj->GetPositionY()+0.1,obj->GetPositionZ()+0.1,theOrientation);
}
/*

bool ChatHandler::HandleAIMoveCommand(const char* args)
{
WorldPacket data;
Object *obj = NULL;

uint64 guid = m_session->GetPlayer()->GetSelection();
if (guid != 0)
{
obj = (Object*)objmgr.GetCreature(guid);
}

if(obj == NULL)
{
FillSystemMessageData(&data, "You should select a creature.");
m_session->SendPacket( &data );
return true;
}

uint8 Value1  = 0;
bool Run  = 0;
uint32 Value2 = 1;
uint32 Value3 = 0;
bool ToFrom = 0;

char* pValue1 = strtok((char*)args, " ");
if (pValue1)
Value1  = static_cast<uint8>(atol(pValue1));

char* pRun = strtok(NULL, " ");
if (pRun)
Run  = atoi(pRun);

char* pValue2 = strtok(NULL, " ");
if (pValue2)
Value2  = atoi(pValue2);

char* pValue3 = strtok(NULL, " ");
if (pValue3)
Value3  = atoi(pValue3);

char* pToFrom = strtok(NULL, " ");
if (pToFrom)
ToFrom  = atoi(pToFrom);

float fromX = ((Creature *)obj)->GetPositionX();
float fromY = ((Creature *)obj)->GetPositionY();
float fromZ = ((Creature *)obj)->GetPositionZ();
float fromO = ((Creature *)obj)->GetOrientation();
float toX = m_session->GetPlayer()->GetPositionX();
float toY = m_session->GetPlayer()->GetPositionY();
float toZ = m_session->GetPlayer()->GetPositionZ();
float toO = m_session->GetPlayer()->GetOrientation();

float distance = ((Creature *)obj)->CalcDistance((Object *)m_session->GetPlayer());
uint32 moveSpeed = 0;
if(!Run)
{
moveSpeed = 2.5f*0.001f;
}
else
{
moveSpeed = 7.0f*0.001f;
}
uint32 moveTime = (uint32) (distance / moveSpeed);

data.Initialize( SMSG_MONSTER_MOVE );
data << guid;
if(ToFrom)
{
data << toX << toY << toZ << toO;
}
else
{
data << fromX << fromY << fromZ << fromO;
}
data << uint8(Value1);
if(Value1 != 1)
{
data << uint32(Run ? 0x00000100 : 0x00000000);
data << moveTime;
data << Value2;
if(ToFrom)
{
data << fromX << fromY << fromZ;
if(Value2 > 1)
{
data << fromO;
}
}
else
{
data << toX << toY << toZ;
if(Value2 > 1)
{
data << toO;
}

}
if(Value2 > 2)
{
data << Value3;
}
}
//((Creature *)obj)->m_m_timeToMove = moveTime;
//m_moveTimer =  UNIT_MOVEMENT_INTERPOLATE_INTERVAL; // update every few msecs

//	m_creatureState = MOVING;
((Creature *)obj)->SendMessageToSet( &data, false );
return true;
}
*/
bool ChatHandler::HandleSetBytesCommand(const char* args, WorldSession *m_session)
{
	Object *obj;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid != 0)
	{
		if((obj = m_session->GetPlayer()->GetMapMgr()->GetUnit(guid)) == 0)
		{
			SystemMessage(m_session, "You should select a character or a creature.");
			return true;
		}
	}
	else
		obj = m_session->GetPlayer();

	char* pBytesIndex = strtok((char*)args, " ");
	if (!pBytesIndex)
		return false;

	uint32 BytesIndex  = atoi(pBytesIndex);

	char* pValue1 = strtok(NULL, " ");
	if (!pValue1)
		return false;

	uint8 Value1  = (uint8)(atol(pValue1));

	char* pValue2 = strtok(NULL, " ");
	if (!pValue2)
		return false;

	uint8 Value2  = (uint8)(atol(pValue2));

	char* pValue3 = strtok(NULL, " ");
	if (!pValue3)
		return false;

	uint8 Value3  = (uint8)(atol(pValue3));

	char* pValue4 = strtok(NULL, " ");
	if (!pValue4)
		return false;

	uint8 Value4  = (uint8)(atol(pValue4));

	std::stringstream sstext;
	sstext << "Set Field " << BytesIndex << " bytes to " << uint16((uint8)Value1) << " " << uint16((uint8)Value2) << " " << uint16((uint8)Value3) << " " << uint16((uint8)Value4) << '\0';
	obj->SetUInt32Value(BytesIndex, ( ( Value1 ) | ( Value2 << 8 ) | ( Value3 << 16 ) | ( Value4 << 24 ) ) );
	SystemMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleGetBytesCommand(const char* args, WorldSession *m_session)
{
	Object *obj;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid != 0)
	{
		if((obj = m_session->GetPlayer()->GetMapMgr()->GetUnit(guid)) == 0)
		{
			SystemMessage(m_session, "You should select a character or a creature.");
			return true;
		}
	}
	else
		obj = m_session->GetPlayer();

	char* pBytesIndex = strtok((char*)args, " ");
	if (!pBytesIndex)
		return false;

	uint32 BytesIndex  = atoi(pBytesIndex);
	uint32 theBytes = obj->GetUInt32Value(BytesIndex);
	/*
	switch (Bytes)
	{
	case 0:
	theBytes = obj->GetUInt32Value(UNIT_FIELD_BYTES_0);
	break;
	case 1:
	theBytes = obj->GetUInt32Value(UNIT_FIELD_BYTES_1);
	break;
	case 2:
	theBytes = obj->GetUInt32Value(UNIT_FIELD_BYTES_2);
	break;
	}
	*/
	std::stringstream sstext;
	sstext << "bytes for Field " << BytesIndex << " are " << uint16((uint8)theBytes & 0xFF) << " " << uint16((uint8)(theBytes >> 8) & 0xFF) << " ";
	sstext << uint16((uint8)(theBytes >> 16) & 0xFF) << " " << uint16((uint8)(theBytes >> 24) & 0xFF) << '\0';

	SystemMessage(m_session, sstext.str().c_str());
	return true;
}
bool ChatHandler::HandleDebugLandWalk(const char* args, WorldSession *m_session)
{
	Player *chr = getSelectedChar(m_session);
	char buf[256];

	if (chr == NULL) // Ignatich: what should NOT happen but just in case...
	{
		SystemMessage(m_session, "No character selected.");
		return false;
	}
	chr->SetMovement(MOVE_LAND_WALK);
	snprintf((char*)buf,256, "Land Walk Test Ran.");
	SystemMessage(m_session, buf);
	return true;
}

bool ChatHandler::HandleDebugWaterWalk(const char* args, WorldSession *m_session)
{
	Player *chr = getSelectedChar(m_session);
	char buf[256];

	if (chr == NULL) // Ignatich: what should NOT happen but just in case...
	{
		SystemMessage(m_session, "No character selected.");
		return false;
	}
	chr->SetMovement(MOVE_WATER_WALK);
	snprintf((char*)buf,256, "Water Walk Test Ran.");
	SystemMessage(m_session,  buf);
	return true;
}

bool ChatHandler::HandleDebugUnroot(const char* args, WorldSession *m_session)
{
	Player *chr = getSelectedChar(m_session);
	char buf[256];

	if (chr == NULL) // Ignatich: what should NOT happen but just in case...
	{
		SystemMessage(m_session,  "No character selected.");
		return false;
	}

	chr->SetMovement(MOVE_UNROOT);

	snprintf((char*)buf,256, "UnRoot Test Ran.");
	SystemMessage(m_session, buf);
	return true;
}

bool ChatHandler::HandleDebugRoot(const char* args, WorldSession *m_session)
{
	Player *chr = getSelectedChar(m_session);
	char buf[256];

	if (chr == NULL) // Ignatich: what should NOT happen but just in case...
	{
		SystemMessage(m_session, "No character selected.");
		return true;
	}
	chr->SetMovement(MOVE_ROOT);
	snprintf((char*)buf,256, "Root Test Ran.");
	SystemMessage(m_session, buf);
	return true;
}

bool ChatHandler::HandleAggroRangeCommand(const char* args, WorldSession *m_session)
{
	Unit *obj = NULL;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid != 0)
	{
		if( (obj = m_session->GetPlayer()->GetMapMgr()->GetUnit(guid)) == NULL )
		{
			SystemMessage(m_session,  "You should select a character or a creature.");
			return true;
		}
	}
	else
	{
		SystemMessage(m_session, "You should select a character or a creature.");
		return true;
	}

	float aggroRange = obj->GetAIInterface()->_CalcAggroRange(m_session->GetPlayer());
	std::stringstream sstext;
	sstext << "Aggrorange is: " << sqrtf(aggroRange) <<'\0';

	SystemMessage(m_session, sstext.str().c_str());

	return true;
}

bool ChatHandler::HandleKnockBackCommand(const char* args, WorldSession *m_session)
{

	float f = args ? (float)atof(args) : 0.0f;
	if(f == 0.0f)
		f = 5.0f;

	Unit *target = m_session->GetPlayer()->GetMapMgr()->GetUnit( m_session->GetPlayer()->GetSelection() );
	if( target == NULL )
	{
		m_session->GetPlayer()->BroadcastMessage("You should select someone to kick");
		return true;
	}
	float dx = sinf(target->GetOrientation());
	float dy = cosf(target->GetOrientation());

	float z = f*0.66f;

	target->SendKnockbackPacket( dy, dx, f, z );

	return true;
}

bool ChatHandler::HandleFadeCommand(const char* args, WorldSession *m_session)
{	
	Unit* target = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!target)
		target = m_session->GetPlayer();
	char* v = strtok((char*)args, " ");
	if(!v)
		return false;

	target->ModThreatModifyer(atoi(v));

	std::stringstream sstext;
	sstext << "threat is now reduced by: " << target->GetThreatModifyer() <<'\0';

	SystemMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleThreatModCommand(const char* args, WorldSession *m_session)
{
	Unit* target = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!target)
		target = m_session->GetPlayer();
	char* v = strtok((char*)args, " ");
	if(!v)
		return false;

	target->ModGeneratedThreatModifyerPCT(0, atoi(v));

	std::stringstream sstext;
	sstext << "new threat caused is now reduced by: " << target->GetGeneratedThreatModifyerPCT(0) << "%" <<'\0';

	SystemMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleCalcThreatCommand(const char* args, WorldSession *m_session)
{
	Unit* target = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!target)
	{
		SystemMessage(m_session, "You should select a creature.");
		return true;
	}
	char* dmg = strtok((char*)args, " ");
	if(!dmg)
		return false;
	char* spellId = strtok(NULL, " ");
	if(!spellId)
		return false;

	uint32 threat = target->GetAIInterface()->_CalcThreat(atol(dmg), dbcSpell.LookupEntry( atoi( spellId ) ), m_session->GetPlayer());

	std::stringstream sstext;
	sstext << "generated threat is: " << threat <<'\0';

	SystemMessage(m_session, sstext.str().c_str());
	return true;
}

bool ChatHandler::HandleThreatListCommand(const char* args, WorldSession *m_session)
{
	Unit* target = NULL;
	target = m_session->GetPlayer()->GetMapMgr()->GetUnit(m_session->GetPlayer()->GetSelection());
	if(!target)
	{
		SystemMessage(m_session, "You should select a creature.");
		return true;
	}

	std::stringstream sstext;
	sstext << "threatlist of creature: " << GUID_LOPART(m_session->GetPlayer()->GetSelection()) << " " << GUID_HIPART(m_session->GetPlayer()->GetSelection()) << '\n';
	TargetMap::iterator itr;
	for(itr = target->GetAIInterface()->GetAITargets()->begin(); itr != target->GetAIInterface()->GetAITargets()->end();)
	{
		Unit *ai_t = target->GetMapMgr()->GetUnit( itr->first );
		if(!ai_t || !itr->second)
		{
			++itr;
			continue;
		}
		sstext << "guid: " << itr->first << " | threat: " << itr->second << "| threat after mod: " << (itr->second + ai_t->GetThreatModifyer()) << "\n";
		++itr;
	}

	SendMultilineMessage(m_session, sstext.str().c_str());
	return true;
}
bool ChatHandler::HandleSendItemPushResult(const char* args, WorldSession* m_session)
{
	uint32 itemid, count, type, unk1, unk2, unk3, unk4;
	char* arg = const_cast<char*>(args);
	itemid = atol(strtok(arg, " "));
	if(!itemid) return false;
	count = atol(strtok(NULL, " "));
	type = atol(strtok(NULL, " "));
	unk1 = atol(strtok(NULL, " "));
	unk2 = atol(strtok(NULL, " "));
	unk3 = atol(strtok(NULL, " "));
	unk4 = atol(strtok(NULL, " "));

	// lookup item
//	ItemPrototype *proto = ItemPrototypeStorage.LookupEntry(itemid);

	WorldPacket data;
	data.SetOpcode(SMSG_ITEM_PUSH_RESULT);

	data << m_session->GetPlayer()->GetGUID();			   // recivee_guid
	data << type << uint32(1);  // unk
	data << count;			  // count
	data << uint8(0xFF);				// uint8 unk const 0xFF
	data << unk1;	   // unk
	data << itemid;
	data << unk2;		  // unk
	data << unk3;		 // random prop
	data << unk4;
	m_session->SendPacket(&data);
	return true;
	//data << ((proto != NULL) ? proto->Quality : uint32(0)); // quality
}

bool ChatHandler::HandleModifyBitCommand(const char* args, WorldSession* m_session)
{

	Object *obj;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid != 0)
	{
		if((obj = m_session->GetPlayer()->GetMapMgr()->GetUnit(guid)) == NULL )
		{
			SystemMessage(m_session, "You should select a character or a creature.");
			return true;
		}
	}
	else
		obj = m_session->GetPlayer();

	char* pField = strtok((char*)args, " ");
	if (!pField)
		return false;

	char* pBit = strtok(NULL, " ");
	if (!pBit)
		return false;

	uint16 field = (uint16)(atoi(pField));
	uint32 bit   = atoi(pBit);

	if (field < 1 || field >= PLAYER_END)
	{
		SystemMessage(m_session, "Incorrect values.");
		return true;
	}

	if (bit < 1 || bit > 32)
	{
		SystemMessage(m_session, "Incorrect values.");
		return true;
	}

	char buf[256];

	if ( obj->HasFlag( field, (1<<(bit-1)) ) )
	{
		obj->RemoveFlag( field, (1<<(bit-1)) );
		snprintf((char*)buf,256, "Removed bit %i in field %i.", (unsigned int)bit, (unsigned int)field);
	}
	else
	{
		obj->SetFlag( field, (1<<(bit-1)) );
		snprintf((char*)buf,256, "Set bit %i in field %i.", (unsigned int)bit, (unsigned int)field);
	}

	SystemMessage(m_session, buf);
	return true;
}

bool ChatHandler::HandleModifyValueCommand(const char* args,  WorldSession* m_session)
{
	Object *obj;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if ( guid != 0 && m_session->GetPlayer()->GetSelectedGo() == NULL )
	{
		if((obj = (Object*)m_session->GetPlayer()->GetMapMgr()->GetUnit(guid)) == NULL )
		{
			SystemMessage(m_session, "You should select a character or a creature.");
			return true;
		}
	}
	else if( m_session->GetPlayer()->GetSelectedGo() != NULL )
	{
		SystemMessage(m_session, "Will change field to GO.");
		obj = m_session->GetPlayer()->GetSelectedGo();
	}
	else
	{
		SystemMessage(m_session, "Will change field to self(player).");
		obj = m_session->GetPlayer();
	}

	char* pField = strtok((char*)args, " ");
	if (!pField)
		return false;

	char* pValue = strtok(NULL, " ");
	if (!pValue)
		return false;

	uint16 field = (uint16)(atoi(pField));
	uint32 value   = atoi(pValue);

	if (field < 1 || field >= PLAYER_END)
	{
		SystemMessage(m_session, "Incorrect Field.");
		return true;
	}
/*
	if (value > sizeof(uint32))
	{
		FillSystemMessageData(&data, m_session, "Incorrect Value.");
		m_session->SendPacket( &data );
		return true;
	}
*/
	char buf[256];
	uint32 oldValue = obj->GetUInt32Value(field);
	obj->SetUInt32Value(field,value);

	snprintf((char*)buf,256,"Set Field %i from %i to %i.", (unsigned int)field, (unsigned int)oldValue, (unsigned int)value);

	if( obj->IsPlayer() )
		SafePlayerCast( obj )->UpdateChances();

	SystemMessage(m_session, buf);

	return true;
}

bool ChatHandler::HandleModifyValueFloatCommand(const char* args,  WorldSession* m_session)
{
	Object *obj;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if ( guid != 0 && m_session->GetPlayer()->GetSelectedGo() == NULL )
	{
		if((obj = (Object*)m_session->GetPlayer()->GetMapMgr()->GetUnit(guid)) == NULL )
		{
			SystemMessage(m_session, "You should select a character or a creature.");
			return true;
		}
	}
	else if( m_session->GetPlayer()->GetSelectedGo() != NULL )
	{
		SystemMessage(m_session, "Will change field to GO.");
		obj = m_session->GetPlayer()->GetSelectedGo();
	}
	else
	{
		SystemMessage(m_session, "Will change field to self(player).");
		obj = m_session->GetPlayer();
	}

	char* pField = strtok((char*)args, " ");
	if (!pField)
		return false;

	char* pValue = strtok(NULL, " ");
	if (!pValue)
		return false;

	uint16 field = (uint16)(atoi(pField));
	float value   = (float)atof(pValue);

	if (field < 1 || field >= PLAYER_END)
	{
		SystemMessage(m_session, "Incorrect Field.");
		return true;
	}
/*
	if (value > sizeof(uint32))
	{
		FillSystemMessageData(&data, m_session, "Incorrect Value.");
		m_session->SendPacket( &data );
		return true;
	}
*/
	char buf[256];
	float oldValue = obj->GetFloatValue(field);
	obj->SetFloatValue(field,value);

	snprintf((char*)buf,256,"Set Field %i from %f to %f.", (unsigned int)field, oldValue, value);

	if( obj->IsPlayer() )
		SafePlayerCast( obj )->UpdateChances();

	SystemMessage(m_session, buf);

	return true;
}

bool ChatHandler::HandleGetValueCommand(const char* args,  WorldSession* m_session)
{
	Object *obj;

	uint64 guid = m_session->GetPlayer()->GetSelection();
	if (guid != 0 && m_session->GetPlayer()->GetSelectedGo() == NULL )
	{
		if((obj = (Object*)m_session->GetPlayer()->GetMapMgr()->GetUnit(guid)) == NULL )
		{
			SystemMessage(m_session, "You should select an object.");
			return true;
		}
	}
	else if( m_session->GetPlayer()->GetSelectedGo() != NULL )
	{
		SystemMessage(m_session, "Will change field to GO.");
		obj = m_session->GetPlayer()->GetSelectedGo();
	}
	else
	{
		SystemMessage(m_session, "Will change field to self(player).");
		obj = m_session->GetPlayer();
	}

	char* pField = strtok((char*)args, " ");
	if (!pField)
		return false;

	uint16 field = (uint16)(atoi(pField));

	if (field < 1 || field >= PLAYER_END)
	{
		SystemMessage(m_session, "Incorrect Field.");
		return true;
	}

	char buf[256];
	uint32 oldValue = obj->GetUInt32Value(field);

	snprintf((char*)buf,256,"Field %i = %d = %u = %f.", (unsigned int)field, *(int32*)&oldValue, *(uint32*)&oldValue, *(float*)&oldValue);

	SystemMessage(m_session, buf);

	return true;
}

bool ChatHandler::HandleRatingsCommand( const char *args , WorldSession *m_session )
{
	char* pField = strtok((char*)args, " ");
	if (!pField)
	{
		SystemMessage(m_session, "usage : .debug setrating <field> <value> (field > max => all)");
		return false;
	}

	char* pValue = strtok(NULL, " ");
	if (!pValue)
	{
		SystemMessage(m_session, "usage : .debug setrating <field> <value> (field > max => all)");
		return false;
	}

	uint16 field = (uint16)(atoi(pField));
	uint32 value   = atoi(pValue);

	Player* m_plyr = getSelectedChar(m_session, false);
	if( m_plyr == NULL || m_plyr->IsPlayer() == false )
		return false;

	if (field > ( PLAYER_RATING_MODIFIER_MASTERY - PLAYER_RATING_MODIFIER_RANGED_SKILL) )
	{
		for( uint32 i = 0; i <= PLAYER_RATING_MODIFIER_MASTERY - PLAYER_RATING_MODIFIER_RANGED_SKILL; i++ )
			m_plyr->SetUInt32Value( PLAYER_FIELD_COMBAT_RATING_1 + i, value );
		m_plyr->BroadcastMessage( "Set all rating fields to %d", value );
	}
	else
	{
		m_plyr->SetUInt32Value( PLAYER_FIELD_COMBAT_RATING_1 + field, value );
		m_plyr->BroadcastMessage( "Set rating field %d to %d", field, value );
	}
	m_plyr->UpdateStats();
	m_plyr->ModAttackSpeed( 0, MOD_MELEE );
	m_plyr->ModAttackSpeed( 0, MOD_RANGED );
	m_plyr->ModAttackSpeed( 0, MOD_SPELL );
	m_plyr->UpdateAttackSpeed();
	m_plyr->CalcDamage();
	m_plyr->UpdateChances();
	return true;
}

bool ChatHandler::HandleDebugDumpCoordsCommmand(const char * args, WorldSession * m_session)
{
	Player* p = m_session->GetPlayer();
	//char buffer[200] = {0};
	FILE * f = fopen("C:\\script_dump.txt", "a");
	if(!f) return true;

	fprintf(f, "mob.CreateWaypoint(%f, %f, %f, %f, 0, 0, 0);\n", p->GetPositionX(), p->GetPositionY(), p->GetPositionZ(),
		p->GetOrientation());
	fclose(f);
	return true;
}

//As requested by WaRxHeAd for database development.
//This should really only be available in special cases and NEVER on real servers... -DGM
bool ChatHandler::HandleSQLQueryCommand(const char* args, WorldSession *m_session)
{
	#ifdef _ONLY_FOOLS_TRY_THIS_
	if(!*args)
	{
		RedSystemMessage(m_session, "No query given.");
		return false;
	}
	
	bool isok = WorldDatabase.Execute(args);

	if(isok)
		GreenSystemMessage(m_session, "Query was executed successfully.");
	else
		RedSystemMessage(m_session, "Query failed to execute.");

	#endif

	return true;
}

//#define _ONLY_FOOLS_TRY_THIS_

static int next_try_id = 0;	//we want to make it easy client side to try out things. This will monitor what we tried out and what we should try out next

uint8 count_bytes( uint8 *guid_bytes )
{
	uint8 ret = 0;
	for( uint32 i=0;i<8;i++)
		if( guid_bytes[i] )
			ret++;
	return ret;
}

uint32 gen_guid_mask(uint32 try_id, uint32 guid_byte_count, uint32 mask_len)
{
	uint32 solutions_found = 0;
	uint32 upper_limit = 1 << mask_len;
	for( uint32 i=0; i<upper_limit; i++)
		if( BitCount8( i ) + BitCount8( i >> 8 ) == guid_byte_count )
		{
			if( solutions_found == try_id )
				return i;
			solutions_found++;
		}
	return 0;
}

uint32 guid_mask_eliminate_bits( uint32 try_id, uint32 old_mask, uint32 mask_len, uint32 bit_count)
{
	uint32 solutions_found = 0;
	uint32 upper_limit = 1 << mask_len;
	for( uint32 i=0; i<upper_limit; i++)
		if( 
			( i & old_mask) == i
			&& 
			BitCount8( i ) + BitCount8( i >> 8 ) == bit_count 
			)
		{
			if( solutions_found == try_id )
				return (old_mask & (~i));
			solutions_found++;
		}
	return 0;
}

static int NextOpcode = 0;
bool ChatHandler::HandleSendpacket(const char * args, WorldSession * m_session)
{
	{
		Player *plr = m_session->GetPlayer();
		WorldPacket data(SMSG_BATTLEGROUND_PLAYER_JOINED, 8);
		uint64 guid = plr->GetGUID();
		uint8 *guid_bytes = (uint8*)&guid;
		data << uint8( 0x4F );
        data << ObfuscateByte( guid_bytes[3] );   
        data << ObfuscateByte( guid_bytes[7] );   
        data << ObfuscateByte( guid_bytes[1] );   
        data << ObfuscateByte( guid_bytes[2] );   
        data << ObfuscateByte( guid_bytes[0] );   
		plr->SendMessageToSet( &data );
	}
	{
		Player *plr = m_session->GetPlayer();
		WorldPacket data( SMSG_ARENA_JOINED, 13 + 10 );
		data << uint8( 0 );
		data << uint32( 1 );
		data << uint64( plr->GetSelection() );
		plr->SendMessageToSet( &data );
	}
	{
		Player *plr = m_session->GetPlayer();
		uint32 Type = BATTLEGROUND_ARENA_2V2;
		uint32 Time = 1000;
		uint32 MapId = plr->GetMapId();
		bool RatedMatch = true;
		sStackWorldPacket( data, SMSG_BATTLEFIELD_STATUS_PLAYER_JOINED, 28 + 10 );
		//mask, if you touch it you break the rest of the packet
		uint8 FirstByte = 0;
		if( IS_ARENA( Type ) )
			FirstByte |= 0xA0;	//extra 2 mask bits = extra 2 bytes
		if( plr->GetTeam() )
			FirstByte |= 0x0A; //Horde 
		else
			FirstByte |= 0x0B; //bit 0 signals team type ? Aliance ? Same as for PVP_LOG_DATA
		data << uint8( FirstByte );
		data << uint8( 0x80 ); 
		data << uint32( 0 );
		data << uint8( 85 );	//maxlevel 85 ? 0x55 ?
		data << uint8( 0x11 );	//this is something obfuscated
		data << uint8( 0x1E );	//this is something obfuscated
		data << uint32( Time ); //milliseconds since started(as created started)
		data << uint32( 0 );	//?
//			data << uint8( ObfuscateByte( Type ) );	//maybe queue position ?
		data << uint8( 7 );	//maybe queue position ? seems to be semi constant
		data << uint8( 0 );		//?
		if( IS_ARENA( Type ) )
			data << uint8( Type );
		data << uint32( MapId );
		if( IS_ARENA( Type ) )
			data << uint8( 2 );	//unk, seen c0 also
		data << uint32( 0 );	//timer until PVP icon near minimap is removed, Given in MS
		data << uint8( RatedMatch );
		if( plr->GetSession() )
			plr->SendMessageToSet(&data);
		return true;
	}
	{
/*
{SERVER} Packet: (0xCB41) SMSG_ROLE_POLL_BEGIN PacketSize = 7 TimeStamp = 10846577
ED 00 00 A5 81 22 0E 

{SERVER} Packet: (0x4321) SMSG_ROLE_CHANGED_INFORM PacketSize = 22 TimeStamp = 10850961
87 FF 0E 00 22 00 81 22 00 00 00 00 
A5 00 0E 08 00 00 00 00 
81 A5 
*/
		Player *plr = m_session->GetPlayer();
		WorldPacket data(SMSG_ROLE_POLL_BEGIN, 20);
		data << uint8( 0 );	//guid mask
		data << plr->GetSelection();
		m_session->SendPacket(&data);
		plr->GetGroup()->SendPacketToAll( &data );
		return true;
	}
	{
		Player *plr = m_session->GetPlayer();
		for(uint32 x=0;x<MAX_NEGATIVE_AURAS1(plr);x++)
		{
			Aura *a = plr->m_auras[x];
			if( a == NULL )
				continue;
			if( a->GetTimeLeft() == (uint32)-1 )
				continue;
			if( a->GetSpellProto()->AttributesExC & CAN_PERSIST_AND_CASTED_WHILE_DEAD )
				continue;
			if( a->IsPassive() )
				continue;
			a->Remove();
		}
		return true;
	}
	{
		Player *p = m_session->GetPlayer();
		uint32 opcodes[] = {0x00341,0x00365,0x00367,0x00721,0x00723,0x00B05,0x00B21,0x00B25,0x00B63,0x01307,0x01721,0x01B45,0x02276,0x022AB,0x02627,0x02666,0x026F6,0x02E6E,0x03227,0x0326F,0x03ABE,0x04303,0x04325,0x04327,0x04341,0x04347,0x04361,0x04367,0x04705,0x04707,0x04725,0x04741,0x04747,0x04765,0x04B45,0x04B63,0x04B67,0x04F01,0x04F61,0x04F67,0x05303,0x05305,0x05321,0x05323,0x05363,0x05367,0x05703,0x05723,0x05741,0x05743,0x05767,0x05B01,0x05B23,0x05B61,0x05F41,0x05F45,0x0623A,0x06636,0x066B3,0x06E6A,0x072E7,0x076BE,0x07AE6,0x07E7E,0x08307,0x08341,0x08743,0x08747,0x08767,0x08B21,0x08C90,0x08F03,0x08F05,0x08F25,0x08F27,0x08F47,0x09307,0x09325,0x09347,0x09363,0x09723,0x09765,0x09767,0x09F23,0x09F43,0x09F61,0x09F63,0x0A233,0x0A26E,0x0A62F,0x0A673,0x0A67F,0x0A6EE,0x0AA62,0x0AA72,0x0AE3F,0x0AEEF,0x0B2B2,0x0B623,0x0B66A,0x0BA63,0x0BA7E,0x0BAEB,0x0BE3B,0x0BEA7,0x0BEAF,0x0BEE2,0x0C307,0x0C321,0x0C347,0x0C705,0x0C707,0x0C743,0x0C767,0x0CB27,0x0CB45,0x0CB47,0x0CB63,0x0CF01,0x0CF41,0x0CF43,0x0CF65,0x0CF67,0x0D303,0x0D305,0x0D327,0x0D343,0x0D361,0x0D701,0x0D703,0x0D727,0x0D743,0x0D745,0x0DB07,0x0DB41,0x0DB65,0x0DF43,0x0DF63,0x0DF65,0x0E67A,0x0E6FF,0x0EA6B,0x0EABB,0x0EAE3,0x0EAFF,0x0EEE6,0x0F67F,0x0F6AB,0x0F6AF,0x0F6BA,0};
		uint32 TryOpcode = opcodes[ NextOpcode ];
		TryOpcode = 0x02E6E;
		if( TryOpcode == 0 )
		{
			sLog.outDebug("out of sugger");
			p->BroadcastMessage("out of sugger");
			return true;
		}
		sStackWorldPacket( data, SMSG_RESYNC_RUNES, 4 + TOTAL_USED_RUNES * 2 + 10);
		data.Initialize( TryOpcode );
		data << uint32( 0xFF );
		data << uint32( TOTAL_USED_RUNES );
		for(uint32 i = 0; i < TOTAL_USED_RUNES; ++i)
		{
			data << uint8( p->m_rune_types[ i ] );
			data << uint8( 128 );
		}
		m_session->SendPacket(&data);
		sLog.outDebug("!!!!!!!!");
		sLog.outDebug("Sending opcode try 0x%04X",opcodes[ NextOpcode ]);
		sLog.outDebug("!!!!!!!!");
		p->BroadcastMessage("Sending opcode try 0x%04X",opcodes[ NextOpcode ]);
		NextOpcode++;
		return true;
	}
	{
		
		Player *p = m_session->GetPlayer();
		if( p->GetFloatValue( PLAYER_RUNE_REGEN_1 + 0 ) > 250.0f )
			p->BroadcastMessage("will be off %f",p->GetFloatValue( PLAYER_RUNE_REGEN_1 + 0 ));
		else
			p->BroadcastMessage("will be on %f",p->GetFloatValue( PLAYER_RUNE_REGEN_1 + 0 ));
		for(uint32 i=0; i<RUNE_TYPE_COUNT;i++)
			if( p->GetFloatValue( PLAYER_RUNE_REGEN_1 + i ) > 250.0f )
				p->SetFloatValue( PLAYER_RUNE_REGEN_1 + i, p->GetFloatValue( PLAYER_RUNE_REGEN_1 + i ) + 250.0f );	
			else
				p->SetFloatValue( PLAYER_RUNE_REGEN_1 + i, p->GetFloatValue( PLAYER_RUNE_REGEN_1 + i ) - 250.0f );	
		return true;
	}
	//crashtest
	{
		Spell *spell = SpellPool.PooledNew( __FILE__, __LINE__ );
		SpellEntry *spellInfo = dbcSpell.LookupEntry( 1 );
		spell->Init( m_session->GetPlayer(), spellInfo ,true, NULL);
		SpellPool.PooledDelete( spell, __FILE__, __LINE__ );
		SpellPool.PooledDelete( spell, __FILE__, __LINE__ );
		return true;
	}
	{
		Unit *u_caster = m_session->GetPlayer();
		Object *m_caster = u_caster;

		sStackWorldPacket( data,SMSG_SPELL_GO,5500 );

		data << m_caster->GetNewGUID() << m_caster->GetNewGUID();

		data << uint8(0);		//cast counter
		data << uint32(6603);	//spell ID
		data << uint32( 0x400 );
		data << getMSTime();
		data << (uint8)(0);		//target count

		data << (uint8)(1);
		data << m_caster->GetGUID();
		data << (uint8)(SPELL_DID_HIT_RESIST);

		data << uint32( 0 ); //targets

		m_session->SendPacket( &data );

		return 0;
	}
	//try to create a buggy A9 packet that would make the client show us invisible or maybe crash. I need to solve this situation because stealthing players are creating this issue
	{
		m_session->GetPlayer()->m_invisFlag |= INVIS_FLAG_JUNGLE;
		m_session->GetPlayer()->UpdateVisibility();
		m_session->GetPlayer()->ProcessPendingUpdates();
		m_session->GetPlayer()->m_invisFlag &= ~INVIS_FLAG_JUNGLE;
		m_session->GetPlayer()->UpdateVisibility();
		m_session->GetPlayer()->ProcessPendingUpdates();
		return 0;
	}
	//try to create a buggy A9 packet that would make the client show us invisible or maybe crash. I need to solve this situation because stealthing players are creating this issue
	{
		MapMgr *mgr = m_session->GetPlayer()->GetMapMgr();
		m_session->GetPlayer()->RemoveFromWorld();
		m_session->GetPlayer()->PushToWorld( mgr );
		m_session->GetPlayer()->RemoveFromWorld();
		m_session->GetPlayer()->PushToWorld( mgr );
		return 0;
	}
	uint32 param = atoi(args);
	WorldPacket data(SMSG_UPDATE_WORLD_STATE, 726);
	if( param == 0 )
	{
		uint8	target_guid[8];	//obfuscated !
		uint64 selectionGUID = m_session->GetPlayer()->GetSelection();
		*(uint64*)target_guid = m_session->GetPlayer()->GetSelection();
		Player *p = m_session->GetPlayer()->GetMapMgr()->GetPlayer( selectionGUID );
		GUID_obfuscate( target_guid );
		sStackWorldPacket( data, SMSG_WARGAMES_INVITE_PARTY_LEADER_OTHER, 16 + 10 );
		data << uint16( 0xF2E3 );	//guid mask
		data << uint8( target_guid[2] );	
		data << uint8( target_guid[7] );	
		data << uint32( 0x0000005A );	//unk = 90
		data << uint8( 0x1E );	//unk 1E
		data << uint8( 0x11 );	//unk 11
		data << uint8( 0x02 );	//unk 02
		data << uint8( target_guid[1] );	
		data << uint8( 0x05 );	//unk
		data << uint8( 0x03 );	//unk
		data << uint8( target_guid[0] );	
		data << uint8( target_guid[3] );	
		p->GetSession()->SendPacket( &data );
		m_session->SendPacket( &data );
	}
	if( param == 1 )
	{
		uint8	target_guid[8];	//obfuscated !
		uint64 selectionGUID = m_session->GetPlayer()->GetSelection();
		*(uint64*)target_guid = m_session->GetPlayer()->GetGUID();
		Player *p = m_session->GetPlayer()->GetMapMgr()->GetPlayer( selectionGUID );
		GUID_obfuscate( target_guid );
		sStackWorldPacket( data, SMSG_WARGAMES_INVITE_PARTY_LEADER_OTHER, 16 + 10 );
		data << uint16( 0xF2E3 );	//guid mask
		data << uint8( target_guid[2] );	
		data << uint8( target_guid[7] );	
		data << uint32( 0x0000005A );	//unk = 90
		data << uint8( 0x1E );	//unk 1E
		data << uint8( 0x11 );	//unk 11
		data << uint8( 0x02 );	//unk 02
		data << uint8( target_guid[1] );	
		data << uint8( 0x05 );	//unk
		data << uint8( 0x03 );	//unk
		data << uint8( target_guid[0] );	
		data << uint8( target_guid[3] );	
		p->GetSession()->SendPacket( &data );
		m_session->SendPacket( &data );
	}
	return true;
/*	{
	uint32 proposed_LOW_guid;
		sStackWorldPacket(data, SMSG_ROLE_CHANGED_INFORM, 50);
		uint64 selectionGUID = m_session->GetPlayer()->GetSelection();
		Player *p = m_session->GetPlayer()->GetMapMgr()->GetPlayer( selectionGUID );
		uint8 *guid_bytes = (uint8 *)&selectionGUID;
		uint8 byte_count = count_bytes( guid_bytes );
		uint16 mask = guid_mask_eliminate_bits( next_try_id, 0x6F87, 16, 2 );

		sLog.outDebug("trying mask 0x%04X for role update try %d",mask,next_try_id );
		data << mask;

		if( guid_bytes[2] )
			data << ObfuscateByte( guid_bytes[2] );
		if( guid_bytes[0] )
			data << ObfuscateByte( guid_bytes[0] );
		if( guid_bytes[3] )
			data << ObfuscateByte( guid_bytes[3] );
		if( guid_bytes[0] )
			data << ObfuscateByte( guid_bytes[0] );

		data << uint32( 0x00000000 );

		if( guid_bytes[1] )
			data << ObfuscateByte( guid_bytes[1] );
		if( guid_bytes[2] )
			data << ObfuscateByte( guid_bytes[2] );

		data << uint32( 0x00000004 );

		if( guid_bytes[3] )
			data << ObfuscateByte( guid_bytes[3] );
		if( guid_bytes[1] )
			data << ObfuscateByte( guid_bytes[1] );

		m_session->SendPacket( &data );
		p->GetSession()->SendPacket( &data );

		uint32 param = atoi(args);
		if( param == 1 )
			next_try_id++;
		else if( param == 2 )
			next_try_id--;
		else if( param == 3 )
			next_try_id = 0;
		else if( param > 10 )
		{
			proposed_LOW_guid = param;
			goto JUMP_HERE_TO_SPAWN_MOB;
		}
		
		return true;
	}/**/
/*	{
		sStackWorldPacket(data, SMSG_WARGAMES_INVITE_PARTY_LEADER_OTHER, 50);
		uint64 selectionGUID = m_session->GetPlayer()->GetSelection();
		Player *p = m_session->GetPlayer()->GetMapMgr()->GetPlayer( selectionGUID );
		uint8 *guid_bytes = (uint8 *)&selectionGUID;
		uint8 byte_count = count_bytes( guid_bytes );
		uint16 mask = gen_guid_mask( next_try_id, byte_count, 16 );

		sLog.outDebug("trying mask 0x%04X for reforge packet( %02X %02X %02X %02X %02X %02X %02X %02X )",mask, guid_bytes[0], guid_bytes[1], guid_bytes[2], guid_bytes[3], guid_bytes[4], guid_bytes[5], guid_bytes[6], guid_bytes[7]);

		GUID_obfuscate( selectionGUID );
		data << mask;
		data << uint8( guid_bytes[2] );	
//		data << uint8( target_guid[7] );	
		data << uint32( 0x0000005A );	//unk = 90
		data << uint8( 0x1E );	//unk 1E
		data << uint8( 0x11 );	//unk 11
		data << uint8( 0x02 );	//unk 02
		data << uint8( guid_bytes[1] );	
		data << uint8( 0x05 );	//unk
		data << uint8( 0x03 );	//unk
		data << uint8( guid_bytes[0] );	
		data << uint8( guid_bytes[3] );	

		p->GetSession()->SendPacket( &data );

		uint32 param = atoi(args);
		if( param == 1 )
			next_try_id++;
		else if( param == 2 )
			next_try_id--;
		else if( param == 3 )
			next_try_id = 0;
//		else if( param > 10 )
//		{
//			proposed_LOW_guid = param;
//			goto JUMP_HERE_TO_SPAWN_MOB;
//		}
		
		return true;
	}/**/
/*	{
		//test to open item reforge window
		sStackWorldPacket(data, SMSG_REFORGING_WINDOW_OPEN, 50);
		uint64 selectionGUID = m_session->GetPlayer()->GetSelection();
		uint8 *guid_bytes = (uint8 *)&selectionGUID;
		uint8 byte_count = count_bytes( guid_bytes );
		uint8 mask = gen_guid_mask( next_try_id, byte_count, 8 );

		data << mask;
		sLog.outDebug("trying mask 0x%02X for reforge packet( %02X %02X %02X %02X %02X %02X %02X %02X )",mask, guid_bytes[0], guid_bytes[1], guid_bytes[2], guid_bytes[3], guid_bytes[4], guid_bytes[5], guid_bytes[6], guid_bytes[7]);
		if( guid_bytes[5] )
			data << ObfuscateByte( guid_bytes[5] );
		if( guid_bytes[4] )
			data << ObfuscateByte( guid_bytes[4] );
		if( guid_bytes[0] )
			data << ObfuscateByte( guid_bytes[0] );
		if( guid_bytes[1] )
			data << ObfuscateByte( guid_bytes[1] );
		if( guid_bytes[7] )
			data << ObfuscateByte( guid_bytes[7] );
		if( guid_bytes[6] )
			data << ObfuscateByte( guid_bytes[6] );

		m_session->SendPacket( &data );

		uint32 param = atoi(args);
		if( param == 1 )
			next_try_id++;
		else if( param == 2 )
			next_try_id--;
		else if( param == 3 )
			next_try_id = 0;
		else if( param > 10 )
		{
			proposed_LOW_guid = param;
			goto JUMP_HERE_TO_SPAWN_MOB;
		}
		
		return true;
	}/**/
	//create mob with X byte low guid
/*	{
JUMP_HERE_TO_SPAWN_MOB:
		uint32 entry=49871;	//Blackrock Battle Worg
		CreatureProto * proto = CreatureProtoStorage.LookupEntry(entry);
		CreatureInfo * info = CreatureNameStorage.LookupEntry(entry);
		float dx = 20.0f * cosf(m_session->GetPlayer()->GetOrientation());
		float dy = 20.0f * sinf(m_session->GetPlayer()->GetOrientation());
		CreatureSpawn * sp = new CreatureSpawn;
		//sp->displayid = info->DisplayID;
		info->GenerateModelId(&sp->displayid);
		sp->entry = entry;
	//	sp->proto = proto;
		sp->form = NULL;
		sp->id = objmgr.GenerateCreatureSpawnID();
		sp->movetype = 0;
		sp->x = m_session->GetPlayer()->GetPositionX() + dx;
		sp->y = m_session->GetPlayer()->GetPositionY() + dy;
		sp->z = m_session->GetPlayer()->GetPositionZ();
		sp->o = m_session->GetPlayer()->GetOrientation();
		sp->emote_state = 0;
		sp->flags = 0;
		sp->factionid = proto->Faction;
		sp->bytes0 = 0;
		sp->bytes1 = 0;
		sp->bytes2 = 0;
		//sp->respawnNpcLink = 0;
		sp->stand_state = 0;
	//	sp->channel_spell=sp->channel_target_creature=sp->channel_target_go=0;
		sp->Item1SlotDisplay = sp->Item2SlotDisplay = sp->Item3SlotDisplay = 0;
		sp->MountedDisplayID = 0;
		sp->difficulty_spawnmask = 65535;
		sp->phase = 2147483647;


		Creature * p ;
		p = m_session->GetPlayer()->GetMapMgr()->CreateCreature(entry);
		p->SetNewGuid( proposed_LOW_guid );
		p->Load(sp, (uint32)NULL, NULL);
		p->PushToWorld(m_session->GetPlayer()->GetMapMgr());
		return true;
	}

/*
	{
		sStackWorldPacket(data,SMSG_MODIFY_COOLDOWN, 50);
        data << uint32(6346);                  // Spell ID
        data << uint64(m_session->GetPlayer()->GetGUID());              // Player GUID
        data << int32(-20000);                   // Cooldown mod in milliseconds
        m_session->SendPacket( &data );
		return true;
	}
	{
		uint32 error = atoi(args);
		sStackWorldPacket(data,SMSG_CAST_FAILED, 1+4+1+4+1+20);
		data << uint8(0);
		data << uint32(10);
		data << uint8(error);
		data << uint8(0);	//so which 0 is first ? no idea
		data << uint32(0);	//maybe this is cooldown remaining
		m_session->SendPacket( &data );
		return true;
	}

	uint32 entry=44214;
	CreatureProto * proto = CreatureProtoStorage.LookupEntry(entry);
	CreatureInfo * info = CreatureNameStorage.LookupEntry(entry);
	float dx = 20.0f * cosf(m_session->GetPlayer()->GetOrientation());
	float dy = 20.0f * sinf(m_session->GetPlayer()->GetOrientation());
	CreatureSpawn * sp = new CreatureSpawn;
	//sp->displayid = info->DisplayID;
	info->GenerateModelId(&sp->displayid);
	sp->entry = entry;
//	sp->proto = proto;
	sp->form = NULL;
	sp->id = objmgr.GenerateCreatureSpawnID();
	sp->movetype = 0;
	sp->x = m_session->GetPlayer()->GetPositionX() + dx;
	sp->y = m_session->GetPlayer()->GetPositionY() + dy;
	sp->z = m_session->GetPlayer()->GetPositionZ();
	sp->o = m_session->GetPlayer()->GetOrientation();
	sp->emote_state = 0;
	sp->flags = 0;
	sp->factionid = proto->Faction;
	sp->bytes0 = 0;
	sp->bytes1 = 0;
	sp->bytes2 = 0;
	//sp->respawnNpcLink = 0;
	sp->stand_state = 0;
//	sp->channel_spell=sp->channel_target_creature=sp->channel_target_go=0;
	sp->Item1SlotDisplay = sp->Item2SlotDisplay = sp->Item3SlotDisplay = 0;
	sp->MountedDisplayID = 0;
	sp->difficulty_spawnmask = 65535;
	sp->phase = 2147483647;


	Creature * p ;
	p = m_session->GetPlayer()->GetMapMgr()->CreateCreature(entry);
	ASSERT(p);
	p->Load(sp, (uint32)NULL, NULL);
	p->PushToWorld(m_session->GetPlayer()->GetMapMgr());
	return true;
	
	WorldPacket data(SMSG_GUILD_EVENT, 400);
	data.Initialize( SMSG_AURA_UPDATE);
	data << m_session->GetPlayer()->GetNewGUID();
	data << uint8( 255 );	//slot, it is fictional !
	data << uint32( 67483 );	//spell id
	data << uint8( VISUALAURA_FLAG_REMOVABLE | VISUALAURA_FLAG_SELF_CAST | VISUALAURA_FLAG_VISIBLE_1 );	//flags, cause i say so
	data << uint8( 85 );	//lvl 80 rules
	data << uint8( 0 );		//stack, ofc it does not stack
	m_session->SendPacket( &data );

	data.Initialize( SMSG_AURA_UPDATE );
	data << m_session->GetPlayer()->GetNewGUID();
	data.CopyFromSnifferDump("0B D9 36 01 00 0B 2C 00");
	m_session->SendPacket( &data );
	return true;

	uint32 counter = atoi(args);
	data.Initialize(SMSG_GUILD_EVENT);
	data << uint8( counter );
	data << uint8( 1 );
	data << "test";
	data << m_session->GetPlayer()->GetGUID();
//	m_session->SendPacket( &data );

	data.Initialize(0xA55E);
	data.CopyFromSnifferDump("00 00 00 00");
	m_session->SendPacket( &data );

	data.Initialize(0x175E);
	data.CopyFromSnifferDump("00 00 00 00");
	m_session->SendPacket( &data );

	data.Initialize(0x255C);
	data.CopyFromSnifferDump("00 00 00 00");
	m_session->SendPacket( &data );

	data.Initialize(0x2878);
	data << m_session->GetPlayer()->GetGUID();
	data.CopyFromSnifferDump("00 00 00 00 02 00 00 00 02 03 04 00 00 00 E0 02 EF 02 02 00 00 00 E0 02 08 00 00 00");
	m_session->SendPacket( &data );

	data.Initialize(0xDD7E);
	data.CopyFromSnifferDump("00 00 00 00");
	m_session->SendPacket( &data );

	data.Initialize(0x3838);
	data.CopyFromSnifferDump("00 00 00 00");
	m_session->SendPacket( &data );

	data.Initialize(0x3C7D);
	data << m_session->GetPlayer()->GetNewGUID();
	data.CopyFromSnifferDump("01 00 00 00 00 00 00 00 00");
	m_session->SendPacket( &data );

	data.Initialize(0x0D5F);
	data.CopyFromSnifferDump("00 00 00 00");
	m_session->SendPacket( &data );

	data.Initialize(0x0C5E);
	data.CopyFromSnifferDump("0E 00 ");
	m_session->SendPacket( &data );

	data.Initialize(0xA775);
	data.CopyFromSnifferDump("0E 00 ");
	m_session->SendPacket( &data );

	data.Initialize(0xD675);
	data.CopyFromSnifferDump("00 00 00 00");
	m_session->SendPacket( &data );

	data.Initialize(0x1C7F);
	data.CopyFromSnifferDump("00 A9 00 00 00 C2 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 C3 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 1C 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 01 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 20 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 97 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 C1 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 B6 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 BE 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 C5 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 88 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 A4 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 AD 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 AE 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 AF 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 B0 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 B1 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 E3 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 CC 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 CD 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 CE 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 CF 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 D0 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 D2 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 D4 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 D5 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 D6 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 D7 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 D8 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 D9 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 DA 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 DB 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 DC 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 AB 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 B2 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 B3 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 B5 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 B7 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 B8 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 BA 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 BB 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 BD 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 BF 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 C0 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 C6 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 C7 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 E2 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 CA 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 CB 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 8A 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 C4 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 A5 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 10 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 ED 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 EF 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 F0 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 F1 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 F2 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 DD 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 D1 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 EE 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 D3 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 C9 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 B9 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 89 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 0A 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 08 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 10 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 AC 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 B4 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 BC 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 12 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 AA 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 A0 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 93 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 A3 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 26 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 02 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 3C 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 3D 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 41 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 43 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 44 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 46 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 47 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 49 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 3E 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 48 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 40 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 3A 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 4A 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 1E 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 22 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 34 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 3F 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 01 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 F5 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 F6 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 E0 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 DF 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 F7 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 F8 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 FA 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 28 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 35 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 28 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 2C 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 2D 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 2E 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 2F 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 30 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 31 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 32 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 33 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 36 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 29 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 45 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 F9 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 14 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 1D 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 02 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 03 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 04 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 0C 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 26 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 39 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 04 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 0E 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 14 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 30 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 16 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 18 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 1A 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 24 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 A1 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 92 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 94 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 95 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 96 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 8C 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 E1 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 11 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 F3 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 F4 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 FB 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 FC 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 FD 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 FE 00 00 05 02 00 00 00 00 00 00 00 00 00 00 00 FF 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 00 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 2E 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 9F 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 05 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 06 01 00 06 02 00 00 00 00 00 00 00 00 00 00 00 32 00 00 02 02 00 00 00 00 00 00 00 00 00 00 00 12 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 17 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 18 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 37 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 25 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 06 00 00 01 02 00 00 00 00 00 00 00 00 00 00 00 42 01 00 05 02 00 00 00 00 00 00 00 00 00 00 00 38 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 1E 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 1F 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 20 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 2A 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 2B 01 00 01 02 00 00 00 00 00 00 00 00 00 00 00 3B 01 00 02 02 00 00 00 00 00 00 00 00 00 00 00 ");
	m_session->SendPacket( &data );

	data.Initialize(0x3898);
	data.CopyFromSnifferDump("00 00 00 00");
	m_session->SendPacket( &data );

	data.Initialize(0x0818);
	data.CopyFromSnifferDump("00 00 00 00");
	m_session->SendPacket( &data );

	data.Initialize(0x3840);
	data.CopyFromSnifferDump("00 01 00 00 00 5E 00 00 00 ");
	m_session->SendPacket( &data );

	data.Initialize(0x257F);
	data.CopyFromSnifferDump("93 64 D7 42 FF 4D 1C 3D CE 62 48 C3 B0 20 29 73 D4 3B C7 75 75 CA 18 19 79 64 47 1E 57 9E C2 9F 86 16 48 12 8F 34 2B 3A 8F F6 6B 13 A7 5F C8 61 0E DC 94 64 99 03 D3 DA F9 48 11 31 BE DE 2B 80 0E 26 3B C5 8A 35 B1 41 C0 2D B2 FA 82 B5 9C 27 18 5A 2B F4 BC B4 F0 48 3B A3 57 4D 05 29 8F 53 77 C9 11 CD 3E A4 4C 32 2F E3 6D 1B B6 87 E4 45 C7 D8 EB 3E E9 39 70 7B 84 59 8E 28 55 70 CE BF 30 96 1F 08 81 3D 81 3E D6 48 6B AF D5 76 85 F4 8F 34 1A 9A 28 46 DE 65 1C C5 7F B7 69 A7 9C C2 43 83 F5 2A 2C D7 ");
	m_session->SendPacket( &data ); */

#ifdef _ONLY_FOOLS_TRY_THIS_

    uint32 arg_len = strlen(args);
    char * xstring = new char [arg_len];
    memcpy(xstring, args,arg_len);

    for (uint32 i = 0; i < arg_len; i++)
    {
        if (xstring[i] == ' ')
        {
            xstring[i] = '\0';
        }
    }

    // we receive our packet as hex, that means we get it like ff ff ff ff
    // the opcode consists of 2 bytes

    if (!xstring)
    {
        sLog.outDebug("[Debug][Sendpacket] Packet is invalid");
        return false;
    }

    WorldPacket data(arg_len);

    uint32 loop = 0;
    uint16 opcodex = 0;
    uint16 opcodez = 0;

    // get the opcode
    sscanf(xstring , "%x", &opcodex);

    // should be around here
    sscanf(&xstring[3] , "%x", &opcodez);

    opcodex =  opcodex << 8;
    opcodex |= opcodez;
    data.Initialize(opcodex);

    
    int j = 3;
    int x = 0;
    do 
    {
        if (xstring[j] == '\0')
        {
            uint32 HexValue;
            sscanf(&xstring[j+1], "%x", &HexValue);
            if (HexValue > 0xFF)
            {
                sLog.outDebug("[Debug][Sendpacket] Packet is invalid");
                return false;
            }
            data << uint8(HexValue);
            //j++;
        }
        j++;
    } while(j < arg_len);

    data.hexlike();

    m_session->SendPacket(&data);

    sLog.outDebug("[Debug][Sendpacket] Packet was send");
#endif
    return true;
}
