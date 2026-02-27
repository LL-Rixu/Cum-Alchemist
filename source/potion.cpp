#include <Potion.h>
#include <Log.h>

#define GetKeyword(x) kwpotion[x] = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(_GetKeyword(#x).data())

template<size_t N>
static consteval std::array<char, N+9> _GetKeyword(const char (&type)[N])
{
    std::array<char, N+9> keyword = { 'M', 'a', 'g', 'i', 'c', 'A', 'l', 'c', 'h' };
    for(size_t i = 9, j = 0; j < N; ++i, ++j) 
    {
        if(type[j] == ':') { --i; continue; }
        keyword[i] = type[j];
    }
    return keyword;
}

Potion::Potion(): factory(RE::IFormFactory::GetConcreteFormFactoryByType<RE::AlchemyItem>()), 
                  esp(RE::TESDataHandler::GetSingleton()->LookupModByName("Cum Alchemist.esp"))
{
    GetKeyword(Beneficial);
    GetKeyword(Harmful);

    GetKeyword(Fortify::Alchemy);
    GetKeyword(Fortify::Alteration);
    GetKeyword(Fortify::Block);
    GetKeyword(Fortify::CarryWeight);
    GetKeyword(Fortify::Conjuration);
    GetKeyword(Fortify::Destruction);
    GetKeyword(Fortify::Enchanting);
    GetKeyword(Fortify::HealRate);
    GetKeyword(Fortify::Health);
    GetKeyword(Fortify::HeavyArmor);
    GetKeyword(Fortify::Illusion);
    GetKeyword(Fortify::LigthArmor);
    GetKeyword(Fortify::Lockpicking);
    GetKeyword(Fortify::Magicka);
    GetKeyword(Fortify::MagickaRate);
    GetKeyword(Fortify::Marksman);
    GetKeyword(Fortify::Mass);
    GetKeyword(Fortify::OneHanded);
    GetKeyword(Fortify::PickPocket);
    GetKeyword(Fortify::Restoration);
    GetKeyword(Fortify::Smithing);
    GetKeyword(Fortify::Sneak);
    GetKeyword(Fortify::Speechcraft);
    GetKeyword(Fortify::Stamina);
    GetKeyword(Fortify::StaminaRate);
    GetKeyword(Fortify::TwoHanded);

    GetKeyword(Weakness::Fire);
    GetKeyword(Weakness::Frost);
    GetKeyword(Weakness::Magic);
    GetKeyword(Weakness::Shock);

    GetKeyword(Damage::Health);
    GetKeyword(Damage::Magicka);
    GetKeyword(Damage::Stamina);

    GetKeyword(Restore::Health);
    GetKeyword(Restore::Magicka);
    GetKeyword(Restore::Stamina);

    GetKeyword(Resist::Fire);
    GetKeyword(Resist::Frost);
    GetKeyword(Resist::Magic);
    GetKeyword(Resist::Poison);
    GetKeyword(Resist::Shock);

    kwpotion[Tracking]   = RE::TESForm::LookupByEditorID<RE::BGSKeyword>("CmAl_Tracking");

    datahandler = RE::TESDataHandler::GetSingleton();
    for(RE::AlchemyItem* potion : datahandler->GetFormArray<RE::AlchemyItem>()) { cache[potion] = potion; }
}

RE::BSFixedString Potion::GetName(std::map<RE::FormID, RE::Effect*>& effects)
{
    char buffer[25];

    RE::FormID m_id;
    float m_mag = -INFINITY;
    int path = 1;
    for(auto& [id, effect] : effects) 
    { 
        path += effect->baseEffect->HasKeyword(kwpotion[Potion::Beneficial]) ? 5 : -3; 

        if(m_mag <= effect->effectItem.magnitude)
        { 
            m_id = id;
            m_mag = effect->effectItem.magnitude; 
        }
    }

    const char* scale = "Weak";
    if(m_mag > 10)  { scale = "Mediocre"; }
    if(m_mag > 25)  { scale = "Moderate"; }
    if(m_mag > 50)  { scale = "Strong"; }
    if(m_mag > 100) { scale = "Mighty"; }

    /* add rest of the potion keywords */
    const char* type = "Mixed";
    if(effects[m_id]->baseEffect->HasKeyword(kwpotion[Restore::Health]))       { type = "Healing"; } 
    else if(effects[m_id]->baseEffect->HasKeyword(kwpotion[Restore::Stamina])) { type = "Stamina"; }
    else if(effects[m_id]->baseEffect->HasKeyword(kwpotion[Restore::Magicka])) { type = "Magicka"; }

    const char* end = (path >= 0) ? "Potion" : "Poison";

    snprintf(buffer, sizeof(buffer), "%s %s %s", scale, type, end);

    return buffer;
}

float Potion::GetWeight(std::map<RE::FormID, RE::Effect*>& effects)
{
    float weigth = 4.f;

    for(auto& [id, effect] : effects) { weigth += effect->GetMagnitude() * ((float)effect->GetDuration()) / 180.f; }

    return log2f(weigth) - 2.f;
}

RE::AlchemyItem::AlchemyFlag Potion::GetFlags(std::map<RE::FormID, RE::Effect*>& effects)
{
    int path = 1;
    for(auto& [id, effect] : effects) { path += effect->baseEffect->HasKeyword(kwpotion[Beneficial]) ? 5 : -3; }

    return (path >= 0) ? RE::AlchemyItem::AlchemyFlag::kMedicine : RE::AlchemyItem::AlchemyFlag::kPoison;
}

RE::TESFileArray* Potion::SetFile()
{
    struct Array { RE::TESFile** _data{ nullptr }; uint32_t _size{ 0 }; }* array = reinterpret_cast<Array*>(calloc(sizeof(Array) + sizeof(RE::TESFile*), 1));

    array->_size = 1;
    array->_data = reinterpret_cast<RE::TESFile**>(&array[1]);
    array->_data[0] = const_cast<RE::TESFile*>(esp);

    return reinterpret_cast<RE::TESFileArray*>(array);
}

RE::AlchemyItem* Potion::GetPotion(std::map<RE::FormID, RE::Effect*>& effects, RE::FormID id)
{
    if(cache.contains(effects)) { return cache[effects]; }

    RE::AlchemyItem* potion = factory->Create();
    if(!potion) { return nullptr; }

    potion->formID = id;
    potion->fullName = GetName(effects);
    potion->weight = GetWeight(effects);
    potion->data.flags.set(GetFlags(effects));
    potion->AddKeyword(kwpotion[Tracking]);

    for(auto& [_id, effect] : effects)
    {
        potion->effects.push_back(effect);    
    }

    potion->sourceFiles.array = SetFile(); // Leaks memory
    datahandler->GetFormArray<RE::AlchemyItem>().push_back(potion);
    RE::TESForm::GetAllForms().first->emplace(id, potion);

    cache[potion] = potion;
    potions.push_back(potion);

    return potion;
}

void Potion::ResetPotion()
{
    for(RE::AlchemyItem* potion : potions)
    {
        auto& array = datahandler->GetFormArray<RE::AlchemyItem>();
        auto iterator = std::find(array.begin(), array.end(), potion);
        array.erase(iterator);
        RE::TESForm::GetAllForms().first->erase(potion->formID);
        potion->SetDelete(true); // Setup proper RE::AlchemyItem culling
    }

    potions.clear();
    cache.clear();
    for(RE::AlchemyItem* potion : datahandler->GetFormArray<RE::AlchemyItem>()) { cache[potion] = potion; }
}