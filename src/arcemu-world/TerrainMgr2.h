#ifndef _TERRAINMGR2_H_
#define _TERRAINMGR2_H_

#include "Singleton.h"

//!!!! ALL THANKS TO MANGOS TEAM !!!! ( converted version from wowd ;) )

//old terrain manager was bound to maps. We wish to be able to interrogate height from anywhere not just maps
//we may bind map to mapterrain if we wish

#define MAX_MAP_ID				 990		//this should be larger then realm MAX_MAPID

#define MAX_NUMBER_OF_GRIDS      64

#define SIZE_OF_GRIDS            533.33333f
#define CENTER_GRID_ID           (MAX_NUMBER_OF_GRIDS/2)

#define CENTER_GRID_OFFSET      (SIZE_OF_GRIDS/2)

#define MAX_NUMBER_OF_CELLS     8
#define SIZE_OF_GRID_CELL       (SIZE_OF_GRIDS/MAX_NUMBER_OF_CELLS)

#define CENTER_GRID_CELL_ID     (MAX_NUMBER_OF_CELLS*MAX_NUMBER_OF_GRIDS/2)
#define CENTER_GRID_CELL_OFFSET (SIZE_OF_GRID_CELL/2)

#define TOTAL_NUMBER_OF_CELLS_PER_MAP    (MAX_NUMBER_OF_GRIDS*MAX_NUMBER_OF_CELLS)

#define MAP_RESOLUTION			128

#define MAP_SIZE                (SIZE_OF_GRIDS*MAX_NUMBER_OF_GRIDS)
#define MAP_HALFSIZE            (MAP_SIZE/2)

//******************************************
// Map file format defines
//******************************************
struct map_fileheader
{
    uint32 mapMagic;
    uint32 versionMagic;
    uint32 buildMagic;
    uint32 areaMapOffset;
    uint32 areaMapSize;
    uint32 heightMapOffset;
    uint32 heightMapSize;
    uint32 liquidMapOffset;
    uint32 liquidMapSize;
};

#define MAP_AREA_NO_AREA      0x0001

struct map_areaHeader
{
    uint32 fourcc;
    uint16 flags;
    uint16 gridArea;
};

#define MAP_HEIGHT_NO_HEIGHT  0x0001
#define MAP_HEIGHT_AS_INT16   0x0002
#define MAP_HEIGHT_AS_INT8    0x0004

struct map_heightHeader
{
    uint32 fourcc;
    uint32 flags;
    float  gridHeight;
    float  gridMaxHeight;
};

#define MAP_LIQUID_NO_TYPE    0x0001
#define MAP_LIQUID_NO_HEIGHT  0x0002

struct map_liquidHeader
{
    uint32 fourcc;
    uint16 flags;
    uint16 liquidType;
    uint8  offsetX;
    uint8  offsetY;
    uint8  width;
    uint8  height;
    float  liquidLevel;
};

enum ZLiquidStatus
{
    LIQUID_MAP_NO_WATER     = 0x00000000,
    LIQUID_MAP_ABOVE_WATER  = 0x00000001,
    LIQUID_MAP_WATER_WALK   = 0x00000002,
    LIQUID_MAP_IN_WATER     = 0x00000004,
    LIQUID_MAP_UNDER_WATER  = 0x00000008
};

#define MAP_LIQUID_TYPE_NO_WATER    0x00
#define MAP_LIQUID_TYPE_WATER       0x01
#define MAP_LIQUID_TYPE_OCEAN       0x02
#define MAP_LIQUID_TYPE_MAGMA       0x04
#define MAP_LIQUID_TYPE_SLIME       0x08

#define MAP_ALL_LIQUIDS   (MAP_LIQUID_TYPE_WATER | MAP_LIQUID_TYPE_OCEAN | MAP_LIQUID_TYPE_MAGMA | MAP_LIQUID_TYPE_SLIME)

#define MAP_LIQUID_TYPE_DARK_WATER  0x10
#define MAP_LIQUID_TYPE_WMO_WATER   0x20

#define MAX_HEIGHT					100000.0f              // can be use for find ground height at surface
#define INVALID_HEIGHT				-100000.0f             // for check, must be equal to VMAP_INVALID_HEIGHT, real value for unknown height is VMAP_INVALID_HEIGHT_VALUE

struct LiquidData
{
    uint32 type;
    float  level;
    float  depth_level;
};

class GridMap
{
    uint32  m_flags;
    // Area data
    uint16  m_gridArea;
    uint16 *m_area_map;
    // Height level data
    float   m_gridHeight;
    float   m_gridIntHeightMultiplier;
    union{
        float  *m_V9;
        uint16 *m_uint16_V9;
        uint8  *m_uint8_V9;
    };
    union{
        float  *m_V8;
        uint16 *m_uint16_V8;
        uint8  *m_uint8_V8;
    };
    // Liquid data
    uint16  m_liquidType;
    uint8   m_liquid_offX;
    uint8   m_liquid_offY;
    uint8   m_liquid_width;
    uint8   m_liquid_height;
    float   m_liquidLevel;
    uint8  *m_liquid_type;
    float  *m_liquid_map;

    bool  loadAreaData(FILE *in, uint32 offset, uint32 size);
    bool  loadHeightData(FILE *in, uint32 offset, uint32 size);
    bool  loadLiquidData(FILE *in, uint32 offset, uint32 size);

    // Get height functions and pointers
    typedef float (GridMap::*pGetHeightPtr) (float x, float y) const;
    pGetHeightPtr m_gridGetHeight;
    float  getHeightFromFloat(float x, float y) const;
    float  getHeightFromUint16(float x, float y) const;
    float  getHeightFromUint8(float x, float y) const;
    float  getHeightFromFlat(float x, float y) const;

public:
    GridMap();
    ~GridMap();
    bool  loadData(char *filaname);
    void  unloadData();

    uint16 getArea(float x, float y);
    inline float getHeight(float x, float y) {return (this->*m_gridGetHeight)(x, y);}
    float  getLiquidLevel(float x, float y);
    uint8  getTerrainType(float x, float y);
    ZLiquidStatus getLiquidStatus(float x, float y, float z, uint8 ReqLiquidType, LiquidData *data = 0);
};

class MapTerrain2
{
public:
	MapTerrain2(uint32 MapId);		//we will precache grids here
	~MapTerrain2();
	float			GetHeight(float x, float y, float z);
	float			GetLiquidHeight(float x, float y, float z);
	uint8			GetLiquidFlags(float x, float y, float z);
	ZLiquidStatus	GetLiquidStatus(float x, float y, float z);
	uint32			GetAreaID(float x, float y, float z);
private:
	GridMap			*GetGrid(float x, float y);
	GridMap			*GridMaps[MAX_NUMBER_OF_GRIDS][MAX_NUMBER_OF_GRIDS];
	uint32			m_MapId;
};

//NGridType* i_grids[MAX_NUMBER_OF_GRIDS][MAX_NUMBER_OF_GRIDS];
//std::bitset<TOTAL_NUMBER_OF_CELLS_PER_MAP*TOTAL_NUMBER_OF_CELLS_PER_MAP> marked_cells;

// manage values for all the maps
// make sure that only 1 VMAP is created per map
class TerrainMgr2 : public Singleton<TerrainMgr2>
{
public:
	TerrainMgr2();			//we will precache maps here :)
	~TerrainMgr2();
	float					GetHeight(uint32 map,float x, float y, float z);
	float					GetLiquidHeight(uint32 map,float x, float y, float z);
	uint8					GetLiquidFlags(uint32 map,float x, float y, float z);
	ZLiquidStatus			GetLiquidStatus(uint32 map,float x, float y, float z);
	uint32					GetAreaID(uint32 map,float x, float y, float z);
	void					EventMapCreated(uint32 mapid);
private:
	std::map<uint32,MapTerrain2 *> m_terrains;
	Mutex					LoadLock;
};

#define sTerrainMgr TerrainMgr2::getSingleton()
#endif