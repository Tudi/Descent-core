//#define USE_OLD_EVENT_MANAGER
#ifdef USE_OLD_EVENT_MANAGER
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

#ifndef _EVENTABLE_OBJECT_H
#define _EVENTABLE_OBJECT_H

class EventableObjectHolder;

// Zack : the story 
// each object inherits EventableObject
// we add events to object using EventableObject so when destroying base object we can delete it's events also
// EventableObject is linked to EventableObjectHolder ( events are added to Holder's list also)
// EventableObjectHolder is updated with each map update cycle thus object event callback functions are called
// EventableObjectHolder is always updated while map is not destroyed
// each time an event is added to a list ref is incremented. Each time an event is removed from a list. ref is decremented. At the moment the event is not anymore in any list it gets deleted
// EventableObject list is kept until object is destroyed. I wonder how large it will grow in time
// Question : how come events get deleted more then they are added ? Decref called more then incref

/**
  * @class EventableObject
  * EventableObject means that the class inheriting this is able to take
  * events. This 'base' class will store and update these events upon
  * receiving the call from the instance thread / WorldRunnable thread.
  */

typedef list<TimedEvent*> EventList;
typedef multimap<uint32, TimedEvent*> EventMap;

#define EVENT_REMOVAL_FLAG_ALL 0xFFFFFFFF
#define WORLD_INSTANCE -1

class SERVER_DECL EventableObject
{
	friend class EventMgr;
	friend class EventableObjectHolder;

protected:
	void event_RemoveEvents();
	void event_RemoveEvents(uint32 EventType);
	void event_ModifyTimeLeft(uint32 EventType, uint32 TimeLeft,bool unconditioned=false);
	void event_ModifyTime(uint32 EventType, uint32 Time);
	void event_ModifyRepeatInterval(uint32 EventType, uint32 Time);
	bool event_HasEvent(uint32 EventType);
	void event_DelayTrigger(uint32 EventType, uint32 Time);
//	void event_RemoveByPointer(TimedEvent * ev);
//	ARCEMU_INLINE int32 event_GetCurrentInstanceId() { return m_event_Instanceid; }
	bool event_GetTimeLeft(uint32 EventType, uint32 * Time);

public:
	uint32 event_GetEventPeriod(uint32 EventType);
	ARCEMU_INLINE void Virtual_Constructor(){};
	// Public methods
	EventableObject();
	virtual ~EventableObject();
	void Virtual_Destructor();

	ARCEMU_INLINE bool event_HasEvents() { return m_events.size() > 0 ? true : false; }
	void event_AddEvent(TimedEvent * ptr);
	void event_Relocate();
	
	// this func needs to be implemented by all eventable classes. use it to retreive the instance
	// id that it needs to attach itself to.
	
	virtual int32 event_GetInstanceID(){ return WORLD_INSTANCE; };

protected:
	FastMutex m_lock;
	EventMap m_events;
	EventableObjectHolder * m_holder;
	
};

/**
  * @class EventableObjectHolder
  * EventableObjectHolder will store eventable objects, and remove/add them when they change
  * from one holder to another (changing maps / instances).
  *
  * EventableObjectHolder also updates all the timed events in all of its objects when its
  * update function is called.
  *
  */

typedef set<EventableObject*> EventableObjectSet;

class EventableObjectHolder
{
public:
	EventableObjectHolder(int32 instance_id);
	~EventableObjectHolder();

	void Update(uint32 time_difference);

	void AddEvent(TimedEvent * ev);
	void AddObject(EventableObject * obj);

	ARCEMU_INLINE uint32 GetInstanceID() { return mInstanceId; }

protected:
	int32 mInstanceId;
	Mutex m_lock;
	EventList m_events;

	Mutex m_insertPoolLock;
	typedef list<TimedEvent*> InsertableQueue;
	InsertableQueue m_insertPool;
};

#endif
#endif

#ifndef USE_OLD_EVENT_MANAGER
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

#ifndef _EVENTABLE_OBJECT_H
#define _EVENTABLE_OBJECT_H

class EventableObjectHolder;

// Zack : the story 
// each object inherits EventableObject
// we add events to object using EventableObject so when destroying base object we can delete it's events also
// EventableObject is linked to EventableObjectHolder ( events are added to Holder's list also)
// EventableObjectHolder is updated with each map update cycle thus object event callback functions are called
// EventableObjectHolder is always updated while map is not destroyed
// each time an event is added to a list ref is incremented. Each time an event is removed from a list. ref is decremented. At the moment the event is not anymore in any list it gets deleted
// EventableObject list is kept until object is destroyed. I wonder how large it will grow in time
// Question : how come events get deleted more then they are added ? Decref called more then incref

/**
  * @class EventableObject
  * EventableObject means that the class inheriting this is able to take
  * events. This 'base' class will store and update these events upon
  * receiving the call from the instance thread / WorldRunnable thread.
  */

typedef list<TimedEvent*> EventList;
typedef multimap<uint32, TimedEvent*> EventMap;

#define WORLD_INSTANCE -1

class SERVER_DECL EventableObject
{
	friend class EventMgr;
	friend class EventableObjectHolder;

protected:
	void event_RemoveEvents();
	void event_RemoveEvents(uint32 EventType);
	bool event_ModifyTimeLeft(uint32 EventType, uint32 TimeLeft);
	void event_ModifyRepeatInterval(uint32 EventType, uint32 Time);
	void event_DelayTrigger(uint32 EventType, uint32 Time);
	bool event_HasEvent(uint32 EventType);
	uint32 event_GetTimeLeft(uint32 EventType);
	void event_RemoveByPointer(TimedEvent * ev); 
//	ARCEMU_INLINE int32 event_GetCurrentInstanceId() { return m_event_Instanceid; }

public:
//	uint32 event_GetEventPeriod(uint32 EventType);
	// Public methods
	EventableObject();
	virtual ~EventableObject();
	ARCEMU_INLINE void Virtual_Constructor();
	ARCEMU_INLINE void Virtual_Destructor();

	ARCEMU_INLINE bool event_HasEvents() { return m_events.empty(); }
	ARCEMU_INLINE void event_AddEvent(TimedEvent * ptr);
	void event_Relocate();
	
	// this func needs to be implemented by all eventable classes. use it to retreive the instance
	// id that it needs to attach itself to.
	
	virtual int32 event_GetInstanceID(){ return WORLD_INSTANCE; };

protected:
	FastMutex	m_lock;
	EventMap	m_events;
	EventableObjectHolder * m_holder;
	uint8		EVO_deleted;
};

/**
  * Idiot implementation of a minheap. Learn to sue someone. Jeesh it is not even a heap list just a list atm.
  * this is an always ascending ordered list based on event timestamp as key
  * beware of external modifications of the timestamp
  */
struct CacheListNode
{
	TimedEvent		*Val;	//holder of an event object
	CacheListNode	*Next;	//1 way linked list
};

class EventCacheList
{
public:
	//empty spaces
	EventCacheList()
	{	
		UsedNodesStartRead = UsedNodesStartWrite = NULL;	
		FreeNodesStartRead = FreeNodesStartWrite = NULL; 
	}
	//make lists empty
	~EventCacheList()
	{
		while( FreeNodesStartRead )
		{
			CacheListNode *del = FreeNodesStartRead;
			FreeNodesStartRead = FreeNodesStartRead->Next;
			delete del;
		}
		while( FreeNodesStartWrite )
		{
			CacheListNode *del = FreeNodesStartWrite;
			FreeNodesStartWrite = FreeNodesStartWrite->Next;
			delete del;
		}
		while( UsedNodesStartRead )
		{
			CacheListNode *del = UsedNodesStartRead;
			UsedNodesStartRead = UsedNodesStartRead->Next;
			del->Val->DecRef();
			delete del;
		}
		while( UsedNodesStartWrite )
		{
			CacheListNode *del = UsedNodesStartWrite;
			UsedNodesStartWrite = UsedNodesStartWrite->Next;
			del->Val->DecRef();
			delete del;
		}
	}
	//in case we want to iterate through the list
	ARCEMU_INLINE CacheListNode *BeginRead(){ return UsedNodesStartRead; }
	//add a new event to this list
	ARCEMU_INLINE void			PushWrite( TimedEvent *Event );
	//since list is key ordered we use events as long as they trigger and leave in list the rest
	ARCEMU_INLINE TimedEvent	*PopRead( );
	ARCEMU_INLINE void			SwapLists() 
	{
		CacheListNode *t = UsedNodesStartRead;
		UsedNodesStartRead = UsedNodesStartWrite;
		UsedNodesStartWrite = t;

		t = FreeNodesStartRead;
		FreeNodesStartRead = FreeNodesStartWrite;
		FreeNodesStartWrite = t;
	}
	ARCEMU_INLINE void			clear();
private:
	ARCEMU_INLINE CacheListNode *GetFreeNodeWite();
	CacheListNode *FreeNodesStartRead,*FreeNodesStartWrite;
	CacheListNode *UsedNodesStartRead,*UsedNodesStartWrite;
};

/**
  * @class EventableObjectHolder
  * EventableObjectHolder will store eventable objects, and remove/add them when they change
  * from one holder to another (changing maps / instances).
  *
  * EventableObjectHolder also updates all the timed events in all of its objects when its
  * update function is called.
  *
  * For large servers this tends to be big. In order to be able to update it quickly we use a priority list based on
  * update stamps. 
  *
  * Others do not detele from our list. We check at each update if we need to remove events from ourself. This gives
  * freedom to optimize handling of the list. Push and Pop should be as fast as possible. with 1000 players this can 
  * go up to 30k elemnts updated at each 50 MS
  *
  * !Event trigger times may change. We do not reorder the list on these events(atm) so the event trigger timestamp should not be decreased
  */

//use multiple event lists to boost inserts and popouts

#define EVENT_LIST_RESOLUTION	2000	//group events by trigger interval
#define EVENT_LIST_RES_COUNT	15		//number of lists we will store

class EventableObjectHolder
{
public:
	EventableObjectHolder(int32 instance_id);
	~EventableObjectHolder();

	void Update(uint32 time_difference);

	ARCEMU_INLINE void AddEvent(TimedEvent * ev);
	void AddObject(EventableObject * obj);

	ARCEMU_INLINE uint32 GetInstanceID() { return mInstanceId; }

protected:
	ARCEMU_INLINE void ReAddEvent(TimedEvent * ev,uint32 res_index);
	//this means that we are inside mutex lock already
	void			AddEventThreadSafe(TimedEvent * ev);
	int32			mInstanceId;
	Mutex			m_lock;
	//lists represent steps like less then 1 sec less then x sec...
	EventCacheList	m_HolderEvents[EVENT_LIST_RES_COUNT + 1];
	uint32			m_ListUpdateStamps[EVENT_LIST_RES_COUNT + 1];
};

#endif
#endif