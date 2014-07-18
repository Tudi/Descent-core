#include "StdAfx.h"

uint32 ItemPrototype::GetArmor() 
{
    if(Quality >= ITEM_QUALITY_HEIRLOOM_LIGHT_YELLOW )                    // heirlooms have it's own dbc...
        return 0;

    if(Class == ITEM_CLASS_ARMOR && SubClass == ITEM_SUBCLASS_ARMOR_SHIELD)
    {
        if(ItemArmorShieldEntry const* ias = dbcItemArmorShieldStore.LookupEntry(ItemLevel))
        {
            return uint32(floor(ias->Value[Quality] + 0.5f));
        }
        return 0;
    }

    ItemArmorQualityEntry const* iaq = dbcItemArmorQualityStore.LookupEntry(ItemLevel);
    ItemArmorTotalEntry const* iat = dbcItemArmorTotalStore.LookupEntry(ItemLevel);

    if(!iaq || !iat)
        return 0;

    if(InventoryType != INVTYPE_HEAD && InventoryType != INVTYPE_CHEST && InventoryType != INVTYPE_SHOULDERS
        && InventoryType != INVTYPE_LEGS && InventoryType != INVTYPE_FEET && InventoryType != INVTYPE_WRISTS
        && InventoryType != INVTYPE_HANDS && InventoryType != INVTYPE_WAIST && InventoryType != INVTYPE_CLOAK
        && InventoryType != INVTYPE_ROBE)
        return 0;

    ArmorLocationEntry const* al = NULL;

    if(InventoryType == INVTYPE_ROBE)
        al = dbcArmorLocationStore.LookupEntry(INVTYPE_CHEST);
    else
        al = dbcArmorLocationStore.LookupEntry(InventoryType);

    if(!al)
        return 0;

    float iatMult, alMult;

    switch(SubClass)
    {
        case ITEM_SUBCLASS_ARMOR_CLOTH:
            iatMult = iat->Value[0];
            alMult = al->Value[0];
            break;
        case ITEM_SUBCLASS_ARMOR_LEATHER:
            iatMult = iat->Value[1];
            alMult = al->Value[1];
            break;
        case ITEM_SUBCLASS_ARMOR_MAIL:
            iatMult = iat->Value[2];
            alMult = al->Value[2];
            break;
        case ITEM_SUBCLASS_ARMOR_PLATE_MAIL:
            iatMult = iat->Value[3];
            alMult = al->Value[3];
            break;
        default:
            return 0;
    }

    return uint32(floor(iaq->Value[Quality] * iatMult * alMult + 0.5f));
}

float ItemPrototype::getDPS() 
{
    float damage = 0.0f;

    if(Class == ITEM_CLASS_WEAPON)
    {
        if(Quality >= ITEM_QUALITY_HEIRLOOM_LIGHT_YELLOW)                // heirlooms have it's own dbc...
            return damage;

        ItemDamageEntry const* id = NULL;

        switch(InventoryType)
        {
            case INVTYPE_WEAPON:
            case INVTYPE_WEAPONMAINHAND:
            case INVTYPE_WEAPONOFFHAND:
                if(Flags2 & ITEM_FLAG2_CASTER_WEAPON)       // caster weapon flag
                    id = dbcItemDamageOneHandCasterStore.LookupEntry(ItemLevel);
                else
                    id = dbcItemDamageOneHandStore.LookupEntry(ItemLevel);
                break;
            case INVTYPE_2HWEAPON:
                if(Flags2 & ITEM_FLAG2_CASTER_WEAPON)       // caster weapon flag
                    id = dbcItemDamageTwoHandCasterStore.LookupEntry(ItemLevel);
                else
                    id = dbcItemDamageTwoHandStore.LookupEntry(ItemLevel);
                break;
            case INVTYPE_AMMO:
                id = dbcItemDamageAmmoStore.LookupEntry(ItemLevel);
                break;
            case INVTYPE_RANGED:
            case INVTYPE_THROWN:
            case INVTYPE_RANGEDRIGHT:
                switch(SubClass)
                {
                    case ITEM_SUBCLASS_WEAPON_BOW:
                    case ITEM_SUBCLASS_WEAPON_GUN:
                    case ITEM_SUBCLASS_WEAPON_CROSSBOW:
                        id = dbcItemDamageRangedStore.LookupEntry(ItemLevel);
                        break;
                    case ITEM_SUBCLASS_WEAPON_THROWN:
                        id = dbcItemDamageThrownStore.LookupEntry(ItemLevel);
                        break;
                    case ITEM_SUBCLASS_WEAPON_WAND:
                        id = dbcItemDamageWandStore.LookupEntry(ItemLevel);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }

        if(!id)
            return damage;

        return id->Value[Quality];
    }

    return damage;
}
/*
int32 ItemPrototype::getFeralBonus(int32 extraDPS)
{
    if( Class == ITEM_CLASS_WEAPON )
    {
        int32 bonus = int32((extraDPS + getDPS())*14.0f) - 767;
        if (bonus < 0)
            return 0;
        return bonus;
    }
    return 0;
}*/
