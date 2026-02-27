#pragma once
#include <PCH.h>

class Alchemy
{
    static Alchemy* alchemy;
    SKSE::Trampoline* trampoline;

    static void internal_EventCreatePotion(RE::CraftingSubMenus::AlchemyMenu* menu);
    static inline REL::Relocation<decltype(internal_EventCreatePotion)> skyrim_EventCreatePotion;

public:
    Alchemy();
    ~Alchemy();

    virtual void EventCreatePotion(RE::CraftingSubMenus::AlchemyMenu* menu) = 0;

    void (*const refresh)(RE::CraftingSubMenus::AlchemyMenu*, uint8_t);
};
