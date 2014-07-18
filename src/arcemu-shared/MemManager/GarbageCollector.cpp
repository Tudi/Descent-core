#include "StdAfx.h"

void	DelayedObjectDeletor::AddObject(Object *p)
{
	if( p == NULL )
		return;
	//avoid double add ?
	if( p->deleted == OBJ_AVAILABLE )
	{
		//hackshit or objects will keep sending packets to "deleted" player
		//need to fix socket issues then i will get back on this
		p->Virtual_Destructor();
/*		if( p->IsPlayer() )
		{
			delete p;
			return;
		}/**/
		//mark deleted after we deleted it
		p->deleted = OBJ_POOLED_FOR_DELETE;
		//add to our delayed fizical delete queue
		list_lock.Acquire();
		delete_list[ p ] = getMSTime() + GARBAGE_DELETE_DELAY;
		list_lock.Release();
	}
}

void	DelayedObjectDeletor::Update()
{
	uint32 ticknow = getMSTime();
	if( NextUpdateStamp > ticknow )
		return;
	NextUpdateStamp = ticknow + GARBAGE_COLLECTOR_UPDATE_INTERVAL;
	std::map<Object *,uint32>::iterator itr,itr2;
	list_lock.Acquire();
	for( itr = delete_list.begin(); itr != delete_list.end();)
	{
		itr2 = itr;
		itr++;
		if( itr2->second <= ticknow )
		{
			if( itr2->first->deleted == OBJ_POOLED_FOR_DELETE )
			{
				itr2->first->deleted = OBJ_DELETED;	//totally pointless, but maybe helps debugging
				delete itr2->first;
			}
			else
			{
				//wow, how did we get here ?
				ASSERT( false );
			}
			delete_list.erase( itr2 );
		}
	}
	list_lock.Release();
}

DelayedObjectDeletor::~DelayedObjectDeletor()
{
	list_lock.Acquire();
	std::map<Object *,uint32>::iterator itr2;
	for( itr2 = delete_list.begin(); itr2 != delete_list.end();itr2++)
	{
		if( itr2->first->deleted == OBJ_POOLED_FOR_DELETE )
		{
			itr2->first->deleted = OBJ_DELETED;	//totally pointless, but maybe helps debugging
			delete itr2->first;
		}
		else
		{
			//wow, how did we get here ?
			ASSERT( false );
		}
	}
	delete_list.clear();
	list_lock.Release();
}

initialiseSingleton( DelayedObjectDeletor );
