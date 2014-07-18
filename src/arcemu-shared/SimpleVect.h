#ifndef _SIMPLEVECT_H_
#define _SIMPLEVECT_H_

//The purpuse of this class is to reduce the amount memory used by the STL class
//note that the instances of this class are mostly used in a static way = created at startup
//elements will be pointers to structs holding data
//try to create the vector with enough space in it to avoid a lot of resizings
//elements should be created by NEW operator in case we use internal method of destroying list

template< class T > class SimpleVect
{
public:
	//empty vector. We resize it if added value does not fit in it
	SimpleVect(){ data = NULL; max_size = 0; next_free = 0; }
	//recon to use this. Vector is initialized with a size that we will use until destroyed.
	SimpleVect(const unsigned int psize )
	{ 
		data = NULL; 
		max_size = 0; 
		next_free = 0;
		Resize( psize );
	}
	~SimpleVect()
	{
		if( data )
		{
			next_free = 0;
			free( data );
			data = NULL ;
			max_size = 0;
		}
	}
	//set a value to an index
	inline void SetValue(const unsigned int pindex,const T value)
	{
		if( pindex >= max_size )
			Resize( pindex + 1 );
		if( pindex >= next_free )
			next_free = pindex + 1;
		data[ pindex ] = value;
	}
	//!!only use this if all values are inserted via this function
	inline void push_back(const T value)
	{
		if( next_free == max_size )
			Resize( next_free + 1 );
		data[ next_free++ ] = value;
	}
	inline void ResetPushPos() { next_free = 0; } //instead of deleting content we just say we have no values
	//get value of an index
	inline T GetValue(const unsigned int pindex )
	{
		if( pindex >= next_free )
			return NULL;
		return data[ pindex ];
	}
	//insert a value into a specific slot
/*	void InsertValueAtIndex(const unsigned int atindex,const T *value)
	{
		//we add a value so we probably need the slot for it
		Resize( max_size + 1 );
		//slide content to left to be able to insert it
//		for(unsigned int i=atindex; i < max_size - 1; i++)
		for(unsigned int i=max_size - 1; i > atindex; i--)
			data[ i ] = data[ i - 1 ];
		data[ atindex ] = value;
	}*/
	//delete a value from a specific slot and shift rest
	void DeleteValueAtIndex(const unsigned int atindex)
	{
		//slide content to left
		for(unsigned int i=atindex; i < max_size - 1; i++)
			data[ i ] = data[ i + 1];
		data[ max_size - 1 ] = NULL;
		if( next_free > 0 )
			next_free--;
	}
	//the last used element is before this index
	inline unsigned int GetMaxSize() { return next_free; }
//	inline unsigned int GetMaxVectSize() { return max_size; }
	//we delete pointers and set sizes to 0
	void DestroyContent()
	{
		if( data )
		{
			for(unsigned int i=0; i < next_free; i++)
				if( data[i] )
				{
					delete data[i];
					data[i] = NULL;
				}
			free( data );
			data = NULL ;
			max_size = 0;
			next_free = 0;
		}
	}
	//we only increase size if we want to add more elements and we do not have space for it
	inline void Resize(unsigned int new_size)
	{
		if( new_size <= max_size )
			return;	//we do not downscale the size of the list. This list is designed for simple static content
		T* tdata = (T*)realloc(data,new_size*sizeof(T)); //really hope this will not fail us :P
		//failed to allocate
		if( tdata == NULL )
		{
			if( data )
				free( data );
			data = NULL;
			return;
		}
		data = tdata;
		//since we resize very rarely we better make sure to have valid values in vector
		memset( &data[max_size],0,(new_size - max_size)*sizeof(T));
		max_size = new_size;
	}
private:
	unsigned int	next_free;	//!!only used by pushback
	unsigned int	max_size;	//total size allocated
	T				*data;
};

template< class T > class SimplePointerListNode
{
public:
	T						*data;
	SimplePointerListNode	*next;
	inline SimplePointerListNode<T>	*Next()
	{
		return next;
	}
};
//i realize the issues is with me but std::list is deleting objects when erasing iterator from lsit. Double deltes are bad ?
template< class T > class SimplePointerList
{
public:
	SimplePointerList(){ head = NULL; }
	~SimplePointerList()
	{
		clear();
	}
	inline void clear(int WithDelete=0)
	{
		while( head )
		{
			SimplePointerListNode<T>	*t_node = head;
			head = head->next;
			if( WithDelete && t_node->data )
				delete t_node->data;
			delete t_node;
		}
	}
	inline void push_front( T *value )
	{
		SimplePointerListNode<T>	*new_node = new SimplePointerListNode<T>;
		new_node->next = head;
		new_node->data = value;
		head = new_node;
	}
	inline void remove( const T *value, int DeleteData = 0 )
	{
		SimplePointerListNode<T>	*prev_node = NULL;
		SimplePointerListNode<T>	*itr = head;
		while( itr && itr->data != value )
		{
			prev_node = itr;
			itr = itr->next;
		}
		//could not find it
		if( itr == NULL )
			return;
		//not in first node. remove node 
		if( prev_node )
			prev_node->next = itr->next;
		else
			head = itr->next;
		if( DeleteData && itr->data )
			delete itr->data;
		delete itr;
	}
	//!! watch itr break when using this !
	inline void remove( SimplePointerListNode<T>	*delitr, int DeleteData )
	{
		SimplePointerListNode<T>	*prev_node = NULL;
		SimplePointerListNode<T>	*itr = head;
		while( itr && itr != delitr )
		{
			prev_node = itr;
			itr = itr->next;
		}
		//could not find it
		if( itr == NULL )
			return;
		//not in first node. remove node 
		if( prev_node )
			prev_node->next = itr->next;
		else
			head = itr->next;
		if( DeleteData && itr->data )
			delete itr->data;
		delete itr;
	}

	inline bool HasValues() { return (head != NULL); }
	inline SimplePointerListNode<T>	*begin() { return head; }
	inline SimplePointerListNode<T>	*end() { return NULL; }
private:
	SimplePointerListNode<T>	*head;
};

template< class T > class CommitPointerListNode
{
public:
	T						*data;
	CommitPointerListNode	*next;
	int						IsDeleted;
	inline CommitPointerListNode<T>	*Next()
	{
		CommitPointerListNode<T>	*ret = next;
		while( ret && ret->IsDeleted != 0 )
			ret = ret->next;
		return ret;
	}
};

#include "./Threading/Mutex.h"
// not designed for large lists
// list will delete the node pointer contents that was created with "new"
// general usage : function starts looping the list that calls other function that changes the list
// special usage : cross thread list changes
template< class T > class CommitPointerList
{
public:
	CommitPointerList(){ head = NULL; cyclic_call = 0; need_commit = 0;}
	~CommitPointerList()
	{
		clear( 1 );
	}
	void push_front( T *value )
	{
		CommitPointerListNode<T>	*new_node = new CommitPointerListNode<T>;
		new_node->next = head;
		new_node->data = value;
		new_node->IsDeleted = 0;
		head = new_node;
	}
	inline bool IsInLoop() { return (cyclic_call!=0); }
	inline void BeginLoop( ) { cyclic_call++; }
	inline void SafeClear( char WithDelete = 1 )
	{
		if( cyclic_call == 0 )
			clear( WithDelete );
		else
		{
			BeginLoop();
			CommitPointerListNode<T>	*t_node = head;
			while( t_node )
			{
				t_node->IsDeleted = WithDelete + 1;
				t_node = t_node->next;
			}
			need_commit = 1;
			EndLoop();
		}
	}
	inline void SafeRemove( CommitPointerListNode<T>	*itr, int DeleteObject )
	{
		itr->IsDeleted = DeleteObject + 1;
		need_commit = 1;
	}
	inline void SafeRemove( const T *value, int DeleteObject )
	{
		BeginLoop();
		CommitPointerListNode<T>	*prev_node = NULL;
		CommitPointerListNode<T>	*itr = head;
		while( itr && itr->data != value )
		{
			prev_node = itr;
			itr = itr->next;
		}
		EndLoop();
		//could not find it
		if( itr == NULL )
			return;
		need_commit = 1;
		itr->IsDeleted = DeleteObject + 1;
		Commit();
	}
	void Commit()
	{
		if( cyclic_call == 0 && need_commit != 0 )
		{
			cleanup_lock.Acquire();
			if( need_commit == 0 )
			{
				cleanup_lock.Release();
				return;
			}
			BeginLoop();
			need_commit = 0;
			CommitPointerListNode<T>	*prev_node = NULL;
			CommitPointerListNode<T>	*itr = head;
			while( itr )
			{
				if( itr->IsDeleted != 0 )
				{
					//not in first node. remove node 
					if( prev_node )
						prev_node->next = itr->next;
					else
						head = itr->next;
					//delete the data
					if( itr->IsDeleted > 1 )
						delete itr->data;
					//backup this itr so we can delete it later
					CommitPointerListNode<T>	*delme = itr;
					itr = itr->next;
					delete delme;
				}
				else
				{
					prev_node = itr;
					itr = itr->next;
				}
			}
			EndLoop();
			cleanup_lock.Release();
		}
	}
	inline void EndLoopAndCommit( )
	{
		cyclic_call--;
		Commit();
	}
	inline int size()
	{
		int counter = 0;
		CommitPointerListNode<T> *itr;
		BeginLoop();
		for( itr = begin(); itr != end(); itr = itr->Next() )
			counter++;
		EndLoopAndCommit();
		return counter;
	}
	inline char Empty() { return (head==NULL); }
	CommitPointerListNode<T>	*begin(){ return head; }
	CommitPointerListNode<T>	*end(){ return NULL; }
private:
	void clear(int WithDelete=0)
	{
		cleanup_lock.Acquire();
		if( cyclic_call == 0 )	//very bad if this is not 0, means we are deleting an object while iterating it's own list (or something like that )
		{
			while( head )
			{
				CommitPointerListNode<T>	*t_node = head;
				head = head->next;
				if( WithDelete && t_node->data )
					delete t_node->data;
				delete t_node;
			}
			need_commit = 0;
		}
		cleanup_lock.Release();
	}
	inline void EndLoop( ) { cyclic_call--; }

	CommitPointerListNode<T>	*head;
	int							cyclic_call;
	char						need_commit;
	//in very rare cases list needs to be thread safe
	Mutex						cleanup_lock;
};
#endif