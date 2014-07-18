#ifndef _ITEM_BUFFER_POOL
#define _ITEM_BUFFER_POOL

//hmm with 2000 players each have 50 items ...Decrease it if you need the space ;)
//@Zack 2000 x 50 = 10000? :P
#ifdef _DEBUG
	#define INITI_POOL_WITH_SIZE				500
	#define INITI_CONTAINER_POOL_WITH_SIZE		500
	//too big values migh create lag spikes on buffer limit extension !
	#define EXTEND_POOL_WITH_SIZE				10
#else
	#define INITI_POOL_WITH_SIZE				10000
	#define INITI_CONTAINER_POOL_WITH_SIZE		5000
	//too big values migh create lag spikes on buffer limit extension !
	#define EXTEND_POOL_WITH_SIZE				100
#endif

//let us have a gap of pool fullness. Some auras get deleted more then once when i write this code and they are overwriting each other
#define MINIMUM_POOL_FULNESS					10
#define POOL_IS_FULL_INDEX						0
#define OBJECT_WAS_ALLOCATED_STANDARD_WAY		0x0A000000	// object was allocated standard way and not pooled
#define OBJECT_WAS_ALLOCATED_WITHIN_POOL		0x0B000000	// object is sitting in the pool
#define OBJECT_WAS_ALLOCATED_WITHIN_POOL_USED	0x0C000000	// object is pooled and it is handed out to be used
#define OBJECT_WAS_DEALLOCATED					0x0D000000	// all deleted objects will have this ( ofc memory is already deallocated )

#define POOL_ID_MASK_ALLOC_TYPE					0x0F000000
#define POOL_ID_MASK_POOL_ID					0xF0000000
#define POOL_ID_MASK_OBJ_INDEX					(~(POOL_ID_MASK_ALLOC_TYPE|POOL_ID_MASK_POOL_ID))

//noob protection :P
#ifdef _DEBUG
//	#define TRACK_LEAKED_OBJECTS_AND_MEMORY_CORRUPTION
#endif

class Item;
class Container;
class Aura;
class Spell;

extern int PoolCounter;

/* Maybe we should use template< class T, int InitPoolSize > but for now
all Pools use the same val so we don't need */

template< class T > class SERVER_DECL tPPoolClass : public Singleton< tPPoolClass< T > >
{
private:
	typedef tPPoolClass< T > tBaseType;

	T					**avail_list;
	uint32				max_avails;
	Mutex				ObjLock;
	std::list<uint32>	avail_indexes;
	char				PoolName[100];
	uint32				ObjectSize;
	uint32				CurAvailFree;
	uint32				MyPoolID;		//to be able to detect if object is getting inserted into the same pool it came from
public:
	tPPoolClass()
		//lol i get compile error here. Is it my old VS 2k3 ?
		//: Singleton()
	{
		max_avails = INITI_POOL_WITH_SIZE;
		CurAvailFree = 0;

		avail_list = (T **)malloc( sizeof(T*) * max_avails );

		if( !avail_list )
			Log.Error( "PointerPool", "Pool failed to alocate more memory = %u bytes", sizeof(T*) * max_avails );

		PoolName[0] = 0;
		ObjectSize = 1;
		MyPoolID = ( PoolCounter++ ) << 28; //there are atm 3 pools

		InitPoolNewSection( 0, max_avails );
	}

	~tPPoolClass()
	{
		//? DestroyPool? :P
	};

	void SetPoolName( char *NewName ) 
	{
		int i;
		for( i=0;NewName[i]!=0 && i<100-1;i++)
			PoolName[i] = NewName[i];
		PoolName[i] = 0;
		if( ObjectSize == 1 )
			ObjectSize = sizeof( T ); //only estimated. Constructor might allocate more
	}

	//fetch from internal list a new Item object
	T* PooledNew( char *_FILE_, int _LINE_ )
	{
		ObjLock.Acquire();
TRY_NEW_ALLOC:
		//out of buffer ? get new ones !
		if( CurAvailFree < MINIMUM_POOL_FULNESS )
			ExtedLimitAvailLimit();
		//Get a Element from the availpool
		uint32 free_index = (*avail_indexes.begin());
		avail_indexes.pop_front();
		CurAvailFree--;
		uint32 ComboNumber = avail_list[ free_index ]->m_bufferPoolId;
		uint32 MyPoolIdAllocType = ComboNumber & POOL_ID_MASK_ALLOC_TYPE;
		uint32 MyPoolIdId = ComboNumber & POOL_ID_MASK_POOL_ID;
		uint32 MyPoolIdObjectIndex = ComboNumber & POOL_ID_MASK_OBJ_INDEX;
		if( MyPoolIdAllocType != OBJECT_WAS_ALLOCATED_WITHIN_POOL )
		{
#ifdef BUFFER_POOL_REMEMBER_HISTORY
			sLog.outError("Pool : MemCorruption 1? %d index %d with bad pool id %d in pool %s, free count %d. %s %d \n %s %d \n %s %d\n", ComboNumber, MyPoolIdObjectIndex, MyPoolID, PoolName, CurAvailFree, _FILE_, _LINE_, avail_list[ free_index ]->m_bufferPoolPrevAllocFILE, avail_list[ free_index ]->m_bufferPoolPrevAllocLine, avail_list[ free_index ]->m_bufferPoolPrevDeAllocFILE, avail_list[ free_index ]->m_bufferPoolPrevDeAllocLine );
#else
			sLog.outError("Pool : MemCorruption 1? %d index %d with bad pool id %d in pool %s, free count %d.\n", ComboNumber, MyPoolIdObjectIndex, MyPoolID, PoolName, CurAvailFree );
#endif
			goto TRY_NEW_ALLOC;
		}
		if( MyPoolIdId != MyPoolID )
		{
#ifdef BUFFER_POOL_REMEMBER_HISTORY
			sLog.outError("Pool : MemCorruption 2? %d index %d with bad pool id %d in pool %s, free count %d. %s %d \n %s %d \n %s %d\n", ComboNumber, MyPoolIdObjectIndex, MyPoolID, PoolName, CurAvailFree, _FILE_, _LINE_, avail_list[ free_index ]->m_bufferPoolPrevAllocFILE, avail_list[ free_index ]->m_bufferPoolPrevAllocLine, avail_list[ free_index ]->m_bufferPoolPrevDeAllocFILE, avail_list[ free_index ]->m_bufferPoolPrevDeAllocLine );
#else
			sLog.outError("Pool : MemCorruption 2? %d index %d with bad pool id %d in pool %s, free count %d.\n", ComboNumber, MyPoolIdObjectIndex, MyPoolID, PoolName, CurAvailFree );
#endif
			goto TRY_NEW_ALLOC;
		}
		if( MyPoolIdObjectIndex != free_index )
		{
#ifdef BUFFER_POOL_REMEMBER_HISTORY
			sLog.outError("Pool : MemCorruption 3? %d index %d with bad pool id %d in pool %s, free count %d. %s %d \n %s %d \n %s %d\n", ComboNumber, MyPoolIdObjectIndex, MyPoolID, PoolName, CurAvailFree, _FILE_, _LINE_, avail_list[ free_index ]->m_bufferPoolPrevAllocFILE, avail_list[ free_index ]->m_bufferPoolPrevAllocLine, avail_list[ free_index ]->m_bufferPoolPrevDeAllocFILE, avail_list[ free_index ]->m_bufferPoolPrevDeAllocLine );
#else
			sLog.outError("Pool : MemCorruption 3? %d index %d with bad pool id %d in pool %s, free count %d.\n", ComboNumber, MyPoolIdObjectIndex, MyPoolID, PoolName, CurAvailFree );
#endif
			goto TRY_NEW_ALLOC;
		}

		avail_list[ free_index ]->Virtual_Constructor();
		avail_list[ free_index ]->m_bufferPoolId = MyPoolID | MyPoolIdObjectIndex | OBJECT_WAS_ALLOCATED_WITHIN_POOL_USED;
#ifdef BUFFER_POOL_REMEMBER_HISTORY
		avail_list[ free_index ]->m_bufferPoolPrevAllocLine = _LINE_;
		avail_list[ free_index ]->m_bufferPoolPrevAllocFILE = _FILE_;
#endif
		ObjLock.Release();

		return avail_list[ free_index ];
	}
	uint32 GetPooledObjectCount(){ return max_avails; }
	float GetPoolSizeMB()
	{ 
		return max_avails * ObjectSize; 
	}
	float GetPoolUnusedSizeMB()
	{ 
		return CurAvailFree * ObjectSize; 
	}
	//insert into free object list
	void PooledDelete( T* dumped, char *_FILE_, int _LINE_ )
	{
		if( !dumped )
			return; //shit happens, in too many forms sadly

		//get our PoolId
		uint32 ComboNumber = (uint32)dumped->m_bufferPoolId;
		uint32 MyPoolIdAllocType = ComboNumber & POOL_ID_MASK_ALLOC_TYPE;
		uint32 MyPoolIdId = ComboNumber & POOL_ID_MASK_POOL_ID;
		uint32 MyPoolIdObjectIndex = ComboNumber & POOL_ID_MASK_OBJ_INDEX;

		if( MyPoolIdAllocType == OBJECT_WAS_ALLOCATED_STANDARD_WAY )
		{
#ifdef BUFFER_POOL_REMEMBER_HISTORY
//			printf("Pool : Object was allocated standard way. No point inserting it into pool. Deleting it!  %s %d \n %s %d \n %s %d\n", _FILE_, _LINE_, dumped->m_bufferPoolPrevAllocFILE, dumped->m_bufferPoolPrevAllocLine, dumped->m_bufferPoolPrevDeAllocFILE, dumped->m_bufferPoolPrevDeAllocLine );
			printf("Pool : Object was allocated standard way. No point inserting it into pool. Deleting it!\n");
#else
			printf("Pool : Object was allocated standard way. No point inserting it into pool. Deleting it! %s %d \n", _FILE_, _LINE_);
#endif
//			delete dumped;
			dumped = NULL;
			return;
		}
		else if( MyPoolIdAllocType == OBJECT_WAS_ALLOCATED_WITHIN_POOL )
		{
#ifdef BUFFER_POOL_REMEMBER_HISTORY
			printf("Pool : Object not even handed out to a request and it is inserted back into pool!  %s %d \n %s %d \n %s %d\n", _FILE_, _LINE_, dumped->m_bufferPoolPrevAllocFILE, dumped->m_bufferPoolPrevAllocLine, dumped->m_bufferPoolPrevDeAllocFILE, dumped->m_bufferPoolPrevDeAllocLine );
#else
			printf("Pool : Object not even handed out to a request and it is inserted back into pool!\n");
#endif
			ASSERT( false );
			return;
		}
		else if( MyPoolIdAllocType != OBJECT_WAS_ALLOCATED_WITHIN_POOL_USED )
		{
#ifdef BUFFER_POOL_REMEMBER_HISTORY
			printf("Pool : Object is in unknown state and not sure what to do with it. Leaking it !  %s %d \n %s %d \n %s %d\n", _FILE_, _LINE_, dumped->m_bufferPoolPrevAllocFILE, dumped->m_bufferPoolPrevAllocLine, dumped->m_bufferPoolPrevDeAllocFILE, dumped->m_bufferPoolPrevDeAllocLine );
#else
			printf("Pool : Object is in unknown state and not sure what to do with it. Leaking it !\n");
#endif
			ASSERT( false );
			return;
		}
		if( MyPoolIdId != MyPoolID )
		{
#ifdef BUFFER_POOL_REMEMBER_HISTORY
			printf("Pool : Object comes from a different pool. Maybe some forcecast is deleting it the wrong way ? Leaking it !  %s %d \n %s %d \n %s %d\n", _FILE_, _LINE_, dumped->m_bufferPoolPrevAllocFILE, dumped->m_bufferPoolPrevAllocLine, dumped->m_bufferPoolPrevDeAllocFILE, dumped->m_bufferPoolPrevDeAllocLine );
#else
			printf("Pool : Object comes from a different pool. Maybe some forcecast is deleting it the wrong way ? Leaking it !\n" );
#endif
			ASSERT( false );
			return;
		}
		//non pooled object requested or invalid
		if( MyPoolIdObjectIndex < 0 || MyPoolIdObjectIndex >= (int32)max_avails )
		{
#ifdef BUFFER_POOL_REMEMBER_HISTORY
			printf("Pool : Object has invalid pool index. Memory corruption ? Leaking it !  %s %d \n %s %d \n %s %d\n", _FILE_, _LINE_, dumped->m_bufferPoolPrevAllocFILE, dumped->m_bufferPoolPrevAllocLine, dumped->m_bufferPoolPrevDeAllocFILE, dumped->m_bufferPoolPrevDeAllocLine );
#else
			printf("Pool : Object has invalid pool index. Memory corruption ? Leaking it !\n");
#endif
			ASSERT( false );
			return;
		}

		ObjLock.Acquire();

		//we can delete it. Do not let Virtual_Destructor() double delete it
		//We do not care about used up guids only available ones. Note that with this overwrite used guid list is not valid anymore
		dumped->m_bufferPoolId = MyPoolID | MyPoolIdObjectIndex | OBJECT_WAS_ALLOCATED_WITHIN_POOL ;
#ifdef BUFFER_POOL_REMEMBER_HISTORY
		dumped->m_bufferPoolPrevDeAllocLine = _LINE_;
		dumped->m_bufferPoolPrevDeAllocFILE = _FILE_;
#endif
		//remove events and remove object from world ...
		dumped->Virtual_Destructor();

		avail_indexes.push_back( MyPoolIdObjectIndex );
		CurAvailFree++;

		ObjLock.Release();
	}
	//since some objects may depend on us and we might depend on other objects sometimes it is better to know when we destroy the pool instead of the destructor
	void DestroyPool( )
	{
		if( !avail_list )
			return;

		T**	temp_avail_list = avail_list;	//make sure there are no requests while we shut down

		avail_list = NULL;
		avail_indexes.clear();
		CurAvailFree = 0;

		for( uint32 i = 0; i < max_avails; i++ )
		{
			uint32 ComboNumber = (uint32)temp_avail_list[i]->m_bufferPoolId;
			uint32 MyPoolIdAllocType = ComboNumber & POOL_ID_MASK_ALLOC_TYPE;
			uint32 MyPoolIdId = ComboNumber & POOL_ID_MASK_POOL_ID;
			uint32 MyPoolIdObjectIndex = ComboNumber & POOL_ID_MASK_OBJ_INDEX;
			if( MyPoolIdAllocType == OBJECT_WAS_ALLOCATED_STANDARD_WAY )
				printf("Found a Non pooled item in pool list!");
			else if( temp_avail_list[i]->m_bufferPoolId != OBJECT_WAS_ALLOCATED_WITHIN_POOL ) //never handed out
			{
				if( temp_avail_list[i]->m_bufferPoolId < (int32)max_avails )
					printf("Incorrect pool state 'not yet deleted' while shutting down pool %d %d - %s", i, temp_avail_list[i]->m_bufferPoolId, PoolName );
				else
					printf("Incorrect pool state 'corrupted index' while shutting down pool %d %d - %s", i, temp_avail_list[i]->m_bufferPoolId, PoolName );
			}

			temp_avail_list[i]->m_bufferPoolId = OBJECT_WAS_ALLOCATED_STANDARD_WAY; //so delete would not panic
			delete temp_avail_list[i];
			temp_avail_list[i] = NULL;
		}

		free( temp_avail_list );
		ASSERT( avail_list == NULL );	//wtf we requested new pooled objects while shutting down ?
	}
private:
	void InitPoolNewSection(uint32 from, uint32 to)
	{
		float MemUsageBefore, MemUsageAfter;
		if( ObjectSize == 1 )
			MemUsageBefore = GetMemUsage();
		for( uint32 i = from; i < to;i++ )
		{
			avail_list[i] = new T;
			ASSERT( avail_list[i] );

			//to be ablt to track object created not from pool
			avail_list[i]->m_bufferPoolId = MyPoolID | i | OBJECT_WAS_ALLOCATED_WITHIN_POOL;
#ifdef BUFFER_POOL_REMEMBER_HISTORY
			avail_list[i]->m_bufferPoolPrevAllocLine = 0;
			avail_list[i]->m_bufferPoolPrevDeAllocLine = 0;
			avail_list[i]->m_bufferPoolPrevAllocFILE = "";
			avail_list[i]->m_bufferPoolPrevDeAllocFILE = "";
#endif
			avail_indexes.push_back( i );
			CurAvailFree++;
		}
		if( ObjectSize == 1 )
		{
			MemUsageAfter = GetMemUsage();
			ObjectSize = ( MemUsageAfter - MemUsageBefore ) / ( to - from );
		}
	}

	void ExtedLimitAvailLimit()
	{
		uint32 prev_max = max_avails;
		max_avails += EXTEND_POOL_WITH_SIZE;
		avail_list = (T**)realloc( avail_list, sizeof(T*) * max_avails );
		
		if( !avail_list )
			Log.Error( "PointerPool", "Pool failed to alocate more memory = %u bytes", sizeof(T*) * max_avails );

		InitPoolNewSection( prev_max, max_avails );
	}
};

/* Grey: ContainerPooling have to come with a sepperate Patch */

//#define ContainerPool tPPoolClass<Container>::getSingleton()
#define ItemPool tPPoolClass<Item>::getSingleton()
#define AuraPool tPPoolClass<Aura>::getSingleton()
#define SpellPool tPPoolClass<Spell>::getSingleton()

#endif

