#ifndef _LOW_FRAGMENTATION_FIXED_SET_
#define _LOW_FRAGMENTATION_FIXED_SET_

#ifdef _DEBUG
#include <assert.h>
#endif

// This was made for a specific purpuse !
// This starts loosing speedwise compared to STD::SET if larger then 50 values ( on my PC )
// The whole idea is to not fragment memory too much so iteration can maintain speed over longer periods
// The real purpuse i implemented this is to reduce "NEW" calls as something corrupts memory and kept crashing. 
// Avg player visibility vector size is 44 units. This can go up to 600 at larger boss encounters. Overall should maintain speed with older STD implementation
// !!! WARNING !! Set can only be changed while not iterating through it
// Rev version has the advantage that newly added values will not be processed this loop

#define STATICPOINTERSET_MIN_INCREASE	10
#define STATICPOINTERSET_SAFETY_GUARD	1		//at least 1
#define STATICPOINTERSET_CLEANUP_DIFF	20		//if more then 10 values got removed since last cleanup. Do cleanup
#define STATICPOINTERSET_STRANGE_SIZE	2000	//this class was not designed to store DB on it. If you ever happen to get here then something is very wrong

template <typename T> class StaticPointerSetRev
{
public:
	StaticPointerSetRev()
	{
		ContentVect = NULL;
//		ContentVect = (T*)malloc( sizeof( T ) * STATICPOINTERSET_SAFETY_GUARD ); //this is only to avoid null tests
//		memset( &ContentVect[0], 0, sizeof( T ) * STATICPOINTERSET_SAFETY_GUARD );
		ContentMaxCount = 0;
		ValuesUsed = 0;
		FirstUsed = 0;
	}
	StaticPointerSetRev( StaticPointerSetRev<T> *CopyCons )
	{
		ContentVect = NULL;
		ContentMaxCount = 0;
		ExtendSize( CopyCons->ContentMaxCount );
		memcpy( &ContentVect[0], &CopyCons->ContentVect[0], CopyCons->ContentMaxCount );
		ContentMaxCount = CopyCons->ContentMaxCount;
		ValuesUsed = CopyCons->ValuesUsed;
		FirstUsed = CopyCons->FirstUsed;
	}
	StaticPointerSetRev( StaticPointerSetRev<T> &CopyCons )
	{
		ContentVect = NULL;
		ContentMaxCount = 0;
		ExtendSize( CopyCons.ContentMaxCount );
		memcpy( &ContentVect[0], &CopyCons.ContentVect[0], sizeof( T ) * CopyCons.ContentMaxCount );
		ValuesUsed = CopyCons.ValuesUsed;
		FirstUsed = CopyCons.FirstUsed;
	}
	~StaticPointerSetRev()
	{
		if( ContentVect != NULL )
			free( ContentVect );
		ContentVect = NULL;
		ContentMaxCount = 0;
		ValuesUsed = 0;
		FirstUsed = 0;
	}
	typedef T* iterator;
	__forceinline T* begin()	{ return &ContentVect[FirstUsed]; }
	__forceinline T* end()		{ return &ContentVect[ContentMaxCount]; }
	__forceinline bool empty() 
	{
		return ( ValuesUsed == 0 );
	}
	__forceinline void erase( T value )
	{
		for( int i=FirstUsed;i<ContentMaxCount;i++ )
			if( ContentVect[i] == value )
			{
				ValuesUsed--;
				if( FirstUsed != i )
				{
					ContentVect[i] = ContentVect[FirstUsed];
#ifdef _DEBUG		
					ContentVect[FirstUsed] = NULL;
#endif
				}
#ifdef _DEBUG		
				else
					ContentVect[i] = NULL;
				assert( ContentVect[ FirstUsed + 1 ] != NULL || FirstUsed + 1 == ContentMaxCount );
#endif
				FirstUsed++;
			}
	}
	__forceinline void erase( iterator itr )
	{
		erase( *itr );
	}
	__forceinline void insert( T value )
	{
		if( value == NULL )
			return;
		if( find( value ) != end() )
			return;
		InsertAsList( value );
	}
	__forceinline void InsertAsList( T value )
	{
		//we are full, extending it
		if( FirstUsed == 0 )
		{
			if( ExtendSize() == false )
				return; //this is bad. We could not extend it
		}
		FirstUsed--;
#ifdef _DEBUG		
		assert( value != NULL );
#endif
		ContentVect[ FirstUsed ] = value;
		ValuesUsed++;
	}
	__forceinline void	ReserveSize( int SizeNeededFree )
	{
		int NeedToExtend = SizeNeededFree - hasfree();
		if( NeedToExtend > 0 )
			ExtendSize( NeedToExtend );
	}
	__forceinline bool	ExtendSize( int SizeIncrease = STATICPOINTERSET_MIN_INCREASE )
	{
		int NewSize = ContentMaxCount + SizeIncrease + STATICPOINTERSET_SAFETY_GUARD;
		//refuse to store more because we are Jerks
		if( NewSize > STATICPOINTERSET_STRANGE_SIZE )
		{
#if defined ASSERT
			ASSERT( false );
#endif
			return false;
		}
		T	*NewContentVect = (T*)malloc( sizeof( T ) * NewSize );
#ifdef _DEBUG		
		memset( &NewContentVect[0], 0, sizeof( T ) * SizeIncrease );
#endif
		if( ContentVect )
		{
			memcpy( &NewContentVect[SizeIncrease], &ContentVect[0], sizeof( T ) * ( ContentMaxCount + STATICPOINTERSET_SAFETY_GUARD ) );
			free( ContentVect );
		}
		else
		{
			memset( &NewContentVect[0], 0, sizeof( T ) * ( SizeIncrease + STATICPOINTERSET_SAFETY_GUARD ) );
		}
		ContentVect = NewContentVect;
		ContentMaxCount += SizeIncrease;
		FirstUsed += SizeIncrease;
#ifdef _DEBUG		
		assert( ContentVect[ FirstUsed - 1 ] == NULL && ( FirstUsed == ContentMaxCount || ContentVect[ FirstUsed ] != NULL ) );
#endif
		return ( ContentVect != NULL );
	}
	__forceinline void clear()
	{
#ifdef _DEBUG		
		memset( ContentVect, 0, sizeof( T ) * ContentMaxCount );
#endif
		ValuesUsed = 0;
		FirstUsed = ContentMaxCount;
	}
	__forceinline T* find( T value )
	{
		for( int i=FirstUsed;i<ContentMaxCount;i++ )
			if( ContentVect[i] == value )
				return &ContentVect[i];
		return end();
	}
	__forceinline int size() { return ValuesUsed; }
	__forceinline int hasfree() { return FirstUsed; }
	__forceinline int CheckIntegrity() { return ( FirstUsed >= 0 && FirstUsed < ContentMaxCount && ContentMaxCount > 0 && ContentMaxCount < STATICPOINTERSET_STRANGE_SIZE && ValuesUsed <= ContentMaxCount && ValuesUsed > 0 ); }
private:
	T		*ContentVect;
	int		ContentMaxCount;
	int		ValuesUsed;
	int		FirstUsed;
};

enum CommitListActionTypes
{
	COMMITLIST_ACTION_ADD	= 1,
	COMMITLIST_ACTION_DEL	,
	COMMITLIST_ACTION_CLR	,
};

template <typename T> class RecursiveProtectedStaticSet
{
public:
	RecursiveProtectedStaticSet()
	{
		RecursionDepth = 0;
	}
	void UpdateMainSetChanges()
	{
		if( ChangesUncommitedSet.size() > 0 )
		{
			//Funky reverse traverse hack. Don't do this at home. Atm we can
			StaticPointerSetRev<T>::iterator itr=ChangesUncommitedSet.end();
			itr--;
			for( ;itr>=ChangesUncommitedSet.begin();itr--)
			{
				int ActionType = (int)(*itr);
				itr--;
				T	*Data = itr;
				if( ActionType == COMMITLIST_ACTION_ADD )
					UpdatedSet.insert( *Data );
				else if( ActionType == COMMITLIST_ACTION_DEL )
					UpdatedSet.erase( *Data );
			}
			ChangesUncommitedSet.clear();
		}
	}
	typedef		  T* iterator;
	__forceinline T* begin()	
	{ 
#if defined( ASSERT ) && defined( _DEBUG )
		ASSERT( UpdatedSet.CheckIntegrity() );
		ASSERT( ChangesUncommitedSet.CheckIntegrity() );
#endif
		return UpdatedSet.begin(); 
	}
	__forceinline T* end()		{ return UpdatedSet.end(); }
	__forceinline bool empty() 
	{
		return UpdatedSet.empty();
	}
	__forceinline void erase( T value )
	{
		if( RecursionDepth > 0 )
		{
			ChangesUncommitedSet.InsertAsList( (T)COMMITLIST_ACTION_DEL );
			ChangesUncommitedSet.InsertAsList( value );
		}
		else
			UpdatedSet.erase( value );
	}
	__forceinline void erase( iterator itr )
	{
		erase( *itr );
	}
	__forceinline void insert( T value )
	{
		if( RecursionDepth == 0 || UpdatedSet.hasfree() )
			UpdatedSet.insert( value );
		else
		{
			ChangesUncommitedSet.ReserveSize( 2 );
			ChangesUncommitedSet.InsertAsList( (T)COMMITLIST_ACTION_ADD );
			ChangesUncommitedSet.InsertAsList( value );
		}
	}
	__forceinline void clear()
	{
		UpdatedSet.clear();
		ChangesUncommitedSet.clear();
	}
	__forceinline T* find( T value )
	{
		return UpdatedSet.find( value );
	}
	__forceinline int size() { return UpdatedSet.size(); }
	__forceinline void RecursionAutoLockAquire()
	{
		RecursionDepth++;
	}
	__forceinline void RecursionAutoLockRelease( )
	{
		RecursionDepth--;
		if( RecursionDepth == 0 )
			UpdateMainSetChanges();
	}
	__forceinline int IsLockedForWrite()
	{
		return RecursionDepth;
	}
	__forceinline void EnsureSize( int Size )
	{
		UpdatedSet.ReserveSize( Size );
		ChangesUncommitedSet.ReserveSize( Size );
	}
private:
	int						RecursionDepth;			//if we are changing the set while others are iterating to it, make sure to not corrupt the iterator
	StaticPointerSetRev<T>	UpdatedSet;				//set that contains values added or removed in a way to not change iterator
	StaticPointerSetRev<T>	ChangesUncommitedSet;	//delay these actions until all loops exited iteration
};

template <typename T> class RecursiveProtectedStaticSet;

class SampleObject;

class RecursiveExitAutoCallback
{
public:
	RecursiveExitAutoCallback( SampleObject *Obj );
	RecursiveExitAutoCallback( );
//	__declspec(dllexport) __declspec(noinline) ~RecursiveExitAutoCallback();
	~RecursiveExitAutoCallback();
	SampleObject *CallbackOwner;
};

class SampleObject
{
public:
	__forceinline void RecursionAutoLockAquire()
	{
		Inrangeset.RecursionAutoLockAquire();
	}
	__forceinline void RecursionAutoLockRelease()
	{
		Inrangeset.RecursionAutoLockRelease();
	}
	RecursiveProtectedStaticSet<void*> Inrangeset;
private:
};

#endif