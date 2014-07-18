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

#include "StdAfx.h"
#include "EventableObject.h"

#ifdef USE_OLD_EVENT_MANAGER
EventableObject::~EventableObject()
{
	/* decrement event count on all events */
	m_lock.Acquire();
	EventMap::iterator itr = m_events.begin();
	for(; itr != m_events.end(); ++itr)
	{
		itr->second->deleted = true;
		itr->second->DecRef();
	}
	m_events.clear();
	m_lock.Release();
	m_holder = NULL;
}

//we virtually are destroying this object that means we will have no more events and we do not belong to any holder anymore
void EventableObject::Virtual_Destructor()
{
	m_lock.Acquire();
	/* decrement event count on all events */
	EventMap::iterator itr = m_events.begin();
	for(; itr != m_events.end(); ++itr)
	{
		itr->second->deleted = true;
		itr->second->DecRef();
	}
	m_events.clear();
	m_lock.Release();

	m_holder = NULL;
}

EventableObject::EventableObject()
{
	m_holder = NULL;
}

void EventableObject::event_AddEvent(TimedEvent * ptr)
{
	m_lock.Acquire();

	if(!m_holder)
		m_holder = sEventMgr.GetEventHolder( event_GetInstanceID() );

	ptr->IncRef();
	ptr->instanceId = event_GetInstanceID();
	pair<uint32,TimedEvent*> p(ptr->eventType, ptr);
	m_events.insert( p );
	m_lock.Release();

	/* Add to event manager */
	if(!m_holder)
	{
		m_holder = sEventMgr.GetEventHolder( WORLD_INSTANCE );
		ASSERT(m_holder);
	}

	m_holder->AddEvent(ptr);
}

void EventableObject::event_RemoveEvents(uint32 EventType)
{
	m_lock.Acquire();
	if( m_events.empty() )
	{
		m_lock.Release();
		return;
	}

	if(EventType == EVENT_REMOVAL_FLAG_ALL)
	{
		EventMap::iterator itr = m_events.begin();
		for(; itr != m_events.end(); ++itr)
		{
			itr->second->deleted = true;
			itr->second->DecRef();
		}
		m_events.clear();
	}
	else
	{
		EventMap::iterator itr2,itr;
		for(itr = m_events.begin(); itr != m_events.end();)
		{
			itr2 = itr;
			itr++;
			if( itr2->second->eventType == EventType )
			{
				itr2->second->deleted = true;
				itr2->second->DecRef();
				m_events.erase(itr2);
			}
		}
	}

	m_lock.Release();
}

void EventableObject::event_RemoveEvents()
{
	event_RemoveEvents(EVENT_REMOVAL_FLAG_ALL);
}

void EventableObject::event_ModifyTimeLeft(uint32 EventType, uint32 TimeLeft,bool unconditioned)
{
	m_lock.Acquire();
	if( m_events.empty() )
	{
		m_lock.Release();
		return;
	}

	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			if(unconditioned)
				itr->second->currTime = TimeLeft;
			else itr->second->currTime = ((int32)TimeLeft > itr->second->msTime) ? itr->second->msTime : (int32)TimeLeft;
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}

	m_lock.Release();
}

bool EventableObject::event_GetTimeLeft(uint32 EventType, uint32 * Time)
{
	m_lock.Acquire();
	if( m_events.empty() )
	{
		m_lock.Release();
		return false;
	}

	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			if( itr->second->deleted )
			{
				++itr;
				continue;
			}

			*Time = (uint32)itr->second->currTime;
			m_lock.Release();
			return true;

		} while(itr != m_events.upper_bound(EventType));
	}

	m_lock.Release();
	return false;
}

void EventableObject::event_ModifyTime(uint32 EventType, uint32 Time)
{
	m_lock.Acquire();
	if( m_events.empty() )
	{
		m_lock.Release();
		return;
	}

	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			itr->second->msTime = Time;
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}

	m_lock.Release();
}

void EventableObject::event_ModifyRepeatInterval(uint32 EventType, uint32 Time)
{
	m_lock.Acquire();
	if( m_events.empty() )
	{
		m_lock.Release();
		return;
	}

	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			itr->second->currTime = itr->second->msTime = Time;
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}

	m_lock.Release();
}

void EventableObject::event_DelayTrigger(uint32 EventType, uint32 Time)
{
	m_lock.Acquire();
	if( m_events.empty() )
	{
		m_lock.Release();
		return;
	}

	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			itr->second->currTime += Time;
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}

	m_lock.Release();
}

bool EventableObject::event_HasEvent(uint32 EventType)
{
	bool ret = false;
	m_lock.Acquire();
	if( m_events.empty() )
	{
		m_lock.Release();
		return false;
	}

	//ret = m_events.find(EventType) == m_events.end() ? false : true;
	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			if(!itr->second->deleted)
			{
				ret = true;
				break;
			}
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}

	m_lock.Release();
	return ret;
}

void EventableObject::event_Relocate()
{
	/* prevent any new stuff from getting added */
	m_lock.Acquire();

	EventableObjectHolder * nh = sEventMgr.GetEventHolder( event_GetInstanceID() );
	if(nh != m_holder)
	{
		// whee, we changed event holder :>
		// doing this will change the instanceid on all the events, as well as add to the new holder.
		
		// no need to do this if we don't have any events, though.
		if(!nh)
			nh = sEventMgr.GetEventHolder(-1);

		// we had ref from object and holder( 1 + 1 ) but when holder got destroyed the ref got decreased to 1
		// this will increase references to events by one 
		nh->AddObject(this);

		// reset our m_holder pointer and instance id
		m_holder = nh;
	}

	// safe again to add 
	m_lock.Release();
}

uint32 EventableObject::event_GetEventPeriod(uint32 EventType)
{
	uint32 ret = 0;
	m_lock.Acquire();
	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
		ret = (uint32)itr->second->msTime;
	
	m_lock.Release();
	return ret;
}

EventableObjectHolder::EventableObjectHolder(int32 instance_id) : mInstanceId(instance_id)
{
	sEventMgr.AddEventHolder(this, instance_id);
}

EventableObjectHolder::~EventableObjectHolder()
{
	/* decrement events reference count */
	m_lock.Acquire();
	sEventMgr.RemoveEventHolder(this);
	EventList::iterator itr = m_events.begin();
	for(; itr != m_events.end(); ++itr)
		(*itr)->DecRef();
	m_events.clear();
	m_lock.Release();
}

void EventableObjectHolder::Update(uint32 time_difference)
{
	m_lock.Acquire();			// <<<<

	/* Insert any pending objects in the insert pool. */
	m_insertPoolLock.Acquire();
	InsertableQueue::iterator iqi = m_insertPool.begin();
	//no need to increase refenrences, we are moving events from one list to another
	while(iqi!= m_insertPool.end())
	{
		if((*iqi)->deleted || (*iqi)->instanceId != mInstanceId )
			(*iqi)->DecRef();
		else
			m_events.push_back( (*iqi) );
		iqi++;
	}
	m_insertPool.clear(); //instead of multiple erases only 1 clear should be more stable no ?
	m_insertPoolLock.Release();

	/* Now we can proceed normally. */
	EventList::iterator itr = m_events.begin();
	EventList::iterator it2;
	TimedEvent * ev;

	while(itr != m_events.end())
	{
		it2 = itr++;
		ev = *it2;

		if( ev->instanceId != mInstanceId //wrong context
			|| ev->deleted //something deleted us
			|| ( mInstanceId == WORLD_INSTANCE && ev->eventFlag & EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT)	//we got removed and this event is onyl executed while inworld
			)
		{
			// remove from this list.
			m_events.erase(it2);
			ev->DecRef();
			continue;
		}

		// Event Update Procedure

		if((uint32)ev->currTime <= time_difference)
		{
			// execute the callback
			if(ev->eventFlag & EVENT_FLAG_DELETES_OBJECT)
			{
				ev->deleted = true;
				ev->cb->execute();
				m_events.erase(it2);
				ev->DecRef();
				continue;
			}
			else
				ev->cb->execute();

			// check if the event is expired now.
            if(ev->repeats && --ev->repeats == 0)
			{
				/* remove the event from here */
				ev->deleted = true;
				m_events.erase(it2);
				ev->DecRef();
				continue;
			}
			else if(ev->deleted)
			{
				// event is now deleted
				m_events.erase(it2);
				ev->DecRef(); //this was added on "addevent"
				continue;
			}

			// event has to repeat again, reset the timer
			ev->currTime = ev->msTime;
		}
		else
		{
			// event is still "waiting", subtract time difference
			ev->currTime -= time_difference;
		}
	}

	m_lock.Release();
}

void EventableObjectHolder::AddEvent(TimedEvent * ev)
{
	// m_lock NEEDS TO BE A RECURSIVE MUTEX
	// zack : not true. While updating we add the new stuff to the insert pool and lock should not appear
	ev->IncRef();
	if(!m_lock.AttemptAcquire())
	{
		m_insertPoolLock.Acquire();
		m_insertPool.push_back( ev );
		m_insertPoolLock.Release();
	}
	else
	{
		m_events.push_back( ev );
		m_lock.Release();
	}
}

void EventableObjectHolder::AddObject(EventableObject * obj)
{
	// transfer all of this objects events into our holder
	if(!m_lock.AttemptAcquire())
	{
		// The other thread is obviously occupied. We have to use an insert pool here, otherwise
		// if 2 threads relocate at once we'll hit a deadlock situation.
		m_insertPoolLock.Acquire();
		EventMap::iterator it2;

		obj->m_lock.Acquire(); //do not let the list change in some other thread
		EventMap::iterator itr,itr2;
		for(itr2 = obj->m_events.begin(); itr2 != obj->m_events.end();)
		{
			itr = itr2;
			itr2++;
			// ignore deleted events (shouldn't be any in here, actually)
			if(itr->second->deleted)
			{
				itr->second->DecRef();
				obj->m_events.erase( itr );
				continue;
			}

			if(mInstanceId == WORLD_INSTANCE && itr->second->eventFlag & EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT)
				continue;
			itr->second->IncRef();	
			itr->second->instanceId = mInstanceId;
			m_insertPool.push_back(itr->second);
		}
		obj->m_lock.Release();

		// Release the insert pool.
		m_insertPoolLock.Release();

		// Ignore the rest of this stuff
		return;
	}
	else
	{
		obj->m_lock.Acquire(); //do not let the list change in some other thread
		EventMap::iterator itr,itr2;
		for(itr2 = obj->m_events.begin(); itr2 != obj->m_events.end();)
		{
			itr = itr2;
			itr2++;
			// ignore deleted events (shouldn't be any in here, actually)
			if(itr->second->deleted)
			{
				itr->second->DecRef();
				obj->m_events.erase( itr );
				continue;
			}

			if(mInstanceId == WORLD_INSTANCE && itr->second->eventFlag & EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT)
				continue;

			itr->second->IncRef();
			itr->second->instanceId = mInstanceId;
			m_events.push_back( itr->second );
		}
		obj->m_lock.Release();
		m_lock.Release();
	}
}
#endif

#ifndef USE_OLD_EVENT_MANAGER
EventableObject::~EventableObject()
{
	/* decrement event count on all events */
	m_lock.Acquire();
	EventMap::iterator itr = m_events.begin();
	for(; itr != m_events.end(); ++itr)
	{
		itr->second->TE_deleted = 1;
		itr->second->DecRef();
	}
	m_events.clear();
	m_lock.Release();
	m_holder = NULL;
}

//we virtually are destroying this object that means we will have no more events and we do not belong to any holder anymore
void EventableObject::Virtual_Destructor()
{
	m_lock.Acquire();
	/* decrement event count on all events */
	EventMap::iterator itr = m_events.begin();
	for(; itr != m_events.end(); ++itr)
	{
		itr->second->TE_deleted = 1;
		itr->second->DecRef();
	}
	m_events.clear();
	m_lock.Release();
	EVO_deleted = 1;

	m_holder = NULL;
}

EventableObject::EventableObject()
{
	m_holder = NULL;
	EVO_deleted = 0;
}

void EventableObject::Virtual_Constructor()
{
	m_holder = NULL;
	EVO_deleted = 0; 
}

void EventableObject::event_AddEvent(TimedEvent * ptr)
{
	m_lock.Acquire();

	//WTF ? adding events to deleted objects ? This is a NOWAY check
	//!! happened a lot of times with corpses(skinned players) + players that were unable to teleport to a map
	//!! need to debug and remove all possible asserts that happened here
	if( EVO_deleted == 1 || ptr->TE_deleted == 1 )
	{
		m_lock.Release();
		ASSERT( false );
		return;
	}

	if( ptr->eventFlag & EVENT_FLAG_UNIQUE_EVENT_DELETE_NEWER )
	{
		if( event_HasEvent( ptr->eventType ) )
		{
			delete ptr;
			m_lock.Release();
			return;
		}
	}
		
	if(!m_holder)
		m_holder = sEventMgr.GetEventHolder( event_GetInstanceID() );

	ptr->IncRef();
	ptr->instanceId = event_GetInstanceID();
	pair<uint32,TimedEvent*> p(ptr->eventType, ptr);
	m_events.insert( p );
	m_lock.Release();

	/* Add to event manager */
	if(!m_holder)
	{
		m_holder = sEventMgr.GetEventHolder( WORLD_INSTANCE );
		ASSERT(m_holder);
	}

	m_holder->AddEvent(ptr);
}

void EventableObject::event_RemoveEvents(uint32 EventType)
{
	m_lock.Acquire();
	if(m_events.empty())
	{
		m_lock.Release();
		return;
	}

	EventMap::iterator itr2,itr;
	for(itr = m_events.begin(); itr != m_events.end();)
	{
		itr2 = itr;
		itr++;
		if( itr2->second->eventType == EventType )
		{
			itr2->second->TE_deleted = 1;
			itr2->second->DecRef();
			m_events.erase(itr2);
		}
	}

	m_lock.Release();
}

void EventableObject::event_RemoveByPointer(TimedEvent * ev)
{
	m_lock.Acquire();
	EventMap::iterator itr = m_events.find(ev->eventType);
	EventMap::iterator it2;
	if(itr != m_events.end())
	{
		do 
		{
			it2 = itr++;

			if(it2->second == ev)
			{
				ev->TE_deleted = 1;
				m_events.erase(it2);
				ev->DecRef();
				m_lock.Release();
				return;
			}

		} while(itr != m_events.upper_bound(ev->eventType));
	}
	m_lock.Release();
}

void EventableObject::event_RemoveEvents()
{
	m_lock.Acquire();
	if( m_events.empty() )
	{
		m_lock.Release();
		return;
	}

	EventMap::iterator itr = m_events.begin();
	for(; itr != m_events.end(); ++itr)
	{
		itr->second->TE_deleted = 1;
		itr->second->DecRef();
	}
	m_events.clear();

	m_lock.Release();
}

bool EventableObject::event_ModifyTimeLeft(uint32 EventType, uint32 TimeLeft)
{
	m_lock.Acquire();
	if( m_events.empty() )
	{
		m_lock.Release();
		return false;
	}

	bool FoundEvents = false;
	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			if( itr->second->TE_deleted == 0 )
			{
				itr->second->TriggerAtStamp = getMSTime() + TimeLeft;
				FoundEvents = true;
			}
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}
	m_lock.Release();
	return FoundEvents;
}

void EventableObject::event_ModifyRepeatInterval(uint32 EventType, uint32 Time)
{
	m_lock.Acquire();
	if( m_events.empty() )
	{
		m_lock.Release();
		return;
	}

	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			itr->second->TriggerTimeInterval = Time;
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}

	m_lock.Release();
}

void EventableObject::event_DelayTrigger(uint32 EventType, uint32 Time)
{
	m_lock.Acquire();
	if( m_events.empty() )
	{
		m_lock.Release();
		return;
	}

	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			itr->second->TriggerAtStamp += Time;
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}

	m_lock.Release();
} 

bool EventableObject::event_HasEvent(uint32 EventType)
{
	bool ret = false;
	m_lock.Acquire();
	if( m_events.empty() )
	{
		m_lock.Release();
		return false;
	}

	//ret = m_events.find(EventType) == m_events.end() ? false : true;
	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			if( itr->second->TE_deleted == 0 )
			{
				ret = true;
				break;
			}
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}

	m_lock.Release();
	return ret;
}

uint32 EventableObject::event_GetTimeLeft(uint32 EventType)
{
	uint32 TickAtStamp = getMSTime();
	m_lock.Acquire();
	if( m_events.empty() )
	{
		m_lock.Release();
		return TickAtStamp;
	}

	//ret = m_events.find(EventType) == m_events.end() ? false : true;
	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
	{
		do 
		{
			if( itr->second->TE_deleted == 0 )
			{
				TickAtStamp = itr->second->TriggerAtStamp;
				break;
			}
			++itr;
		} while(itr != m_events.upper_bound(EventType));
	}

	m_lock.Release();
	if( TickAtStamp >= getMSTime() )
		return TickAtStamp - getMSTime();
	return 0;
}

void EventableObject::event_Relocate()
{
	/* prevent any new stuff from getting added */
	m_lock.Acquire();

	EventableObjectHolder * nh = sEventMgr.GetEventHolder( event_GetInstanceID() );
	if(nh != m_holder)
	{
		// whee, we changed event holder :>
		// doing this will change the instanceid on all the events, as well as add to the new holder.
		
		// no need to do this if we don't have any events, though.
		if(!nh)
			nh = sEventMgr.GetEventHolder(-1);

		// we had ref from object and holder( 1 + 1 ) but when holder got destroyed the ref got decreased to 1
		// this will increase references to events by one 
		nh->AddObject(this);

		// reset our m_holder pointer and instance id
		m_holder = nh;
	}

	/* safe again to add */
	m_lock.Release();
}
/*
uint32 EventableObject::event_GetEventPeriod(uint32 EventType)
{
	uint32 ret = 0;
	m_lock.Acquire();
	EventMap::iterator itr = m_events.find(EventType);
	if(itr != m_events.end())
		ret = (uint32)itr->second->TriggerTimeInterval;
	
	m_lock.Release();
	return ret;
}*/

EventableObjectHolder::EventableObjectHolder(int32 instance_id) : mInstanceId(instance_id)
{
	sEventMgr.AddEventHolder(this, instance_id);
	for(int32 i=0;i<=EVENT_LIST_RES_COUNT;i++)
		m_ListUpdateStamps[i] = 0;
}

EventableObjectHolder::~EventableObjectHolder()
{
	/* decrement events reference count */
	m_lock.Acquire();
	sEventMgr.RemoveEventHolder(this);
	for( uint32 i=0;i<=EVENT_LIST_RES_COUNT;i++)
	{
		m_HolderEvents[i].SwapLists();
		CacheListNode *itr = m_HolderEvents[i].BeginRead();
		while( itr )
		{
			itr->Val->DecRef();
			itr = itr->Next;
		}
		m_HolderEvents[i].SwapLists();
		itr = m_HolderEvents[i].BeginRead();
		while( itr )
		{
			itr->Val->DecRef();
			itr = itr->Next;
		}
		m_HolderEvents[i].clear();
	}
	m_lock.Release();
}

void EventableObjectHolder::Update(uint32 time_difference)
{
	//just so compiler can optimize this
	const uint32 TimeNow = getMSTime();

	for( int32 i=0;i<=EVENT_LIST_RES_COUNT;i++)
	{
		if( m_ListUpdateStamps[i] <= TimeNow )
		{
			//this needs to be threadsafe and this function shouldnot be called from anywhere else 
			m_lock.Acquire();			//
			//disable updating this list until we get values for it
			m_ListUpdateStamps[i] = 0xFFFFFFFF;
			//bring forth the write list as read list
			m_HolderEvents[i].SwapLists();
			m_lock.Release();			//
			//similar to std iteration 
			CacheListNode *itr = m_HolderEvents[i].BeginRead();
			//We chage the list so we need to have "end" saved into a variable !
			while( itr )
			{
				itr= itr->Next;
				//we eat up this list
				TimedEvent * ev = m_HolderEvents[i].PopRead( );
				//check if event got deleted or would execute in wrong context( player moved on other map)
				if( 
					ev->TE_deleted //something deleted us
					|| ev->instanceId != mInstanceId //wrong context
					|| ( mInstanceId == WORLD_INSTANCE && ev->eventFlag & EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT)	//we got removed and this event is onyl executed while inworld
					)
				{
					ev->DecRef();
					continue;
				}

				if( ev->TriggerAtStamp <= TimeNow )
				{
					// execute the callback
					ev->cb->execute();

					// check if the event is expired now.
					if( ( ev->repeats && --ev->repeats == 0 ) || ev->TE_deleted == 1 )
					{
						// no more procs, mark event as deleted
						ev->TE_deleted = 1;
						ev->DecRef();
						continue;
					}

					// event has to repeat again, reset the timer. Note that using this way would help us recover server lag
					// but then migh fuck up our iterator 
					ev->TriggerAtStamp += ev->TriggerTimeInterval;
				}
				//repeating events are not very frecvent
				ReAddEvent( ev, i );
			}
		}
	}
}

void EventableObjectHolder::AddEvent(TimedEvent * ev)
{
	m_lock.Acquire();
	//noway wtf check here 
	if( ev->TE_deleted == 1 )
	{
		ASSERT( false );
		m_lock.Release();
		return;
	}
	ev->IncRef();
	AddEventThreadSafe( ev );
	m_lock.Release();
}

void EventableObjectHolder::ReAddEvent(TimedEvent * ev,uint32 res_index)
{
	m_lock.Acquire();
	if( ev->TE_deleted == 1 )
	{
//		ASSERT( false );	//can happen if for example object gets queued for deletion, than deleted by something, then event holder gets updated and it's not yet time to delete the object. Rare, but it can happen
		m_lock.Release();
		return;
	}
	//make sure we will not forget to update this list
	if( ev->TriggerAtStamp < m_ListUpdateStamps[ res_index ] )
		m_ListUpdateStamps[ res_index ] = ev->TriggerAtStamp;
	//store event
	m_HolderEvents[ res_index ].PushWrite( ev );
	m_lock.Release();
}

 void EventableObjectHolder::AddEventThreadSafe(TimedEvent * ev)
{
//	m_lock.Acquire();
	if( ev->TE_deleted == 1 )
	{
		ASSERT( false );
//		m_lock.Release();
		return;
	}
	//time diff when it will trigger
	int32 time_diff = ev->TriggerAtStamp - getMSTime();
	//depending on next trigger this will be added to different list
	uint32 res_index = time_diff / EVENT_LIST_RESOLUTION;
	//make sure we do not get bad index
	if( res_index > EVENT_LIST_RES_COUNT )
		res_index = EVENT_LIST_RES_COUNT;
	//make sure we will not forget to update this list
	if( ev->TriggerAtStamp < m_ListUpdateStamps[ res_index ] )
		m_ListUpdateStamps[ res_index ] = ev->TriggerAtStamp;
	//store event
	m_HolderEvents[ res_index ].PushWrite( ev );
//	m_lock.Release();
}

void EventableObjectHolder::AddObject(EventableObject * obj)
{
	//this triggers a lot because on object deletion this is called
	if( obj->EVO_deleted == 1 )
	{
//		ASSERT( false );
		return;
	}
	// transfer all of this objects events into our holder
	m_lock.Acquire();
	obj->m_lock.Acquire(); //do not let the list change in some other thread
	EventMap::iterator itr,itr2;
	for(itr2 = obj->m_events.begin(); itr2 != obj->m_events.end();)
	{
		itr = itr2;
		itr2++;
		// ignore deleted events (shouldn't be any in here, actually)
		if(itr->second->TE_deleted)
		{
			itr->second->DecRef();
			obj->m_events.erase( itr );
			continue;
		}

		if(mInstanceId == WORLD_INSTANCE && itr->second->eventFlag & EVENT_FLAG_DO_NOT_EXECUTE_IN_WORLD_CONTEXT)
			continue;

		itr->second->IncRef();
		itr->second->instanceId = mInstanceId;
		AddEventThreadSafe( itr->second );
	}
	obj->m_lock.Release();
	m_lock.Release();
}

void	EventCacheList::PushWrite( TimedEvent *Event )
{
	//fetch a new node holder
	CacheListNode *NewNode = GetFreeNodeWite();
	//dump old start
	NewNode->Next = UsedNodesStartWrite;
	UsedNodesStartWrite = NewNode;
	//store val
	NewNode->Val = Event;
}

//remove an event in case we have an iterator pointing to it
TimedEvent	*EventCacheList::PopRead( )
{
	//remove first node from used list
	if( UsedNodesStartRead )
	{
		//backup return value
		CacheListNode *OldFirst = UsedNodesStartRead;
		//jump to next node
		UsedNodesStartRead = UsedNodesStartRead->Next;
		//add old node to free list
		OldFirst->Next = FreeNodesStartRead;
		FreeNodesStartRead = OldFirst;
		//and return the value
		return OldFirst->Val;
	}
	return NULL;
}

void	EventCacheList::clear()
{
	while( UsedNodesStartRead )
	{
		if( FreeNodesStartRead )
			FreeNodesStartRead->Next = FreeNodesStartRead;
		FreeNodesStartRead = UsedNodesStartRead;
		UsedNodesStartRead = UsedNodesStartRead->Next;
	}
	while( UsedNodesStartWrite )
	{
		if( FreeNodesStartWrite )
			FreeNodesStartWrite->Next = FreeNodesStartWrite;
		FreeNodesStartWrite = UsedNodesStartWrite;
		UsedNodesStartWrite = UsedNodesStartWrite->Next;
	}
}

CacheListNode *EventCacheList::GetFreeNodeWite()
{
	CacheListNode *Ret;
	if( FreeNodesStartWrite )
	{
		Ret = FreeNodesStartWrite;
		FreeNodesStartWrite = FreeNodesStartWrite->Next;
	}
	else
		Ret = new CacheListNode;
	return Ret;
}
#endif
