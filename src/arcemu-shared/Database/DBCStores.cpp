/*
 * ArcEmu MMORPG Server
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

#include "DBCStores.h"
#include "DataStore.h"
#include "NGLog.h"

SERVER_DECL DBCStorage <CharSectionsEntry>			dbcCharSections;
SERVER_DECL DBCStorage <CharacterFacialHairStylesEntry>dbcCharacterFacialHairStyles;

SERVER_DECL DBCStorage <MountCapabilityEntry>		dbcMountCapabilityStore;
SERVER_DECL DBCStorage <MountTypeEntry>				dbcMountTypeStore;

SERVER_DECL DBCStorage <ArmorLocationEntry>         dbcArmorLocationStore;
SERVER_DECL DBCStorage <ItemArmorQualityEntry>      dbcItemArmorQualityStore;
SERVER_DECL DBCStorage <ItemArmorShieldEntry>       dbcItemArmorShieldStore;
SERVER_DECL DBCStorage <ItemArmorTotalEntry>        dbcItemArmorTotalStore;
SERVER_DECL DBCStorage <ItemDamageEntry>            dbcItemDamageAmmoStore;
SERVER_DECL DBCStorage <ItemDamageEntry>            dbcItemDamageOneHandStore;
SERVER_DECL DBCStorage <ItemDamageEntry>            dbcItemDamageOneHandCasterStore;
SERVER_DECL DBCStorage <ItemDamageEntry>            dbcItemDamageRangedStore;
SERVER_DECL DBCStorage <ItemDamageEntry>            dbcItemDamageThrownStore;
SERVER_DECL DBCStorage <ItemDamageEntry>            dbcItemDamageTwoHandStore;
SERVER_DECL DBCStorage <ItemDamageEntry>            dbcItemDamageTwoHandCasterStore;
SERVER_DECL DBCStorage <ItemDamageEntry>            dbcItemDamageWandStore;

SERVER_DECL DBCStorage <LFGDungeonEntry>			dbcLFGDungeonStore;
SERVER_DECL DBCStorage <WorldMapAreaEntry>			dbcWorldMapZoneStore;
SERVER_DECL DBCStorage <VehicleSeatEntry>			dbcVehicleSeatEntry;
SERVER_DECL DBCStorage <VehicleEntry>				dbcVehicleEntry;
//SERVER_DECL DBCStorage<CurrencyTypesEntry>			dbcCurrencyTypesStore;
SERVER_DECL DBCStorage<ScalingStatDistributionEntry> dbcScalingStatDistribution;
SERVER_DECL DBCStorage<ScalingStatValuesEntry>		dbcScalingStatValues;
SERVER_DECL DBCStorage<WorldMapOverlay>				dbcWorldMapOverlayStore;
SERVER_DECL DBCStorage<BattlemasterListEntry>		dbcBattlemasterListStore;
SERVER_DECL DBCStorage<ItemLimitCategory>			dbcItemLimitCategory;
SERVER_DECL DBCStorage<AreaGroup>					dbcAreaGroup;
SERVER_DECL DBCStorage<GlyphPropertiesEntry>        dbcGlyphPropertiesStore;
SERVER_DECL DBCStorage<GlyphSlotEntry>              dbcGlyphSlotStore;
SERVER_DECL std::list<AchievementCriteriaEntry*>	dbcAchievementCriteriaTypeLookup[ACHIEVEMENT_CRITERIA_TYPE_TOTAL];
SERVER_DECL DBCStorage<AchievementEntry>			dbcAchievementStore;
SERVER_DECL DBCStorage<AchievementCriteriaEntry>	dbcAchievementCriteriaStore;
SERVER_DECL DBCStorage<BarberShopStyleEntry>		dbcBarberShopStyleStore;
SERVER_DECL DBCStorage<GtBarberShopCostBaseEntry>	dbcBarberShopCostStore;
SERVER_DECL DBCStorage<GemPropertyEntry>			dbcGemProperty;
SERVER_DECL DBCStorage<ItemSetEntry>				dbcItemSet;
SERVER_DECL DBCStorage<Lock>						dbcLock;

SERVER_DECL DBCStorage<SpellRuneCostEntry>			dbcSpellRuneCostEntry;
SERVER_DECL DBCStorage<SpellRadius>					dbcSpellRadius;
SERVER_DECL DBCStorage<SpellCastTime>				dbcSpellCastTime;
SERVER_DECL DBCStorage<SpellEntry>					dbcSpell;
SERVER_DECL DBCStorage<SpellDuration>				dbcSpellDuration;
SERVER_DECL DBCStorage<SpellRange>					dbcSpellRange;
SERVER_DECL DBCStorage<SpellEffectEntry>			dbcSpellEffect;
SERVER_DECL DBCStorage<SpellDifficultyEntry>		dbcSpellDifficulty;
SERVER_DECL DBCStorage<SpellFocusObjectEntry>		dbcSpellFocusObject;
//SERVER_DECL DBCStorage<SpellItemEnchantmentConditionEntry>	dbcSpellItemEnchantmentCondition;
//SERVER_DECL SpellCategoryStore					dbcSpellCategory;
SERVER_DECL DBCStorage<SpellShapeshiftFormEntry>	dbcSpellShapeshiftForm;
SERVER_DECL DBCStorage<SpellAuraOptionsEntry>		dbcSpellAuraOptions;
SERVER_DECL DBCStorage<SpellAuraRestrictionsEntry>	dbcSpellAuraRestrictions;
SERVER_DECL DBCStorage<SpellCastingRequirementsEntry>	dbcSpellCastingRequirements;
SERVER_DECL DBCStorage<SpellCategoriesEntry>		dbcSpellCategories;
SERVER_DECL DBCStorage<SpellClassOptionsEntry>		dbcSpellClassOptions;
SERVER_DECL DBCStorage<SpellCooldownsEntry>			dbcSpellCooldowns;
SERVER_DECL DBCStorage<SpellEquippedItemsEntry>		dbcSpellEquippedItems;
SERVER_DECL DBCStorage<SpellInterruptsEntry>		dbcSpellInterrupts;
SERVER_DECL DBCStorage<SpellLevelsEntry>			dbcSpellLevels;
SERVER_DECL DBCStorage<SpellPowerEntry>				dbcSpellPower;
SERVER_DECL DBCStorage<SpellReagentsEntry>			dbcSpellReagents;
SERVER_DECL DBCStorage<SpellScalingEntry>			dbcSpellScaling;
SERVER_DECL DBCStorage<SpellShapeshiftEntry>		dbcSpellShapeshift;
SERVER_DECL DBCStorage<SpellTargetRestrictionsEntry>	dbcSpellTargetRestrictions;
SERVER_DECL DBCStorage<SpellTotemsEntry>			dbcSpellTotems;
SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcGTSpellScale;

SERVER_DECL DBCStorage<ResearchBranchEntry>			dbcResearchBranch;
SERVER_DECL DBCStorage<ResearchProjectEntry>		dbcResearchProject;
SERVER_DECL DBCStorage<ResearchSiteEntry>			dbcResearchSite;
SERVER_DECL DBCStorage<GuildPerkSpellEntry>			dbcGuildPerks;
SERVER_DECL DBCStorage<emoteentry>					dbcEmoteEntry;
SERVER_DECL DBCStorage<AreaTable>					dbcArea;
SERVER_DECL DBCStorage<FactionTemplateDBC>			dbcFactionTemplate;
SERVER_DECL DBCStorage<FactionDBC>					dbcFaction;
SERVER_DECL DBCStorage<EnchantEntry>				dbcEnchant;
SERVER_DECL DBCStorage<RandomProps>					dbcRandomProps;
SERVER_DECL DBCStorage<skilllinespell>				dbcSkillLineSpell;
SERVER_DECL DBCStorage<skilllineentry>				dbcSkillLine;
SERVER_DECL DBCStorage<DBCTaxiNode>					dbcTaxiNode;
SERVER_DECL DBCStorage<DBCTaxiPath>					dbcTaxiPath;
SERVER_DECL DBCStorage<DBCTaxiPathNode>				dbcTaxiPathNode;
SERVER_DECL DBCStorage<AuctionHouseDBC>				dbcAuctionHouse;
SERVER_DECL DBCStorage<TalentEntry>					dbcTalent;
SERVER_DECL DBCStorage<TalentTabEntry>				dbcTalentTab;
SERVER_DECL DBCStorage<TalentPrimarySpellsEntry>	dbcTalentPrimarySpells;
SERVER_DECL DBCStorage<CreatureSpellDataEntry>		dbcCreatureSpellData;
SERVER_DECL DBCStorage<CreatureFamilyEntry>			dbcCreatureFamily;
SERVER_DECL DBCStorage<CharClassEntry>				dbcCharClass;
SERVER_DECL DBCStorage<CharRaceEntry>				dbcCharRace;
SERVER_DECL DBCStorage<MapEntry>					dbcMap;
SERVER_DECL DBCStorage<ItemExtendedCostEntry>		dbcItemExtendedCost;
SERVER_DECL DBCStorage<ItemRandomSuffixEntry>		dbcItemRandomSuffix;
SERVER_DECL DBCStorage<RandomPropertiesPointsEntry>	dbcRandomPropertyPoints;
SERVER_DECL DBCStorage<ItemReforgeEntry>			dbcItemReforge;
SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcCombatRating;
SERVER_DECL DBCStorage<ChatChannelDBC>				dbcChatChannels;
SERVER_DECL DBCStorage<DurabilityQualityEntry>		dbcDurabilityQuality;
SERVER_DECL DBCStorage<DurabilityCostsEntry>		dbcDurabilityCosts;
SERVER_DECL DBCStorage<BankSlotPrice>				dbcBankSlotPrices;
//SERVER_DECL DBCStorage<BankSlotPrice>				dbcStableSlotPrices;
SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcMeleeCrit;
SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcMeleeCritBase;
SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcSpellCrit;
SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcSpellCritBase;
SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcMPRegen;
SERVER_DECL DBCStorage<gtClassLevelFloat>			dbcMPRegenBase;
//SERVER_DECL DBCStorage<gtFloat>						dbcHPRegen;
SERVER_DECL DBCStorage<gtFloat>						dbcHPRegenBase;
SERVER_DECL DBCStorage<AreaTriggerEntry>			dbcAreaTrigger;
SERVER_DECL DBCStorage<QuestXPEntry>				dbcQuestXP;
SERVER_DECL DBCStorage<QuestRewRepEntry>			dbcQuestRewRep;
SERVER_DECL DBCStorage<CharTitlesEntry>				dbcCharTitle;
SERVER_DECL DBCStorage<WorldSafeLocsEntry>			dbcWorldSafeLocs; 
SERVER_DECL DBCStorage<WMOAreaTableEntry>			dbcWMOAreaTable;

const char* VehicleEntryFormat = "uuffffuuuuuuuufffffffffffffffssssfufuxxx";
const char* areagroupFormat = "uuuuuuuu";
const char* SpellRuneCostFormat="uuuuu";
const char* GlyphPropertiesFormat="niii";
const char* GlyphSlotFormat="nii";
const char* LockFormat = "uuuuuuxxxuuuuuxxxuuuuuxxxxxxxxxxx";
const char* spellcasttimeFormat = "uuxx";
const char* spellradiusFormat = "ufxf";
const char* spelldurationFormat = "uuuu";
const char* factiontemplatedbcFormat = "uuuuuuuuuuuuuu";
const char* creaturespelldataFormat = "uuuuuuuuu";
const char * durabilityqualityFormat = "uf";
const char * durabilitycostsFormat = "uuuuuuuuuuuuuuuuuuuuuuuuuuuuuu";
const char* bankslotpriceformat = "uu";
//const char* gtfloatformat = "f";
const char* gtClassfloatformat = "uf";
const char* questxpformat = "uxuuuuuuuux";
const char* questrewrepformat = "ixiiiiiiiii";

template<class T>
bool loader_stub(const char * filename, const char * format, bool ind, T& l, bool loadstrs)
{
	Log.Notice("DBC", "Loading %s.", filename);
	return l.Load(filename, format, ind, loadstrs);	//just to let you know. Format can have 3 values : x(skip),s(str),?(val)
}

#define LOAD_DBC(filename, format, ind, stor, strings) if(!loader_stub(filename, format, ind, stor, strings)) { return false; } 

bool LoadDBCs()
{
	printf("mem usage : %f\n",GetMemUsage() );

	const char* charsectionsformat = "xuuusxxxxu";
	LOAD_DBC("DBC/CharSections.dbc", charsectionsformat, false, dbcCharSections, true);
	printf("mem usage : %f\n",GetMemUsage() );
//	const char* CharacterFacialHairStylesformat = "xuuuxxxxx";
//	LOAD_DBC("DBC/CharacterFacialHairStyles.dbc", CharacterFacialHairStylesformat, false, dbcCharacterFacialHairStyles, false);
//	printf("mem usage : %f\n",GetMemUsage() );

//	const char* wmoareaformat = "uiiixxxxxuuxxxxxxxxxxxxxxxxx";
	const char* wmoareaformat = "uiiixxxxxuuxxxx";
	LOAD_DBC("DBC/WMOAreaTable.dbc", wmoareaformat, true, dbcWMOAreaTable, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char dbcWorldsafelocsfmt[]="iifffs";
	LOAD_DBC("DBC/WorldSafeLocs.dbc",dbcWorldsafelocsfmt,true,dbcWorldSafeLocs, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char dbcMountCapabilityStorefmt[]="iiixxiii";
	LOAD_DBC("DBC/MountCapability.dbc",dbcMountCapabilityStorefmt,false,dbcMountCapabilityStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char dbcMountTypeStorefmt[]="iiiiiiiiiiiiiiiiiixxxxxxx";
	LOAD_DBC("DBC/MountType.dbc",dbcMountTypeStorefmt,false,dbcMountTypeStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char dbcResearchBranchfmt[]="usuuxu";
	LOAD_DBC("DBC/ResearchBranch.dbc",dbcResearchBranchfmt,false,dbcResearchBranch, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char dbcResearchProjectfmt[]="usxxuuuxu";
	LOAD_DBC("DBC/ResearchProject.dbc",dbcResearchProjectfmt,false,dbcResearchProject, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char dbcResearchSitefmt[]="uuusu";
	LOAD_DBC("DBC/ResearchSite.dbc",dbcResearchSitefmt,false,dbcResearchSite, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char GuildPerkfmt[]="uuu";
	LOAD_DBC("DBC/GuildPerkSpells.dbc",GuildPerkfmt,false,dbcGuildPerks, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char ArmorLocationfmt[]="nfffff";
	LOAD_DBC("DBC/ArmorLocation.dbc",ArmorLocationfmt,false,dbcArmorLocationStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char ItemArmorQualityfmt[]="nfffffffi";
	LOAD_DBC("DBC/ItemArmorQuality.dbc",ItemArmorQualityfmt,false,dbcItemArmorQualityStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char ItemArmorShieldfmt[]="nifffffff";
	LOAD_DBC("DBC/ItemArmorShield.dbc",ItemArmorShieldfmt,false,dbcItemArmorShieldStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char ItemArmorTotalfmt[]="niffff";
	LOAD_DBC("DBC/ItemArmorTotal.dbc",ItemArmorTotalfmt,false,dbcItemArmorTotalStore, false);

	printf("mem usage : %f\n",GetMemUsage() );
	const char ItemDamagefmt[]="nfffffffi";
	LOAD_DBC("DBC/ItemDamageAmmo.dbc",ItemDamagefmt,false,dbcItemDamageAmmoStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/ItemDamageOneHand.dbc",ItemDamagefmt,false,dbcItemDamageOneHandStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/ItemDamageOneHandCaster.dbc",ItemDamagefmt,false,dbcItemDamageOneHandCasterStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/ItemDamageRanged.dbc",ItemDamagefmt,false,dbcItemDamageRangedStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/ItemDamageThrown.dbc",ItemDamagefmt,false,dbcItemDamageThrownStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/ItemDamageTwoHand.dbc",ItemDamagefmt,false,dbcItemDamageTwoHandStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/ItemDamageTwoHandCaster.dbc",ItemDamagefmt,false,dbcItemDamageTwoHandCasterStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/ItemDamageWand.dbc",ItemDamagefmt,false,dbcItemDamageWandStore, false);

	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellEffectEntryfmt[]="xufuuiffiiiiiifiifiiiiiiiix";
	LOAD_DBC("DBC/SpellEffect.dbc",SpellEffectEntryfmt,false,dbcSpellEffect, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellShapeshiftFormfmt[]="uxxiixiiixxiiiiiiiixx";
	LOAD_DBC("DBC/SpellShapeshiftForm.dbc",SpellShapeshiftFormfmt,false,dbcSpellShapeshiftForm, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellShapeshiftEntryfmt[]="uixixx";
	LOAD_DBC("DBC/SpellShapeshift.dbc",SpellShapeshiftEntryfmt,false,dbcSpellShapeshift, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellAuraOptionsEntryfmt[]="uuuuu";
	LOAD_DBC("DBC/SpellAuraOptions.dbc",SpellAuraOptionsEntryfmt,false,dbcSpellAuraOptions, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellAuraRestrictionsEntryfmt[]="uiiiiiiii";
	LOAD_DBC("DBC/SpellAuraRestrictions.dbc",SpellAuraRestrictionsEntryfmt,false,dbcSpellAuraRestrictions, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellCastingRequirementsEntryfmt[]="uixxixi";
	LOAD_DBC("DBC/SpellCastingRequirements.dbc",SpellCastingRequirementsEntryfmt,false,dbcSpellCastingRequirements, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellCategoriesEntryfmt[]="uuuuuuu";
	LOAD_DBC("DBC/SpellCategories.dbc",SpellCategoriesEntryfmt,false,dbcSpellCategories, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellClassOptionsEntryfmt[]="uxiiiix";
	LOAD_DBC("DBC/SpellClassOptions.dbc",SpellClassOptionsEntryfmt,false,dbcSpellClassOptions, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellCooldownsEntryfmt[]="uiii";
	LOAD_DBC("DBC/SpellCooldowns.dbc",SpellCooldownsEntryfmt,false,dbcSpellCooldowns, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellEquippedItemsEntryfmt[]="uiii";
	LOAD_DBC("DBC/SpellEquippedItems.dbc",SpellEquippedItemsEntryfmt,false,dbcSpellEquippedItems, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellInterruptsEntryfmt[]="uixixi";
	LOAD_DBC("DBC/SpellInterrupts.dbc",SpellInterruptsEntryfmt,false,dbcSpellInterrupts, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellLevelsEntryfmt[]="uiii";
	LOAD_DBC("DBC/SpellLevels.dbc",SpellLevelsEntryfmt,false,dbcSpellLevels, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellPowerEntryfmt[]="uiiiixx";
	LOAD_DBC("DBC/SpellPower.dbc",SpellPowerEntryfmt,false,dbcSpellPower, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellReagentsEntryfmt[]="uiiiiiiiiiiiiiiii";
	LOAD_DBC("DBC/SpellReagents.dbc",SpellReagentsEntryfmt,false,dbcSpellReagents, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellScalingEntryfmt[]="uiiiiffffffffffi";
	LOAD_DBC("DBC/SpellScaling.dbc",SpellScalingEntryfmt,false,dbcSpellScaling, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellTargetRestrictionsEntryfmt[]="uixiii";
	LOAD_DBC("DBC/SpellTargetRestrictions.dbc",SpellTargetRestrictionsEntryfmt,false,dbcSpellTargetRestrictions, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char SpellTotemsEntryfmt[]="uiiii";
	LOAD_DBC("DBC/SpellTotems.dbc",SpellTotemsEntryfmt,false,dbcSpellTotems, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char classSpellScalefmt[]="uf";
	LOAD_DBC("DBC/gtSpellScaling.dbc", classSpellScalefmt, false, dbcGTSpellScale, true);

	printf("mem usage : %f\n",GetMemUsage() );
	const char* LFGDungeonFormat = "nxxuuuuuuuxuxxuxux";
	LOAD_DBC("DBC/LFGDungeons.dbc",LFGDungeonFormat,false,dbcLFGDungeonStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* WorldMapZoneFormat = "uuusffffxxxx";
	LOAD_DBC("DBC/WorldMapArea.dbc",WorldMapZoneFormat,false,dbcWorldMapZoneStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/Vehicle.dbc",VehicleEntryFormat,true,dbcVehicleEntry, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* VehicleSeatEntryFormat = "uuiffffffffffiiiiiifffffffiiifffiiiiiiiffiiiiixxxxxxxxxxxxxxxxxxxx";
	LOAD_DBC("DBC/VehicleSeat.dbc",VehicleSeatEntryFormat,true,dbcVehicleSeatEntry, false);
//	printf("mem usage : %f\n",GetMemUsage() );
//	const char* CurrencyTypesEntryFormat = "xnxu";
//	LOAD_DBC("DBC/CurrencyTypes.dbc",CurrencyTypesEntryFormat,true,dbcCurrencyTypesStore, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* scalingstatdistributionformat = "uiiiiiiiiiiuuuuuuuuuuxu";
	LOAD_DBC("DBC/ScalingStatDistribution.dbc",scalingstatdistributionformat,true,dbcScalingStatDistribution, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* scalingstatvaluesformat = "iniiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii";
	LOAD_DBC("DBC/ScalingStatValues.dbc",scalingstatvaluesformat,true,dbcScalingStatValues, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* WorldMapOverlayStoreFormat="nxiiiixxxxxxxxx";
	LOAD_DBC("DBC/WorldMapOverlay.dbc", WorldMapOverlayStoreFormat, true, dbcWorldMapOverlayStore, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* BattlemasterListEntryFormat = "uiiiiiiiiuxxuuuuuuu";
	LOAD_DBC("DBC/BattlemasterList.dbc", BattlemasterListEntryFormat, true, dbcBattlemasterListStore, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* itemlimitcategoryFormat = "uxux";
	LOAD_DBC("DBC/ItemLimitCategory.dbc", itemlimitcategoryFormat, true, dbcItemLimitCategory, true);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/AreaGroup.dbc", areagroupFormat, true, dbcAreaGroup, true);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/SpellRuneCost.dbc", SpellRuneCostFormat, true, dbcSpellRuneCostEntry, true);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/GlyphProperties.dbc", GlyphPropertiesFormat, true, dbcGlyphPropertiesStore, true);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/GlyphSlot.dbc", GlyphSlotFormat, true, dbcGlyphSlotStore, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* AchievementCriteriaFormat="niiiiLiiisixiiixxxxxxxx";
	LOAD_DBC("DBC/Achievement_Criteria.dbc", AchievementCriteriaFormat, true, dbcAchievementCriteriaStore, true);
	printf("mem usage : %f\n",GetMemUsage() );
const char* AchievementFormat=
	"n" // ID
	"i" // factionFlag
	"i" // mapID
	"u" // unknown1
//	"sxxxxxxxxxxxxxxx" // name
	"s" // name
//	"u" // name_flags
//	"sxxxxxxxxxxxxxxx" // description
	"s" // description
//	"u" // desc_flags
	"i" // categoryId
	"i" // points
	"u" // orderInCategory
	"i" // flags
	"u" // flags2
//	"sxxxxxxxxxxxxxxx" // rewardName
	"s" // rewardName
//	"u" // rewardName_flags
	"u" // count
	"u" // refAchievement
;
	LOAD_DBC("DBC/Achievement.dbc", AchievementFormat, true, dbcAchievementStore, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* BarberShopStyleEntryFormat="nxxxxxxi";
	LOAD_DBC("DBC/BarberShopStyle.dbc", BarberShopStyleEntryFormat, true, dbcBarberShopStyleStore, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* BarberShopCostFormat="xf";
	LOAD_DBC("DBC/gtBarberShopCostBase.dbc", BarberShopCostFormat, false, dbcBarberShopCostStore, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* ItemSetFormat = "usuuuuuuuuxxxxxxxxxuuuuuuuuuuuuuuuuuu";
	LOAD_DBC("DBC/ItemSet.dbc", ItemSetFormat, true, dbcItemSet, true);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/Lock.dbc", LockFormat, true, dbcLock, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* EmoteEntryFormat = "uxuuuuxuxuxxxxxxxxx";
	LOAD_DBC("DBC/EmotesText.dbc", EmoteEntryFormat, true, dbcEmoteEntry, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* skilllinespellFormat = "iiiiiiiiiiiiix";
	LOAD_DBC("DBC/SkillLineAbility.dbc", skilllinespellFormat, false, dbcSkillLineSpell, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* EnchantEntrYFormat = "uxuuuuuuuuuuuusuuuuuuuu";
	LOAD_DBC("DBC/SpellItemEnchantment.dbc", EnchantEntrYFormat, true, dbcEnchant, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* GemPropertyEntryFormat = "uuuuux";
	LOAD_DBC("DBC/GemProperties.dbc", GemPropertyEntryFormat, true, dbcGemProperty, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* skilllineentrYFormat = "uusxxxx";
	LOAD_DBC("DBC/SkillLine.dbc", skilllineentrYFormat, true, dbcSkillLine, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* spellentryFormat = "uuuuuuuuuuuxuuuufuuuussssuuuuufuuuuuuuuuuuuuuuuu";
	LOAD_DBC("DBC/Spell.dbc", spellentryFormat, true, dbcSpell, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* itemextendedcostFormat = "uuuuuuuuuuuuuuuuuuuuuuuuuuuxxxx";
//	LOAD_DBC("DBC/ItemExtendedCost.dbc", itemextendedcostFormat, true, dbcItemExtendedCost, false);
	LOAD_DBC("DBC/ItemExtendedCost.db2", itemextendedcostFormat, true, dbcItemExtendedCost, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* talententryFormat = "uuuuuuuuuxuxxxxuxxx";
	LOAD_DBC("DBC/Talent.dbc", talententryFormat, true, dbcTalent, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* talenttabentryFormat = "uxxuuuxxxuu";
	LOAD_DBC("DBC/TalentTab.dbc", talenttabentryFormat, true, dbcTalentTab, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* talentprimaryspellFormat = "uuux";
	LOAD_DBC("DBC/TalentTreePrimarySpells.dbc", talentprimaryspellFormat, true, dbcTalentPrimarySpells, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/SpellCastTimes.dbc", spellcasttimeFormat, true, dbcSpellCastTime, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/SpellRadius.dbc", spellradiusFormat, true, dbcSpellRadius, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* spellrangeFormat = "uffffxxx";
	LOAD_DBC("DBC/SpellRange.dbc", spellrangeFormat, true, dbcSpellRange, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/SpellDuration.dbc", spelldurationFormat, true, dbcSpellDuration, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* randompropsFormat = "uxuuuxxx";
	LOAD_DBC("DBC/ItemRandomProperties.dbc", randompropsFormat, true, dbcRandomProps, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* areatableFormat = "uuuuuxxxxxusuxxxxxxxxxxxxx";
	LOAD_DBC("DBC/AreaTable.dbc", areatableFormat, true, dbcArea, true);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/FactionTemplate.dbc", factiontemplatedbcFormat, true, dbcFactionTemplate, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* factiondbcFormat = "uiuuuuuuuuiiiiuuuuuxxxxsxx";
	LOAD_DBC("DBC/Faction.dbc", factiondbcFormat, true, dbcFaction, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* dbctaxinodeFormat = "uufffxuuuff";
	LOAD_DBC("DBC/TaxiNodes.dbc", dbctaxinodeFormat, false, dbcTaxiNode, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* dbctaxipathFormat = "uuuu";
	LOAD_DBC("DBC/TaxiPath.dbc", dbctaxipathFormat, false, dbcTaxiPath, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* dbctaxipathnodeFormat = "uuuufffuuxx";
	LOAD_DBC("DBC/TaxiPathNode.dbc", dbctaxipathnodeFormat, false, dbcTaxiPathNode, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/CreatureSpellData.dbc", creaturespelldataFormat, true, dbcCreatureSpellData, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* creaturefamilyFormat = "ufufuuuuuusx";
	LOAD_DBC("DBC/CreatureFamily.dbc", creaturefamilyFormat, true, dbcCreatureFamily, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* charraceFormat = "nxixiixixxxxixsxxxxxixxx";
	LOAD_DBC("DBC/ChrRaces.dbc", charraceFormat, true, dbcCharRace, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* charclassFormat = "uuxsxxxxxxxxxx";
	LOAD_DBC("DBC/ChrClasses.dbc", charclassFormat, true, dbcCharClass, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* mapentryFormat = "nxixxxsixxixiffxixxx";
	LOAD_DBC("DBC/Map.dbc", mapentryFormat, true, dbcMap, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* auctionhousedbcFormat = "uuuux";
	LOAD_DBC("DBC/AuctionHouse.dbc", auctionhousedbcFormat, true, dbcAuctionHouse, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* itemrandomsuffixformat = "uxxuuuuuuuuuu";
	LOAD_DBC("DBC/ItemRandomSuffix.dbc", itemrandomsuffixformat, true, dbcItemRandomSuffix, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* randompropopointsfmt = "iiiiiiiiiiiiiiii";
	LOAD_DBC("DBC/RandPropPoints.dbc", randompropopointsfmt, true, dbcRandomPropertyPoints, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* itemreforgeformat = "uufuu";
	LOAD_DBC("DBC/ItemReforge.dbc", itemreforgeformat, true, dbcItemReforge, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/gtCombatRatings.dbc", gtClassfloatformat, false, dbcCombatRating, false);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* chatchannelformat = "uuxsx";
	LOAD_DBC("DBC/ChatChannels.dbc", chatchannelformat, true, dbcChatChannels, true);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/DurabilityQuality.dbc", durabilityqualityFormat, true, dbcDurabilityQuality, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/DurabilityCosts.dbc", durabilitycostsFormat, true, dbcDurabilityCosts, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/BankBagSlotPrices.dbc", bankslotpriceformat, true, dbcBankSlotPrices, false);
//	printf("mem usage : %f\n",GetMemUsage() );
//	LOAD_DBC("DBC/StableSlotPrices.dbc", bankslotpriceformat, true, dbcStableSlotPrices, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/gtChanceToMeleeCrit.dbc", gtClassfloatformat, false, dbcMeleeCrit, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/gtChanceToMeleeCritBase.dbc", gtClassfloatformat, false, dbcMeleeCritBase, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/gtChanceToSpellCrit.dbc", gtClassfloatformat, false, dbcSpellCrit, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/gtChanceToSpellCritBase.dbc", gtClassfloatformat, false, dbcSpellCritBase, false);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/gtRegenMPPerSpt.dbc", gtClassfloatformat, false, dbcMPRegenBase, false); //it's not a mistake.
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/gtOCTRegenMP.dbc", gtClassfloatformat, false, dbcMPRegen, false); //it's not a mistake.
//	printf("mem usage : %f\n",GetMemUsage() );
//	LOAD_DBC("DBC/gtRegenHPPerSpt.dbc", gtfloatformat, false, dbcHPRegenBase, false); //it's not a mistake.
//	printf("mem usage : %f\n",GetMemUsage() );
//	LOAD_DBC("DBC/gtOCTRegenHP.dbc", gtfloatformat, false, dbcHPRegen, false); //it's not a mistake.
	printf("mem usage : %f\n",GetMemUsage() );
	const char* areatriggerformat = "uufffxxxxxxxx";
	LOAD_DBC("DBC/AreaTrigger.dbc", areatriggerformat, true, dbcAreaTrigger, true);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/QuestXP.dbc", questxpformat, false, dbcQuestXP, true);
	printf("mem usage : %f\n",GetMemUsage() );
	LOAD_DBC("DBC/QuestFactionReward.dbc", questrewrepformat, false, dbcQuestRewRep, true);
	printf("mem usage : %f\n",GetMemUsage() );
	const char* chartitleFormat = "uxsxux";
	LOAD_DBC("DBC/CharTitles.dbc", chartitleFormat, true, dbcCharTitle, true);
	printf("mem usage : %f\n",GetMemUsage() );
	return true;
}

uint32 *SpellEntry::GetSpellGroupType()
{
	return SpellGroupType;
}

/*
uint32 SpellEntry::GetCasterAuraState()
{
	if( SpellAuraRestrictionsId )
	{
		SpellAuraRestrictionsEntry *sar = dbcSpellAuraRestrictions.LookupEntryForced( SpellAuraRestrictionsId );
		if( sar )
			return sar->CasterAuraState;
	}
	return 0;
}

uint32 SpellEntry::GetChannelInterruptFlags()
{
	return 0;
}

uint32 SpellEntry::GetRecoveryTime()
{
	return 0;
}

uint32 SpellEntry::GetInterruptFlags()
{
	return 0;
}

uint32 SpellEntry::GetCategoryRecoveryTime()
{
	return 0;
}

uint32 SpellEntry::GetRequiredShapeShift()
{
	if( SpellShapeshiftId )
	{
		SpellShapeshiftEntry *ssse =dbcSpellShapeshift.LookupEntryForced( SpellShapeshiftId );
		return ssse->Stances;
	}
	return 0;
}

uint32 SpellEntry::GetTargetAuraState()
{
	if( SpellAuraRestrictionsId )
	{
		SpellAuraRestrictionsEntry *sar = dbcSpellAuraRestrictions.LookupEntryForced( SpellAuraRestrictionsId );
		if( sar )
			return sar->TargetAuraState;
	}
	return 0;
}

uint32 SpellEntry::GetTargetAuraStateNot()
{
	if( SpellAuraRestrictionsId )
	{
		SpellAuraRestrictionsEntry *sar = dbcSpellAuraRestrictions.LookupEntryForced( SpellAuraRestrictionsId );
		if( sar )
			return sar->TargetAuraStateNot;
	}
	return 0;
}

uint32 SpellEntry::GetCasterAuraStateNot()
{
	if( SpellAuraRestrictionsId )
	{
		SpellAuraRestrictionsEntry *sar = dbcSpellAuraRestrictions.LookupEntryForced( SpellAuraRestrictionsId );
		if( sar )
			return sar->CasterAuraStateNot;
	}
	return 0;
}

uint32 SpellEntry::GetCasterAuraSpell()
{
	if( SpellAuraRestrictionsId )
	{
		SpellAuraRestrictionsEntry *sar = dbcSpellAuraRestrictions.LookupEntryForced( SpellAuraRestrictionsId );
		if( sar )
			return sar->casterAuraSpell;
	}
	return 0;
}

uint32 SpellEntry::GetCasterAuraSpellNot()
{
	if( SpellAuraRestrictionsId )
	{
		SpellAuraRestrictionsEntry *sar = dbcSpellAuraRestrictions.LookupEntryForced( SpellAuraRestrictionsId );
		if( sar )
			return sar->excludeCasterAuraSpell;
	}
	return 0;
}

uint32 SpellEntry::GetTargetAuraSpell()
{
	if( SpellAuraRestrictionsId )
	{
		SpellAuraRestrictionsEntry *sar = dbcSpellAuraRestrictions.LookupEntryForced( SpellAuraRestrictionsId );
		if( sar )
			return sar->targetAuraSpell;
	}
	return 0;
}

uint32 SpellEntry::GetTargetAuraSpellNot()
{
	if( SpellAuraRestrictionsId )
	{
		SpellAuraRestrictionsEntry *sar = dbcSpellAuraRestrictions.LookupEntryForced( SpellAuraRestrictionsId );
		if( sar )
			return sar->excludeTargetAuraSpell;
	}
	return 0;
} */

uint32 SpellEntry::GetRequiresSpellFocus()
{
	if( SpellTargetRestrictionsId )
	{
		SpellCastingRequirementsEntry *str = dbcSpellCastingRequirements.LookupEntryForced( SpellTargetRestrictionsId );
		if( str )
			return str->RequiresSpellFocus;
	}
	return 0;
}
/*
uint32 SpellEntry::GetRequiresAreaId()
{
	if( SpellTargetRestrictionsId )
	{
		SpellCastingRequirementsEntry *str = dbcSpellCastingRequirements.LookupEntryForced( SpellTargetRestrictionsId );
		if( str )
			return str->AreaGroupId;
	}
	return 0;
} */

uint32 *SpellEntry::GetTotem()
{
	if( SpellTotemsId )
	{
		SpellTotemsEntry *ste =dbcSpellTotems.LookupEntryForced( SpellTotemsId );
		if( ste )
			return ste->Totem;
	}
	return 0;
}

int32 *SpellEntry::GetReagent()
{
	if( SpellReagentsId )
	{
		SpellReagentsEntry *sre = dbcSpellReagents.LookupEntryForced( SpellReagentsId );
		if( sre )
			return &sre->Reagent[0];
	}
	return 0;
}

uint32 *SpellEntry::GetReagentCount()
{
	if( SpellReagentsId )
	{
		SpellReagentsEntry *sre =dbcSpellReagents.LookupEntryForced( SpellReagentsId );
		if( sre )
			return sre->ReagentCount;
	}
	return 0;
}

int32 SpellEntry::GetEquippedItemClass()
{
	if( SpellEquippedItemsId )
	{
		SpellEquippedItemsEntry *see =dbcSpellEquippedItems.LookupEntryForced( SpellEquippedItemsId );
		if( see )
			return see->EquippedItemClass;
	}
	return -1;
}

int32 SpellEntry::GetEquippedItemSubClass()
{
	if( SpellEquippedItemsId )
	{
		SpellEquippedItemsEntry *see =dbcSpellEquippedItems.LookupEntryForced( SpellEquippedItemsId );
		if( see )
			return see->EquippedItemSubClassMask;
	}
	return -1;
}

uint32 SpellEntry::GetRequiredItemFlags()
{
	if( SpellEquippedItemsId )
	{
		SpellEquippedItemsEntry *see =dbcSpellEquippedItems.LookupEntryForced( SpellEquippedItemsId );
		if( see )
			return see->EquippedItemInventoryTypeMask;
	}
	return 0;
}
/*
uint32 SpellEntry::GetSpellDMGType()
{
	if( SpellCategoriesId )
	{
		SpellCategoriesEntry *sce = dbcSpellCategories.LookupEntryForced( SpellCategoriesId );
		if( sce )
			return sce->DmgClass;
	}
	return 0;
}

uint32 SpellEntry::GetMaxTargets()
{
	if( SpellTargetRestrictionsId )
	{
		SpellTargetRestrictionsEntry *str = dbcSpellTargetRestrictions.LookupEntryForced( SpellTargetRestrictionsId );
		if( str )
			return str->MaxAffectedTargets;
	}
	return 0;
}*/

uint32 SpellEntry::GetDispelType()
{
	if( SpellCategoriesId )
	{
		SpellCategoriesEntry *sce = dbcSpellCategories.LookupEntryForced( SpellCategoriesId );
		if( sce )
			return sce->Dispel;
	}
	return 0;
}
/*
uint32 SpellEntry::GetManaCost()
{
	if( SpellPowerId )
	{
		SpellPowerEntry *spe = dbcSpellPower.LookupEntryForced( SpellPowerId );
		if( spe )
			return spe->manaCost;
	}
	return 0;
}

uint32 SpellEntry::GetManaCostPCT()
{
	if( SpellPowerId )
	{
		SpellPowerEntry *spe = dbcSpellPower.LookupEntryForced( SpellPowerId );
		if( spe )
			return spe->ManaCostPercentage;
	}
	return 0;
}
*/
/*
uint32 SpellEntry::GetBaseLevel()
{
	if( SpellLevelsId )
	{
		SpellLevelsEntry *sle = dbcSpellLevels.LookupEntryForced( SpellLevelsId );
		if( sle )
			return sle->baseLevel;
	}
	return 0;
}

uint32 SpellEntry::GetMaxLevel()
{
	if( SpellLevelsId )
	{
		SpellLevelsEntry *sle = dbcSpellLevels.LookupEntryForced( SpellLevelsId );
		if( sle )
			return sle->maxLevel;
	}
	return 0;
}
*/
