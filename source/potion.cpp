#include <Potion.h>

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
    GetKeyword(Fortify::LightArmor);
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

    datahandler = RE::TESDataHandler::GetSingleton();
    for(RE::AlchemyItem* potion : datahandler->GetFormArray<RE::AlchemyItem>()) { cache[potion] = potion; }
}

static _forceinline const char* GetScale(const std::map<RE::FormID, RE::Effect*>& effects)
{
    const char* scales[] = { "Weak", "Mediocre", "Moderate", "Strong", "Mighty" };

    float magnitude = 0.f;
    for(auto& [id, effect] : effects)
    {
        magnitude += effect->effectItem.magnitude * effect->effectItem.magnitude;
    }
    const float average = sqrtf(magnitude) / static_cast<float>(effects.size());
    const int i = static_cast<const int>(1.52f * std::logf(0.27f * average + 1.f));
    return scales[std::clamp(i, 0, 4)];
}

/* Implement smarter type naming function */
static _forceinline const char* GetType(const std::map<RE::FormID, RE::Effect*>& effects, const std::array<RE::BGSKeyword*, 43>& kwpotion)
{
    const char* types[] = 
    {
        "Beneficial",
        "Harmful",

        // Fortify
        "Alchemy",
        "Alteration",
        "Block",
        "Carry weight",
        "Conjuration",
        "Destruction",
        "Enchanting",
        "Regeneration", // HealRate,
        "Health",
        "Heavy armor",
        "Illusion",
        "Ligth armor",
        "Lockpicking",
        "Magicka",
        "Lucidity", // MagickaRate
        "Archery",  // Marksman
        "Mass",
        "One handed",
        "Pickpocket",
        "Restoration",
        "Smithing",
        "Sneak",
        "Speechcraft",
        "Stamina",
        "Vigor", // StaminaRate
        "Two handed",

        // Damage
        "Health",
        "Magicka",
        "Stamina",

        // Resist
        "Fire",
        "Frost",
        "Magic",
        "Poison",
        "Shock",

        // Restore
        "Healing",
        "Magic",
        "Stamina",

        // Weakness
        "Fire",
        "Frost",
        "Magic",
        "Shock",
    };

    std::vector<std::pair<int, float>> match;

    std::unordered_map<RE::FormID, int> map;
    for(size_t i = 0; i < kwpotion.size(); ++i) 
    {
        if(!kwpotion[i]){ continue; }
        map[kwpotion[i]->formID] = i; 
    }

    for(auto& [id, effect] : effects)
    {
        auto keywords = effect->baseEffect->keywords;
        for(size_t i = 0; i < effect->baseEffect->numKeywords; ++i)
        {
            if(map.contains(keywords[i]->formID))
            {
                match.push_back({map[keywords[i]->formID], effect->effectItem.magnitude});
            }
        }
    }
    
    if(match.empty()) { return ""; }

    std::sort(match.begin(), match.end(), [](std::pair<int, float>& a, std::pair<int, float>& b) { return a.second > b.second; });

    return types[std::clamp(match[0].first, 0, (int)(sizeof(types) / sizeof(*types)) - 1)];
}

static _forceinline const char* GetEnd(const std::map<RE::FormID, RE::Effect*>& effects, const RE::BGSKeyword* keyword)
{
    int path = 1;
    for(auto& [id, effect] : effects) { path += effect->baseEffect->HasKeyword(keyword) ? 5 : -3; }
    return (path >= 0) ? "potion" : "poison";
}

RE::BSFixedString Potion::GetName(const std::map<RE::FormID, RE::Effect*>& effects) const
{
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%s %s %s", GetScale(effects), GetType(effects, kwpotion), GetEnd(effects, kwpotion[Beneficial]));
    return buffer;
}

float Potion::GetWeight(const std::map<RE::FormID, RE::Effect*>& effects) const
{
    float weigth = 4.f;

    for(auto& [id, effect] : effects) { weigth += effect->GetMagnitude() * ((float)effect->GetDuration()) / 180.f; }

    return log2f(weigth) - 2.f;
}

RE::AlchemyItem::AlchemyFlag Potion::GetFlags(const std::map<RE::FormID, RE::Effect*>& effects) const
{
    int path = 1;
    for(auto& [id, effect] : effects) { path += effect->baseEffect->HasKeyword(kwpotion[Beneficial]) ? 5 : -3; }

    return (path >= 0) ? RE::AlchemyItem::AlchemyFlag::kMedicine : RE::AlchemyItem::AlchemyFlag::kPoison;
}

RE::TESFileArray* Potion::SetFile() const
{
    struct Array { RE::TESFile** _data{ nullptr }; uint32_t _size{ 0 }; }* array = reinterpret_cast<Array*>(std::calloc(sizeof(Array) + sizeof(RE::TESFile*), 1));

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

    for(auto& [_id, effect] : effects)
    {
        potion->effects.push_back(effect);    
    }

    potion->sourceFiles.array = SetFile();
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
        std::free(potion->sourceFiles.array);
        potion->sourceFiles.array = nullptr;
        potion->SetDelete(true); // Setup proper RE::AlchemyItem culling
    }

    potions.clear();
    cache.clear();
    for(RE::AlchemyItem* potion : datahandler->GetFormArray<RE::AlchemyItem>()) { cache[potion] = potion; }
}