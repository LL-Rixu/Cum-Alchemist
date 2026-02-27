#include <alchemy.h>

Alchemy* Alchemy::alchemy = nullptr;

Alchemy::Alchemy():
    refresh(reinterpret_cast<void (*)(RE::CraftingSubMenus::AlchemyMenu*, uint8_t)>(REL::Offset(0x090c250).address()))
{
    alchemy = this;

    const REL::Relocation<std::uintptr_t> target { REL::Offset(0x90d04b) };

    trampoline = new SKSE::Trampoline();
    trampoline->create(64);
    skyrim_EventCreatePotion = trampoline->write_call<5>(target.address(), internal_EventCreatePotion);
}

Alchemy::~Alchemy()
{
    alchemy = nullptr;
    delete trampoline;
}

void Alchemy::internal_EventCreatePotion(RE::CraftingSubMenus::AlchemyMenu* menu) { alchemy->EventCreatePotion(menu); }