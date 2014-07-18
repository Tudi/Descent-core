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

#ifndef WOWSERVER_AIINTERFACE_H
#define WOWSERVER_AIINTERFACE_H

/* platforms that already define M_PI in math.h */
#ifdef M_PI
#undef M_PI
#endif

//#define M_PI	   3.14159265358979323846
#define M_PI	   3.14159f
#define UNIT_MOVEMENT_INTERPOLATE_INTERVAL 400/*750*/ // ms smoother server/client side moving vs less cpu/ less b/w
#define TARGET_UPDATE_INTERVAL_ON_PLAYER 2000 // we most likely will have to kill players and only then check mobs
#define TARGET_UPDATE_INTERVAL 6000 // this is a multiple of PLAYER_TARGET_UPDATE_INTERVAL
#define oocr 50.0f // out of combat range
#define PLAYER_SIZE 1.5f

#define ENABLE_CREATURE_DAZE
#ifdef ENABLE_CREATURE_DAZE
	#define CREATURE_SPELL_TO_DAZE 1604
	#define CREATURE_DAZE_TRIGGER_ANGLE M_PI/2 //for the beginners this means 45 degrees 
#endif

#define DISTANCE_TO_SMALL_TO_WALK 1.0f //this is required so creature will not try to reposition itself to obtain perfect combat range. Not using this might lead to exploits
//#define USE_SPELL_CAST_TO_DEBUG_CREATURE_MOVE_SPAM 588

//!!! it is in seconds and not Milliseconds
#define MOB_SPELLCAST_GLOBAL_COOLDOWN			2000 //there are individual cooldown and global ones. Global cooldown stops mob from casting 1 instant speels on you per second
#define MOB_SPELLCAST_REFRESH_COOLDOWN_INTERVAL	2000

//#define INHERIT_FOLLOWED_UNIT_SPEED 1

#define HACKY_SERVER_CLIENT_POS_SYNC

#define CALL_FOR_FRIEND_COOLDOWN 3000 //x second cooldown to not chain help

#define IS_IN_INSTANCE( AII ) ( AII->m_Unit->m_mapMgr && IS_MAP_INSTANCE( AII->m_Unit->m_mapMgr->GetMapInfo() ) )

#define THREATH_CLIENT_UPDATE_INTERVAL			1000	//server updates every 50 ms, no need to spam the shit out of the client

#define AI_INTERFACE_DB_FLAG_MOVE_FLY			512		//note that this can get depracated from blizz value
#define AI_INTERFACE_DB_FLAG_MOVE_RUN			256		//note that this can get depracated from blizz value

#define AI_REACTION_CLIENT_AUTO_ALWAYS_FACE_TARGET			2

class Object;
class Creature;
class Unit;
class Player;
class WorldSession;
class SpellCastTargets;
struct TotemSpells;

enum MonsterMoveFlags	// for AIInterface::SendMoveToPacket
{
	MONSTER_MOVE_FLAG_WALK			= 0x0,
//	MONSTER_MOVE_FLAG_TELEPORT		= 0x100,	//crashes client in 420 unless we update everywhere packet struct
	MONSTER_MOVE_FLAG_ARC_JUMP		= 0x00000100,	//removed value until i find them to not crash the client
//	MONSTER_MOVE_FLAG_RUN			= 0x00001000,
	MONSTER_MOVE_FLAG_RUN			= 0x00000000,	//removed value until i find them to not crash the client
	MONSTER_MOVE_FLAG_DEATHGRIP		= 0x00000300,	//this is supposed to be arc jump
	MONSTER_MOVE_FLAG_HEROIC_LEAP	= 0x00000300,	//this is supposed to be arc jump
	MONSTER_MOVE_FLAG_FLY			= 0x04000400,	
	MONSTER_MOVE_FLAG_TAXI_FLY_420	= 0x0C008400,	//need to figure out sub flags, right now it is a hackfix
    Forward							= 0x00000001,
    Backward						= 0x00000002,
    StrafeLeft						= 0x00000004,
    StrafeRight						= 0x00000008,
    TurnLeft						= 0x00000010,
    TurnRight						= 0x00000020,
    Falling							= 0x00000040,	//used with base movement flags
    NoSpline						= 0x00000080,	//used with base movement flags
    Parabolic						= 0x00000100,
    WalkMode						= 0x00000200,	// only combination i found is 0x14000300
    Flying							= 0x00000400,	// only combo 0x04000400
    RotationFixed					= 0x00000800,	
    FinalPoint						= 0x00001000,	
    FinalTarget						= 0x00002000,	
    FinalOrient						= 0x00004000,
    CatMullRom						= 0x00008000,
    Cyclic							= 0x00010000,
    EnterCycle						= 0x00020000,
    Animation						= 0x00040000,
    Unk4							= 0x00080000,
    Unk5							= 0x00100000,
    Unk6							= 0x00200000,
    Unk7							= 0x00400000,
    Unk8							= 0x00800000,
    InvertOrient					= 0x01000000,
    SmoothPathing					= 0x02000000, //seen it paired with 0x04000000 or 0x00000010
    Unk11							= 0x04000000, //seen it single or paired with 0x00000400 or with 0x02000000
/*	Forward							= 0x00000001,
	Backward						= 0x00000002,
	StrafeLeft						= 0x00000004,
	StrafeRight						= 0x00000008,
	TurnLeft						= 0x00000010,
	TurnRight						= 0x00000020,
	PitchUp							= 0x00000040,
	PitchDown						= 0x00000080,
	WalkMode						= 0x00000100,
	GravityDisabled					= 0x00000200,
	Root							= 0x00000400,
	Falling							= 0x00000800,
	FallingFar						= 0x00001000,
	PendingStop						= 0x00002000,
	PendingStrafeStop				= 0x00004000,
	PendingForward					= 0x00008000,
	PendingBackward					= 0x00010000,
	PendingStrafeLeft				= 0x00020000,
	PendingStrafeRight				= 0x00040000,
	PendingRoot						= 0x00080000,
	Swimming						= 0x00100000,
	Ascending						= 0x00200000,
	Descending						= 0x00400000,
	CanFly							= 0x00800000,
	Flying							= 0x01000000,
	//unknown						= 0x02000000,
	WaterWalking					= 0x04000000,
	SafeFall						= 0x08000000,
	Hover							= 0x10000000,
	CollisionDisabled				= 0x20000000,
	OnTransport						= 0x40000000, // wtf? */
	Unk12							= 0x08000000, // related monster move vectors
    Unk13							= 0x10000000,
};

// Some undocumentated monster move flags
enum MonsterMoveyFlagys
{
	MONSTER_MOVE_NONE					= 0x00000000,
	MONSTER_MOVE_RUN					= 0x00001000,
	MONSTER_MOVE_FLIGHT					= 0x00003000,
	MONSTER_MOVE_TELEPORT_VEHICLE		= 0x00800000,
 
	MONSTER_MOVE_FORWARD				= 0x00000001,
	MONSTER_MOVE_BACKWARD				= 0x00000002,
	MONSTER_MOVE_STRAFE_LEFT			= 0x00000004,
	MONSTER_MOVE_STRAFE_RIGHT			= 0x00000008,
	MONSTER_MOVE_LEFT					= 0x00000010, // turn
	MONSTER_MOVE_RIGHT					= 0x00000020, // turn
	MONSTER_MOVE_PITCH_UP				= 0x00000040,
	MONSTER_MOVE_PITCH_DOWN				= 0x00000080,
	MONSTER_MOVE_TELEPORT				= 0x00000100,
	MONSTER_MOVE_TELEPORT2				= 0x00000200,
	MONSTER_MOVE_LEVITATING				= 0x00000400, // swimming ?
	MONSTER_MOVE_UNK1					= 0x00000800, // float+uint32
	MONSTER_MOVE_SPLINE					= 0x00002000, // spline n*(float x,y,z)
	// 0x4000, 0x8000, 0x10000, 0x20000 run
	MONSTER_MOVE_SPLINE2				= 0x00040000, // spline n*(float x,y,z)
	MONSTER_MOVE_UNK2					= 0x00080000, // used for flying mobs
	MONSTER_MOVE_UNK3					= 0x00100000, // used for flying mobs
	MONSTER_MOVE_UNK4					= 0x00200000, // uint8+uint32
	MONSTER_MOVE_UNK5					= 0x00400000, // run in place, then teleport to final point
	MONSTER_MOVE_UNK6					= 0x00800000, // teleport
	MONSTER_MOVE_FLY					= 0x02000000, // swimming/flying (depends on mob?) - only seen it with 0x04000000 combined
	MONSTER_MOVE_UNK9					= 0x04000000, // run
	MONSTER_MOVE_UNK10					= 0x08000000, // run
	MONSTER_MOVE_UNK11					= 0x10000000, // run
	MONSTER_MOVE_UNK12					= 0x20000000, // run
	MONSTER_MOVE_UNK13					= 0x40000000, // levitating
};

enum MonsterMoveType
{
    MonsterMoveNormal       = 0,
    MonsterMoveStop         = 1,
    MonsterMoveFacingSpot   = 2,
    MonsterMoveFacingTarget = 3,
    MonsterMoveFacingAngle  = 4
};

enum AIType
{
	AITYPE_LONER,
	AITYPE_AGRO,
	AITYPE_SOCIAL,
	AITYPE_PET,
	AITYPE_TOTEM,
	AITYPE_GUARDIAN, //we got a master but he cannot control us, we follow and battle oposite factions
};

enum MovementType
{
	MOVEMENTTYPE_NONE,
	MOVEMENTTYPE_RANDOMWP,
	MOVEMENTTYPE_CIRCLEWP,
	MOVEMENTTYPE_WANTEDWP,
	MOVEMENTTYPE_DONTMOVEWP,
	MOVEMENTTYPE_QUEST = 10,
	MOVEMENTTYPE_FORWARDTHANSTOP = 11,
	MOVEMENTTYPE_DONT_USE_WP,
};

enum AI_Agent
{
	AGENT_NULL,
	AGENT_MELEE,
	AGENT_RANGED,
	AGENT_FLEE,
	AGENT_SPELL,
	AGENT_CALLFORHELP
};

//don't forget to add types at the end of the list instead middle !
enum AI_SpellTargetType
{
	TTYPE_NULL,
	TTYPE_SINGLETARGET,
	TTYPE_DESTINATION,
	TTYPE_SOURCE,
	TTYPE_CASTER,
	TTYPE_OWNER,
	TTYPE_SECOND_MOST_HATED,
	TTYPE_HEALER,
	TTYPE_TANK,
	TTYPE_DPS,
	TTYPE_AT_RANGE,
	TTYPE_FRIENDLY_FOR_BUFF,
	TTYPE_FRIENDLY_TO_HEAL,
	TTYPE_AT_RANGE_PLAYER,
	TTYPE_AT_RANGE_PLAYER_RANDOM,
	TTYPE_ENEMY_FOR_BUFF,
};

enum AI_State
{
	STATE_IDLE,
	STATE_ATTACKING,
	STATE_CASTING,
	STATE_FLEEING,
	STATE_FOLLOWING,
	STATE_EVADE,
	STATE_MOVEWP,
	STATE_FEAR,
	STATE_WANDER,
	STATE_STOPPED,
	STATE_SCRIPTMOVE,
	STATE_SCRIPTIDLE
};

enum MovementState
{
	MOVEMENTSTATE_MOVE,
	MOVEMENTSTATE_FOLLOW,
	MOVEMENTSTATE_STOP,
	MOVEMENTSTATE_FOLLOW_OWNER
};

enum CreatureState
{
	STOPPED,
	MOVING,
	WAITING,
	ATTACKING
};

enum AiEvents
{
	EVENT_ENTERCOMBAT,
	EVENT_LEAVECOMBAT,
	EVENT_DAMAGEDEALT,
	EVENT_DAMAGETAKEN,
	EVENT_FEAR,
	EVENT_UNFEAR,
	EVENT_FOLLOWOWNER,
	EVENT_WANDER,
	EVENT_UNWANDER,
	EVENT_UNITDIED,
	EVENT_CHAINGENTERCOMBAT,
};

struct SpellEntry;
//enum MOD_TYPES;
enum AutoCastEvents;

class AI_Spell
{
public:
	AI_Spell()		
	{ cooldowntime = 0; initial_cooldown = 0; global_cooldown = 0; shout_before_cast = NULL; }
	uint32			entryId;
	uint32			procChance;
	SpellEntry *	spell;
	uint16			agent;
	uint8			spelltargetType;
	uint16			difficulty_mask;	// only cast it in specific map difficulty modes
	uint32			cooldown;		
	uint32			initial_cooldown;	// in case this spell has not been casted then first time it would be selected it gets this delay. Ex : enrage
	uint32			global_cooldown;	// this is mostly 2 sec cooldown after any spell cast
	uint32			cooldowntime;		// this is local time when spell will be available again
	uint32			procCount;
	uint32			procCounter;
	uint32			min_hp_pct;
	uint32			max_hp_pct;
	float			minrange;
	float			maxrange;
	AutoCastEvents	autocast_type;
	char			*shout_before_cast;	// warning this is only pointer copy and not memory copy 
};

bool isGuard(uint32 id);
bool isNeutralGuard(uint32 id);
uint32 getGuardId(uint32 id);

typedef HM_NAMESPACE::hash_map<uint64, int32> TargetMap;

typedef std::set<uint64> AssistTargetSet;
typedef std::map<uint32, AI_Spell*> SpellMap;

class SERVER_DECL AIInterface
{
public:

	AIInterface();
	virtual ~AIInterface();

	// Misc
	void Init(Unit *un, AIType at, MovementType mt);
	void Init(Unit *un, AIType at, MovementType mt, Unit *owner); // used for pets
	Unit *GetUnit() { return m_Unit; }
	Unit *GetPetOwner();
	ARCEMU_INLINE void SetPetOwner(Unit * owner);
	void DismissPet();
	ARCEMU_INLINE void SetUnitToFollow(Unit* un) 
	{ 
		if( un )
			UnitToFollowGUID = un->GetGUID(); 
		else
			UnitToFollowGUID = 0;
	};
	void SetUnitToFear(Unit* un)  { UnitToFear = un; };
//	void SetFollowDistance(float dist) { FollowDistance = dist; };
	void SetUnitToFollowAngle(float angle) { m_followAngle = angle; }
	bool setInFront(Unit* target);
	Unit* getUnitToFollow();
	void setCreatureState(CreatureState state){ m_creatureState = state; }
	ARCEMU_INLINE uint8 getAIState() { return m_AIState; }
	ARCEMU_INLINE uint8 getAIType() { return m_AIType; }
	ARCEMU_INLINE uint8 getCurrentAgent() { return m_aiCurrentAgent; }
	void setCurrentAgent(AI_Agent agent) { m_aiCurrentAgent = agent; }
	uint32	getThreatByGUID(uint64 guid);
	uint32	getThreatByPtr(Unit* obj);
	Unit	*GetMostHated();
	Unit	*GetSecondHated();
//	bool	modThreatByGUID(uint64 guid, int32 mod);
	bool	modThreatByPtr(Unit* obj, int32 mod);
	void	RemoveThreatByPtr(Unit* obj);
//	ARCEMU_INLINE AssistTargetSet GetAssistTargets() { return m_assistTargets; }
//	void addAssistTargets(Unit* Friends);
	ARCEMU_INLINE void LockAITargets(bool lock) 
	{ 
		lock? m_aiTargetsLock.Acquire(): m_aiTargetsLock.Release(); 
	};
	ARCEMU_INLINE TargetMap *GetAITargets() { return &m_aiTargets; }
	void ClearHateList();
	void WipeHateList();
	void WipeTargetList();
	bool taunt(Unit* caster, bool apply = true);
	Unit* getTauntedBy();
	bool GetIsTaunted();

	ARCEMU_INLINE size_t getAITargetsCount() { return m_aiTargets.size(); }
	ARCEMU_INLINE uint32 getOutOfCombatRange() { return m_outOfCombatRange; }
	void setOutOfCombatRange(uint32 val) { m_outOfCombatRange = val; }

	// Spell
	void CastSpell(Unit* caster, SpellEntry *spellInfo, SpellCastTargets targets);
	SpellEntry *getSpellEntry(uint32 spellId);
	AI_Spell *getSpell();
	void addSpellToList(AI_Spell *sp);
	//don't use this until i finish it !!
	void CancelSpellCast();

	// Event Handler
	void HandleEvent(uint32 event, Unit* pUnit, uint32 misc1);
	void OnDeath(Object* pKiller);
	void AttackReaction(Unit *pUnit, uint32 damage_dealt, uint32 spellId = 0);
	void HealReaction(Unit* caster, Unit* victim, SpellEntry* sp, uint32 amount);
//	void Event_Summon_EE_totem(uint32 summon_duration);
//	void Event_Summon_FE_totem(uint32 summon_duration);
	void EventAiInterfaceParamsetFinish();
	void EventChangeFaction( Unit *ForceAttackersToHateThisInstead=NULL );	//we have to tell our current enemies to stop atacking us, we should also forget about our targets

	// Update
	virtual void Update(uint32 p_time);

	// Movement
	void SendMoveToPacket(float toX, float toY, float toZ, float toO, uint32 time, uint32 MoveFlags, float Zspeed=0.0f, WorldSession *Session=NULL);
	//void SendMoveToSplinesPacket(std::list<Waypoint> wp, bool run);
	void MoveTo(float x, float y, float z, float o);
	uint32 getMoveFlags();
	void UpdateMove();
	void SendCurrentMove(Player* plyr/*uint64 guid*/);
	bool StopMovement(uint32 time);
	uint32 getCurrentWaypoint() { return m_currentWaypoint; }
	void changeWayPointID(uint32 oldwpid, uint32 newwpid);
	bool addWayPoint(WayPoint* wp);
	bool saveWayPoints();
	bool showWayPoints(Player* pPlayer, bool Backwards);
	bool hideWayPoints(Player* pPlayer);
	WayPoint* getWayPoint(uint32 wpid);
	void deleteWayPoint(uint32 wpid);
	void deleteWaypoints();
	ARCEMU_INLINE bool hasWaypoints() { return m_waypoints!=NULL; }
	ARCEMU_INLINE void setMoveType(uint32 movetype) { m_moveType = movetype; }
	ARCEMU_INLINE uint32 getMoveType() { return m_moveType; }
	ARCEMU_INLINE void setMoveRunFlag(bool f) { m_moveRun = f; }
	ARCEMU_INLINE bool getMoveRunFlag() { return m_moveRun; }
	void setWaypointToMove(uint32 id) { m_currentWaypoint = id; }
	bool IsFlying();

	// Calculation
	float _CalcAggroRange(Unit* target);
	void _CalcDestinationAndMove(Unit *target, float dist);
	float _CalcCombatRange(Unit* target, bool ranged);
	float _CalcDistanceFromHome();
	uint32 _CalcThreat(uint32 damage, SpellEntry * sp, Unit* Attacker);
	
	void SetAllowedToEnterCombat(bool val) { disable_combat = !val; }
	ARCEMU_INLINE bool GetAllowedToEnterCombat(void) { return !disable_combat; }

	void				EventRemoveInrangeUnit(Unit* target);
	ARCEMU_INLINE void SetAIState(AI_State newstate) { m_AIState = newstate; }

	// Movement
	uint32	m_currentWaypoint;
	uint32	m_moveType;
	bool	m_canMove;
	bool	m_WayPointsShowing;
	bool	m_WayPointsShowBackwards;
	bool	m_moveBackward;
	bool	m_moveRun;
	bool	m_moveFly;
	bool	m_moveSprint;
	bool	m_canFlee;
	bool	m_canCallForHelp;
	bool	m_canRangedAttack;
	bool	disable_combat;
	bool	disable_melee;
	bool	disable_ranged;
	bool	disable_spell;
	bool	disable_targeting;
	CreatureState m_creatureState;

	size_t GetWayPointsCount()
	{
		if( m_waypoints )
			return m_waypoints->GetMaxSize();	/* ignore 0 */
		else
			return 0;
	}
	float			m_FleeHealth;
	uint32			m_FleeDuration;
	float			m_CallForHelpHealth;
	uint32			m_totemspelltime;
	TotemSpells		*totemspell;

	ARCEMU_INLINE void AddStopTime(uint32 Time) { m_TimeRemainingReachDestination += Time; }
	ARCEMU_INLINE void SetNextSpell(AI_Spell * sp) { m_nextSpell = sp; }
	Unit* GetNextTarget();
	void SetNextTarget (Unit *nextTarget);
	void SetNextTarget (uint64 nextTarget); 

	Creature	*m_formationLinkTarget;
	float		m_formationFollowDistance;
	float		m_formationFollowAngle;
	uint32		m_formationLinkSqlId;
	float		agro_range_mod;	//not sure this is worth keeping
	uint64		m_forced_target;	//when we set a pet to attack some specific target he should not retarget

	void WipeReferences();
	WayPointVect		*m_waypoints;
	WayPointVect		*m_custom_waypoint_map;
	TimedEmoteList		*timed_emotes;
 
	SimplePointerList<AI_Spell> m_spells;

	uint32 spell_global_cooldown_stamp;

	void CheckNextSpell(AI_Spell * sp)
	{
		if(m_nextSpell == sp)
			m_nextSpell = 0;
	}

	void ResetProcCounts();

	ARCEMU_INLINE void SetWaypointMap(WayPointVect * m) { m_waypoints = m; }
	void SetWaypointMap(WayPointMap * m); //stupid compatibility with scripts :(
	bool m_isGuard;
	bool m_isNeutralGuard;
//	bool m_fastMove;
	void setGuardTimer(uint32 timer) { m_guardTimer = timer; }
	virtual void _UpdateCombat(uint32 p_time);

	Unit* FindTarget();
	Unit* FindTargetForSpellTargetType(AI_Spell *sp);
	bool FindFriends(float dist,bool urgent);
	void SendAIReaction( uint64 Target=0, uint32 ReactionType=AI_REACTION_CLIENT_AUTO_ALWAYS_FACE_TARGET, WorldSession *Session=NULL );
protected:
	bool	UnsafeCanOwnerAttackUnit(Unit *pUnit);		//this is designed for internal use only

	// Update
	void _UpdateTargets();
	void _UpdateMovement(uint32 p_time);
	void _UpdateTimer(uint32 p_time);
	void _UpdateThreathList();
	void ClearThreathList();
//	bool m_updateAssist;
//	bool m_updateTargets;
//	uint32 m_updateAssistTimer;
	uint32 m_updateTargetsTimer;
	uint32 m_updateTargetsTimer2;

	// Misc
	AI_Spell	*m_nextSpell;
	uint64		m_nextTarget;
	uint32		m_fleeTimer;
	bool		m_hasFleed;
	bool		m_hasCalledForHelp;
	uint32		m_outOfCombatRange;

//	Unit *gracefull_hit_on_target;
	Unit		*m_Unit;
	uint64		m_PetOwnerGUID;

	//std::set<AI_Target> m_aiTargets;
	Mutex		m_aiTargetsLock;
	TargetMap	m_aiTargets;
//	AssistTargetSet m_assistTargets;
	AIType		m_AIType;
	AI_State	m_AIState;
	AI_Agent	m_aiCurrentAgent;

	Unit		*tauntedBy; //This mob will hit only tauntedBy mob.
#ifdef HACKY_SERVER_CLIENT_POS_SYNC
	bool		moved_for_attack;
#endif

	// Movement
	float	m_destinationX;
	float	m_destinationY;
	float	m_destinationZ;
	uint64	m_UnitToMoveTo;

	float m_wanderStartX;
	float m_wanderStartY;
	//typedef std::map<uint32, WayPoint*> WayPointMap;
	uint64		UnitToFollowGUID;
//	uint64		UnitToFollow_backup;//used unly when forcing creature to wander (blind spell) so when effect wears off we can follow our master again (guardian)
//	float		FollowDistance_backup;
#define FollowDistance 3.0f
//	float		FollowDistance;
	float		m_followAngle;
//	float		m_lastFollowX;
//	float		m_lastFollowY;
	Unit		*UnitToFear;
	uint32		m_TimeRemainingReachDestination;
	uint32		m_LastMoveUpdateStamp;

	MovementType	m_MovementType;
	MovementState	m_MovementState;
	uint32			m_guardTimer;
	int32			m_currentHighestThreat;
	std::list<spawn_timed_emotes*>::iterator	next_timed_emote;
	uint32										timed_emote_expire;
	uint32	call_friend_cooldown; //do not spam call for help ir chain call help
	uint32	entered_combat_stamp;	//some spells like enrage and berserk require this 
public:
	bool skip_reset_hp;
	int32	DamageDoneLastMeleeStrike;

	void WipeCurrentTarget();
	bool CheckCurrentTarget();
//	bool TargetUpdateCheck(Unit * ptr);
	void HandleChainAggro(Unit* u);
};
#endif
