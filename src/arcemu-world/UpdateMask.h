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

#ifndef __UPDATEMASK_H
#define __UPDATEMASK_H

class UpdateMask
{
	union {
		uint32	*mUpdateMask;
		uint8	*mUpdateMask8;
	};
	uint32 mCount; // in values
	uint32 mBlocks; // in uint32 blocks

public:
	UpdateMask( ) : mUpdateMask( 0 ), mCount( 0 ), mBlocks( 0 ) { }
	UpdateMask( const UpdateMask& mask ) : mUpdateMask( 0 ) { *this = mask; }

	~UpdateMask( )
	{
		if(mUpdateMask)
		{
			delete [] mUpdateMask;
			mUpdateMask = NULL;
		}
	}

	void SetBit( const uint32 index )
	{
		ASSERT(index < mCount);
		mUpdateMask8[ index >> 3 ] |= 1 << ( index & 0x7 );
		// mUpdateMask8[ index / 8 ] |= 1 * pow( 2, index % 8 );
	}

	void UnsetBit( const uint32 index )
	{
		ASSERT(index < mCount);
		mUpdateMask8[ index >> 3 ] &= (0xff ^ (1 <<  ( index & 0x7 ) ) );
		// mUpdateMask8[ index / 8 ] &= 255 - ( 1 * pow( 2, index % 8 ) ) );
	}

	uint8 GetBit( const uint32 index ) const
	{
		ASSERT(index < mCount);
		return ( mUpdateMask8[ index >> 3 ] & ( 1 << ( index & 0x7 ) ));
		//actually int->bool conversion is not needed here
	}

	uint8 GetBit( const uint32 index, const uint8 *vis_mask ) const
	{
		ASSERT(index < mCount);
		int ind = index >> 3;
		return ( (mUpdateMask8[ ind ] & vis_mask[ ind ]) & ( 1 << ( index & 0x7 ) ));
		//actually int->bool conversion is not needed here
	}

	//note that mask 2 must have same length !
	uint8 GetBitWithMask( const uint32 index,const uint8 *mask2 ) const
	{
		ASSERT(index < mCount);
		uint32 index2 = index >> 3;
		return ( mUpdateMask8[ index2 ] & mask2[ index2 ] & ( 1 << ( index & 0x7 ) ));
	}

	uint32 GetBlock( const uint32 index ) const
	{
		ASSERT(index < mBlocks);
		return mUpdateMask[ index ];
		//actually int->bool conversion is not needed here
	}

	uint32 GetUpdateBlockCount() const 
	{
		int32 x;
		for( x=mBlocks-1;x>=0;x--)
			if(mUpdateMask[x])
				return (x+1);
		return 0; 
	}
	uint32 GetUpdateBlockCount(uint32 *visibility_mask) const 
	{
		int32 x;
		for( x=mBlocks-1;x>=0;x--)
			if( mUpdateMask[x] & visibility_mask[x] )
				return (x+1);
		return 0; 
	}	ARCEMU_INLINE uint32 GetBlockCount() const {return mBlocks;}
	
	ARCEMU_INLINE uint32 GetLength() const { return (mBlocks *sizeof(uint32)); }
	ARCEMU_INLINE uint32 GetCount() const { return mCount; }
	ARCEMU_INLINE const uint8* GetMask() const { return (uint8*)mUpdateMask; }

	void SetCount(uint32 valuesCount)
	{
		if(mUpdateMask)
			delete [] mUpdateMask;

		mCount = valuesCount;
		//mBlocks = valuesCount/32 + 1;
		//mBlocks = (valuesCount + 31) / 32;
		//mBlocks = mCount >> 5;
		//if(mCount & 31)
		//	++mBlocks;
		mBlocks = ( mCount + 31 ) >> 5;

		mUpdateMask = new uint32[mBlocks];
		memset(mUpdateMask, 0, mBlocks * sizeof(uint32));
	}

	void Clear()
	{
		if (mUpdateMask)
			memset(mUpdateMask, 0, mBlocks << 2);
	}

	UpdateMask& operator = ( const UpdateMask& mask )
	{
		SetCount(mask.mCount);
		memcpy(mUpdateMask, mask.mUpdateMask, mBlocks << 2);

		return *this;
	}

	void operator &= ( const UpdateMask& mask )
	{
		ASSERT(mask.mCount <= mCount);
		for(uint32 i = 0; i < mBlocks; i++)
			mUpdateMask[i] &= mask.mUpdateMask[i];
	}

	void operator |= ( const UpdateMask& mask )
	{
		ASSERT(mask.mCount <= mCount);
		for(uint32 i = 0; i < mBlocks; i++)
			mUpdateMask[i] |= mask.mUpdateMask[i];
	}
	void Inherit( const UpdateMask *mask )
	{
		ASSERT(mask->mCount <= mCount);
		for(uint32 i = 0; i < mBlocks; i++)
			mUpdateMask[i] |= mask->mUpdateMask[i];
	}

	UpdateMask operator & ( const UpdateMask& mask ) const
	{
		ASSERT(mask.mCount <= mCount);

		UpdateMask newmask;
		newmask = *this;
		newmask &= mask;

		return newmask;
	}

	UpdateMask operator | ( const UpdateMask& mask ) const
	{
		ASSERT(mask.mCount <= mCount);

		UpdateMask newmask;
		newmask = *this;
		newmask |= mask;

		return newmask;
	}
};

#endif

