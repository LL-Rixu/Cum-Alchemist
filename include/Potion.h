#pragma once
#include <PCH.h>

#include <Key.h>

class Potion
{
    RE::TESDataHandler* datahandler;
    RE::ConcreteFormFactory<RE::AlchemyItem, RE::FormType::AlchemyItem>* factory;

    RE::BSFixedString GetName(const std::map<RE::FormID, RE::Effect*>&) const;
    float GetWeight(const std::map<RE::FormID, RE::Effect*>&) const;
    RE::AlchemyItem::AlchemyFlag GetFlags(const std::map<RE::FormID, RE::Effect*>&) const;
    RE::TESFileArray* SetFile() const;
public:
    const RE::TESFile* esp;
    std::unordered_map<Key<RE::AlchemyItem*>, RE::AlchemyItem*> cache;
    std::vector<RE::AlchemyItem*> potions;
    enum { Beneficial = 0, Harmful = 1 };

    struct Fortify
    {
        enum
        {
            Alchemy = 2,
            Alteration = 3,
            Block = 4,
            CarryWeight = 5,
            Conjuration = 6,
            Destruction = 7,
            Enchanting = 8,
            HealRate = 9,
            Health = 10,
            HeavyArmor = 11,
            Illusion = 12,
            LightArmor = 13,
            Lockpicking = 14,
            Magicka = 15,
            MagickaRate = 16,
            Marksman = 17,
            Mass = 18,
            OneHanded = 19,
            PickPocket = 20,
            Restoration = 21,
            Smithing = 22,
            Sneak = 23,
            Speechcraft = 24,
            Stamina = 25,
            StaminaRate = 26,
            TwoHanded = 27,
        };
    };
    struct Damage { enum { Health = 28, Magicka = 29, Stamina = 30, }; };
    struct Resist { enum { Fire = 31, Frost = 32, Magic = 33, Poison = 34, Shock = 35 }; };
    struct Restore { enum{ Health = 36, Magicka = 37, Stamina = 38 }; };
    struct Weakness { enum { Fire = 39, Frost = 40, Magic = 41, Shock = 42 }; };

    std::array<RE::BGSKeyword*, 43> kwpotion;

    Potion();
    ~Potion() = default;

    RE::AlchemyItem* GetPotion(std::map<RE::FormID, RE::Effect*>&, RE::FormID);

    void ResetPotion();
};