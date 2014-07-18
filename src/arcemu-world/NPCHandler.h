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
#ifndef __NPCHANDLER_H
#define __NPCHANDLER_H

//safe way to handle arrays in debug mode
#include <array>

enum GOSSIP_SPECIAL
{
	GOSSIP_NO_SPECIAL		   = 0x00,
	GOSSIP_POI				  = 0x01,
	GOSSIP_SPIRIT_HEALER_ACTIVE = 0x02,
	GOSSIP_VENDOR			   = 0x03,
	GOSSIP_TRAINER			  = 0x04,
	GOSSIP_TABARD_VENDOR		= 0x05,
	GOSSIP_INNKEEPER			= 0x06,
	GOSSIP_PETITIONER		   = 0x07,
	GOSSIP_TAXI				 = 0x08,
	GOSSIP_TEXT				 = 0x09,
	GOSSIP_MENU				 = 0x0A,
	GOSSIP_BATTLEMASTER		 = 0x0B,
	GOSSIP_BANKEER			  = 0x0C,
};

enum Poi_Icon
{
    ICON_POI_BLANK              =   0,                      // Blank (not visible), in 2.4.3 have value 15 with 1..15 values in 0..14 range
    ICON_POI_GREY_AV_MINE       =   1,                      // Grey mine lorry
    ICON_POI_RED_AV_MINE        =   2,                      // Red mine lorry
    ICON_POI_BLUE_AV_MINE       =   3,                      // Blue mine lorry
    ICON_POI_BWTOMB             =   4,                      // Blue and White Tomb Stone
    ICON_POI_SMALL_HOUSE        =   5,                      // Small house
    ICON_POI_GREYTOWER          =   6,                      // Grey Tower
    ICON_POI_REDFLAG            =   7,                      // Red Flag w/Yellow !
    ICON_POI_TOMBSTONE          =   8,                      // Normal tomb stone (brown)
    ICON_POI_BWTOWER            =   9,                      // Blue and White Tower
    ICON_POI_REDTOWER           =   10,                     // Red Tower
    ICON_POI_BLUETOWER          =   11,                     // Blue Tower
    ICON_POI_RWTOWER            =   12,                     // Red and White Tower
    ICON_POI_REDTOMB            =   13,                     // Red Tomb Stone
    ICON_POI_RWTOMB             =   14,                     // Red and White Tomb Stone
    ICON_POI_BLUETOMB           =   15,                     // Blue Tomb Stone
    ICON_POI_16                 =   16,                     // Grey ?
    ICON_POI_17                 =   17,                     // Blue/White ?
    ICON_POI_18                 =   18,                     // Blue ?
    ICON_POI_19                 =   19,                     // Red and White ?
    ICON_POI_20                 =   20,                     // Red ?
    ICON_POI_GREYLOGS           =   21,                     // Grey Wood Logs
    ICON_POI_BWLOGS             =   22,                     // Blue and White Wood Logs
    ICON_POI_BLUELOGS           =   23,                     // Blue Wood Logs
    ICON_POI_RWLOGS             =   24,                     // Red and White Wood Logs
    ICON_POI_REDLOGS            =   25,                     // Red Wood Logs
    ICON_POI_26                 =   26,                     // Grey ?
    ICON_POI_27                 =   27,                     // Blue and White ?
    ICON_POI_28                 =   28,                     // Blue ?
    ICON_POI_29                 =   29,                     // Red and White ?
    ICON_POI_30                 =   30,                     // Red ?
    ICON_POI_GREYHOUSE          =   31,                     // Grey House
    ICON_POI_BWHOUSE            =   32,                     // Blue and White House
    ICON_POI_BLUEHOUSE          =   33,                     // Blue House
    ICON_POI_RWHOUSE            =   34,                     // Red and White House
    ICON_POI_REDHOUSE           =   35,                     // Red House
    ICON_POI_GREYHORSE          =   36,                     // Grey Horse
    ICON_POI_BWHORSE            =   37,                     // Blue and White Horse
    ICON_POI_BLUEHORSE          =   38,                     // Blue Horse
    ICON_POI_RWHORSE            =   39,                     // Red and White Horse
    ICON_POI_REDHORSE           =   40                      // Red Horse
};

enum TrainerServiceType
{
	TRAINER_SERVICE_AVAILABLE		=0x0,
	TRAINER_SERVICE_UNAVAILABLE	  =0x1,
	TRAINER_SERVICE_USED			 =0x2,
	TRAINER_SERVICE_NOT_SHOWN		=0x3,
	TRAINER_SERVICE_NEVER			=0x4,
	TRAINER_SERVICE_NO_PET		   =0x5
};
#define NUM_TRAINER_SERVICE_TYPES		0x6

enum TrainerType
{
	TRAINER_TYPE_GENERAL			 =0x0,
	TRAINER_TYPE_TALENTS			 =0x1,
	TRAINER_TYPE_TRADESKILLS		 =0x2,
	TRAINER_TYPE_PET				 =0x3
};
#define NUM_TRAINER_TYPES				0x4

#pragma pack(push,1)
struct GossipText_Text
{
	float Prob;
	//char * Text[2];
	tr1::array< char*, 2> Text;
	uint32 Lang;
	//uint32 Emote[6];
	tr1::array< uint32, 6> Emote;
};

struct GossipText
{
	uint32 ID;
	//GossipText_Text Texts[8];
	tr1::array< GossipText_Text, 8> Texts;
};
#pragma pack(pop)

struct GossipOptions
{
	uint32 ID;
	uint32 GossipID;
	uint16 Icon;
	std::string OptionText;
	uint32 NextTextID;
	uint32 Special;
	float PoiX;
	float PoiY;
	uint32 PoiIcon;
	uint32 PoiFlags;
	uint32 PoiData;
	std::string PoiName;
	uint32 BgMapId;
};

struct GossipNpc
{
	GossipNpc() { pOptions = NULL;  }
	uint32 ID;
	uint32 EntryId;
	uint32 TextID;
	uint32 OptionCount;
	GossipOptions *pOptions;
};

struct trainertype
{
	const char* partialname;
	uint32 type;
};

#define TRAINER_TYPE_MAX 17
extern trainertype trainer_types[TRAINER_TYPE_MAX];

#define TRAINER_STATUS_LEARNABLE		1
#define TRAINER_STATUS_NOT_LEARNABLE	2
#define TRAINER_STATUS_ALREADY_HAVE		0

#endif
