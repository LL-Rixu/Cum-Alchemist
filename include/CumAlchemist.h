#pragma once
#include <PCH.h>

#include <Io.h>
#include <Cum.h>
#include <MCM.h>
#include <Perk.h>
#include <Potion.h>
#include <Alchemy.h>

class CumAlchemist : public Alchemy, public Potion, public Cum, public Perk, public MCM, public Io
{
    static CumAlchemist* alchemist;
    RE::PlayerCharacter* player;

    RE::FormID GetID();
    bool GetEffects(RE::CraftingSubMenus::AlchemyMenu*, std::map<RE::FormID, RE::Effect*>&);
public:
    static void interface(SKSE::MessagingInterface::Message*);

    void EventCreatePotion(RE::CraftingSubMenus::AlchemyMenu*) override;
    void EventOnLoad(std::vector<uint8_t>&) override;
    std::vector<uint8_t> EventOnSave() override;
    void EventOnCum(RE::TESForm*, bool, RE::TESForm*, float) override;

    void AddItem(RE::TESBoundObject*, int32_t);
    void AddExp(RE::ActorValue, float);

    CumAlchemist();
    ~CumAlchemist();
};