#include	"StaticRandom.h"
#include	<zlib.h>
#define		ARCEMU_SHARED_LIB
#include	"../arcemu-world/FeatureDefinitions.h"

#ifdef USE_STATIC_FILE_RANDOM_GEN

#define RANDOM_NUM_BUFF_SIZE	( (512 * 1024) - 1 )	// 512k covers around 18 bits of full random coverage
#define ZLIB_INPUT_BUFFER		4 * RANDOM_NUM_BUFF_SIZE
//#define RANDOMFILENAME			"RandomNumbers.zip"

uint8	*RandomNumbers;
uint32	RandomNumbersReadIndex;

uint8 * GenerateZlibBuff( uint32 &OutputSize )
{
	z_stream zlib_stream;
	uint8	*zlibOutBuff = (uint8*)malloc( ZLIB_INPUT_BUFFER );
	uint8	*zlibInBuff = (uint8*)malloc( ZLIB_INPUT_BUFFER );

	zlib_stream.zalloc = Z_NULL;
	zlib_stream.zfree  = Z_NULL;
	zlib_stream.opaque = Z_NULL;
	
	if( deflateInit(&zlib_stream, Z_BEST_COMPRESSION) != Z_OK )
		ASSERT( false );

	deflateReset( &zlib_stream );

	//fill input with whatever but not the same data
	for( uint32 i=0;i<ZLIB_INPUT_BUFFER/4;i++)
		*(uint32*)(&zlibInBuff[ i * 4 ]) = i;

	// set up zlib_stream pointers
	zlib_stream.next_out  = (Bytef*)zlibOutBuff;
	zlib_stream.avail_out = ZLIB_INPUT_BUFFER;
	zlib_stream.next_in   = (Bytef*)zlibInBuff;
	zlib_stream.avail_in  = ZLIB_INPUT_BUFFER;

	//deflate
	int ret = deflate(&zlib_stream, Z_FINISH); 

	//for some strange reason we did not manage to deflate it : destination buffer too small probably
	if( ret != Z_STREAM_END ) 
	{
		ASSERT( 0 );
		return 0;
	}

	ASSERT( zlib_stream.avail_out != 0 && zlib_stream.avail_in == 0 );

	//check if we got an even larger output then input. If so then we send it uncompressed
	if( zlib_stream.total_out + 4 > ZLIB_INPUT_BUFFER ) 
		return 0;

	//write it to file
/*	FILE *FOut = fopen( RANDOMFILENAME , "wb" );
	if( FOut )
	{
		fwrite( zlibOutBuff, 1, zlib_stream.total_out, FOut );
		fclose( FOut );
	}/**/
	OutputSize = zlib_stream.total_out;

	//cleanup
//	free( zlibOutBuff );
	free( zlibInBuff );
	return zlibOutBuff;
}
/*
int GetFileSize( FILE *fp )
{
	if( fp == NULL )
		return 0;
	fseek(fp, 0L, SEEK_END);
	int sz = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return sz;
}*/

void InitRandomNumberGenerators()
{
	//try to open random file
/*	FILE *FIn = fopen( RANDOMFILENAME, "rb" );
	if( FIn == NULL )
	{
		GenerateStaticFile();
		FIn = fopen( RANDOMFILENAME, "rb" );
	}
	if( FIn == NULL )
	{
		RandomNumbers = (uint8*)malloc( ZLIB_INPUT_BUFFER );
	}
	else
	{
		uint32 RandomNumbersSize = GetFileSize( FIn );
		RandomNumbers = (uint8*)malloc( RANDOM_NUM_BUFF_SIZE + 16 );
		if( RandomNumbersSize < RANDOM_NUM_BUFF_SIZE )
		{
			fread( RandomNumbers, 1, RandomNumbersSize, FIn );
			for( uint32 i = RandomNumbersSize; i < RANDOM_NUM_BUFF_SIZE; i++ )
				RandomNumbers[ i ] = RandomNumbers[ i % RandomNumbersSize ];
		}
		else
			fread( RandomNumbers, 1, RANDOM_NUM_BUFF_SIZE, FIn );
		RandomNumbersSize = RandomNumbersSize - 4;

		fclose( FIn );
	}*/
	{
		uint32 RandomNumbersSize;
		RandomNumbers = GenerateZlibBuff( RandomNumbersSize );
		//do we have enough data ? If not then duplicate what we have
		if( RandomNumbersSize < RANDOM_NUM_BUFF_SIZE )
		{
			assert( false );
			for( uint32 i = RandomNumbersSize; i < RANDOM_NUM_BUFF_SIZE; i++ )
				RandomNumbers[ i ] = RandomNumbers[ i % RandomNumbersSize ];
		}
	}
	//start at random location to avoid generating same numbers at every server startup
	//actually it would be good to be able to reproduce the same rolls 
	RandomNumbersReadIndex = GetTickCount() % ( RANDOM_NUM_BUFF_SIZE );
}

void CleanupRandomNumberGenerators()
{	
	free( RandomNumbers );
	RandomNumbers = NULL;
}

__forceinline uint32 RandomUIntInline()
{
	uint32 MyIndex = RandomNumbersReadIndex & RANDOM_NUM_BUFF_SIZE ; //if this is like 2^x-1 then it will work
	RandomNumbersReadIndex += 4; 
	return *(uint32*)(&RandomNumbers[MyIndex]);
}

__forceinline bool RandChance( uint32 Chance )
{
	uint32 MyIndex = RandomNumbersReadIndex & RANDOM_NUM_BUFF_SIZE ; //if this is like 2^x-1 then it will work
	RandomNumbersReadIndex += 1;
	//our interval is 0 - 255, chance is 0 - 100, we need to scale down ourself by ( 100 / 255 )
	if( (uint32)RandomNumbers[MyIndex] * 100 < Chance * 256 )
		return true;
	return false;
}

__forceinline bool RandChance( int32 Chance )
{
	uint32 MyIndex = RandomNumbersReadIndex & RANDOM_NUM_BUFF_SIZE; //if this is like 2^x-1 then it will work
	RandomNumbersReadIndex += 1; 
	//our interval is 0 - 255, chance is 0 - 100, we need to scale down ourself by ( 100 / 255 )
	if( RandomNumbers[MyIndex] * 100 < Chance * 256 )
		return true;
	return false;
}

__forceinline bool RandChance( float Chance )
{
	uint32 MyIndex = RandomNumbersReadIndex & RANDOM_NUM_BUFF_SIZE; //if this is like 2^x-1 then it will work
	RandomNumbersReadIndex += 1; 
	//our interval is 0 - 255, chance is 0 - 100, we need to scale down ourself by ( 100 / 255 )
//	if( RandomNumbers[MyIndex] * 100 < float2int32( Chance ) * 256 ) // 1373 MS
//	if( RandomNumbers[MyIndex] < float2int32( Chance * 2.56f ) ) // 1576
	if( (uint32)RandomNumbers[MyIndex] * 100 < uint32( Chance ) * 256 ) // 905 MS
		return true;
	return false;
}

double RandomDouble()
{
	// Output random float number in the interval 0 <= x < 1
	union 
	{
		double f; 
		uint32 i[2];
	} convert;
	uint32 r = RandomUIntInline();               // Get 32 random bits
	convert.i[0] =  r << 20;
	convert.i[1] = (r >> 12) | 0x3FF00000;
	return convert.f - 1.0;
}

uint32 RandomUInt(uint32 n)
{
	if( n == -1 )
		return 0;
	uint32 Ret = RandomUIntInline();
	return ( Ret % ( n + 1 ) );
}

double RandomDouble(double n)
{
	return RandomDouble() * n;
}

float RandomFloat()
{
	union FloatParts
	{
		float f;
		int32 i;
	};

	FloatParts value;
	uint32 RandomUi = RandomUIntInline();
	value.i = ((uint32)0x7F << 23) | (RandomUi & (uint32)0x7FFFFF);
	value.f -= 1.0;
	return value.f;
//	return float(RandomDouble());
}

float RandomFloat(float n)
{
	return RandomFloat() * n;
//	return float(RandomDouble() * double(n));
}

uint32 RandomUInt()
{
	return RandomUIntInline();
}

#endif