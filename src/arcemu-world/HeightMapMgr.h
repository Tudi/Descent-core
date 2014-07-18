#ifndef _HEIGHTMAPMGR_H_
#define _HEIGHTMAPMGR_H_

#include "Singleton.h"

// global class that might provide statistical height map for maps
// this will sample player movement and create maps according to that
// Right now it stores the upmost 2 floors of a map

#define VMAP_FLOOR_COUNT				1				//not really used but if it makes you feel better then it is fixed
#define VMAP_FLOOR_MIN_DISTANCE			4				//counted in yards, the closer the player is to this value it is considered to belong to that floor level
#define VMAP_FLOOR_MAX_DISTANCE			500				//Do not store coordonates if we felt through map
#define VMAP_SAVE_INTERVAL				1*60*60*1000	//for consistency map is dumped into file
#define VMAP_SAVE_MIN_VAL_COUNT			100				//avoid spamming save functions
#define VMAP_VALUE_NOT_INITIALIZED		( 0x00 )		//make sure it fits into float with all precissions
#define VMAP_SAFETY_ERROR_CORRECTION	1.5000001f		//better increase actual height of sample to not let people fall through ground
#define VMAP_SCALE_WOW_MAP_RES			3				//every N yards in wow is 1 cell for us
//if it gets larger then uint16 then you need to change struct also !
#define VMAP_VALUE_BOTTOM_OF_THE_OCEAN	-3000			//no way to have values like this in maps
#define VMAP_VALUE_FAR_IN_THE_SKY		3000			//no way to have values like this in maps
#define VMAP_CUR_VERSION				18				//mark in header if we can load this saved file or not
#define VMAP_SUBCELL_SIZE				100				//maps are divided into 2 levels, high level and low level
#define VMAP_MAX_MAP_SIZE				(40000/VMAP_SCALE_WOW_MAP_RES)		//the amount of info we can hold
#define VMAP_MAP_SIZE_HALF				(VMAP_MAX_MAP_SIZE/2)	//centered always
#define VMAP_MAX_CORD					(VMAP_MAX_MAP_SIZE)	//we cannot process values larger then this
//#define	VMAP_STABLE_STATISTIC_VALUE		0x7FFF			//just a high enough value to be able to say for sure that no errors were saved here
//if it gets larger then uint8 then you need to change struct also !
#define VMAP_VALUES_TO_SAMPLE_PER_FLOOR	(0x7FFF / (VMAP_VALUE_FAR_IN_THE_SKY*2) )	
#define VMAP_ACCEPTABLE_NEIGHBOUR_CHANGE	10			// on edges error might be larger. But on general flat areas it should be ok

//write this to file so we know which subcells we load
struct VMapSubCellMaskFile
{
	unsigned char SubCellMask[VMAP_MAX_MAP_SIZE/VMAP_SUBCELL_SIZE][VMAP_MAX_MAP_SIZE/VMAP_SUBCELL_SIZE];
};

// store actual value in a matrix of values
// 1 cell reprezents 1x1 yards in wow
// we use sum of values for error resiliance. If we get 1 false value due to some maphacker then it should get corrected by others walking on the same spot
struct VMapCell
{
	//signed short is -32000,32000 ->plenty for us
	signed short	floor_sums[VMAP_FLOOR_COUNT];			//low floor and higy floor
	unsigned char	floor_sums_count[VMAP_FLOOR_COUNT];
//	signed short	floor_quick_adapt[VMAP_FLOOR_COUNT];	//this always gets updated on sampling
};

//to be able to easelly read/write file
struct VMapHeader
{
	int32		m_version;		// if file has other version then content is dumped
	int32		m_cell_size;	// maybe cell changes and we would read ullshit
};

//actual subcell
//we have a main 50000x50000 map that we divide into smaller maps of 100x100
struct VMapSubCell
{
	VMapCell	SubCell[VMAP_SUBCELL_SIZE][VMAP_SUBCELL_SIZE];
};

//store values for only 1 map
//todo : implement error checking for map. If neighbour values are very off the scale then alert/remove
//todo : !! add read / write lock
class VMap
{
public:
	/* we are building stuff */
	VMap(uint32 map_id);
	/* destructor ! */
	~VMap();
	/* on each normal player movement samples are added to maps to get a statistically as best value as possible */
	void					AddSample(float x, float y, float z);
	/* beloved function to be used by emu to ask if mob/player could move to some specific location or not */
	float					GetHeight(float x, float y, float z=VMAP_VALUE_NOT_INITIALIZED);
	/* used by GM command to fix a specific area with bad height. Might happen for example in stair case events */
	void					ResetArea(float x, float y, int cells);
private:

	/*	Save map content to file. This is automatized and you do not need to call this */
	void					SaveMap();
	/*	Load map content from file. This happens on map creation. No need to call it */
	void					LoadMap(bool load_to_merge);
	/*	Try to search for additional maps in special folder to see if he could merge the 2 contents */
//	void					LoadMapMerge();
	/*	Frecvently called function to translate wow cords into our coordonate system */
	VMapCell				*getCellForWoWCords(float x, float y);
	char					IsValueAcceptable(float x, float y, float z);

	/*	Mostly used to not load incompatible map versions from file */ 
	VMapHeader				m_header;
	/*	Our GUID( actually mapmanager will make sure we are unique ) */
	uint32					m_map_id;
	/*	Do not spam file saving to not stress system. We save on intervals */
	uint32					m_next_save_stamp;
	/*	Save map if we have values to be saved */
	uint32					m_values_changed;
	/*	2 step cell storage system to reduce the amount of memory waisted on not used cells */
	VMapSubCell				*SubCellLookup[VMAP_MAX_MAP_SIZE/VMAP_SUBCELL_SIZE][VMAP_MAX_MAP_SIZE/VMAP_SUBCELL_SIZE];
};

#ifdef ENABLE_STATISTICS_GENERATED_HEIGHTMAP
// manage values for all the maps
// make sure that only 1 VMAP is created per map
class VMapMgr : public Singleton<VMapMgr>
{
public:
	VMapMgr();
	~VMapMgr();
	void					AddSample(uint32 map,float x, float y, float z);
	float					GetHeight(uint32 map,float x, float y, float z=VMAP_VALUE_BOTTOM_OF_THE_OCEAN);
	void					ResetArea(uint32 map,float x, float y, int cells);
private:
	std::map<uint32,VMap *> m_vmaps;
};

#define sSVMaps VMapMgr::getSingleton()
#endif

#endif