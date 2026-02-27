#include <CumAlchemist.h>
#include <Log.h>

CumAlchemist* CumAlchemist::alchemist;

void CumAlchemist::interface(SKSE::MessagingInterface::Message* message)
{
    switch(message->type)
    {
    case SKSE::MessagingInterface::kDataLoaded: alchemist = new CumAlchemist(); break;

    case SKSE::MessagingInterface::kPreLoadGame: OnLoad(message->data, message->dataLen); break;
    case SKSE::MessagingInterface::kSaveGame: OnSave(message->data, message->dataLen); break;
    
    case SKSE::MessagingInterface::kDeleteGame: /* TODO remove .potion file */ break;

    default: break;
    }
}

CumAlchemist::CumAlchemist(): player(RE::PlayerCharacter::GetSingleton()) {}

bool CumAlchemist::GetEffects(RE::CraftingSubMenus::AlchemyMenu* menu, std::map<RE::FormID, RE::Effect*>& effects)
{
    uint8_t cum_count = 0;

    std::vector<RE::IngredientItem*> ingridients;
    std::unordered_map<RE::FormID, std::pair<RE::Effect*, uint32_t>> map;

    for(uint32_t i : menu->selectedIndexes)
    {
        if (i >= menu->ingredientEntries.size() || !menu->ingredientEntries[i].isSelected || !menu->ingredientEntries[i].ingredient->countDelta) { continue; }

        ingridients.push_back(menu->ingredientEntries[i].ingredient->object->As<RE::IngredientItem>());
    }

    for(RE::IngredientItem* ingridient : ingridients) 
    {
        for (RE::Effect* effect : ingridient->effects) 
        {
            const RE::FormID key = effect->baseEffect->GetFormID();
            std::pair<RE::Effect*, uint32_t>& element = map[key];

            element.second += 1;

            if(!element.first)
            {
                element.first = new RE::Effect(*effect);
                continue;
            }

            RE::Effect& ef = *element.first;
            ef.effectItem.duration += effect->effectItem.duration;
            ef.effectItem.magnitude += effect->effectItem.magnitude;
        }

        player->RemoveItem(ingridient, 1, RE::ITEM_REMOVE_REASON::kRemove, nullptr, nullptr);

        cum_count += IsCum(ingridient);
    }

    int path = 1 + cum_count * 5;

    for(auto& [id, element] : map)
    {
        if(element.second == 1) { continue; }

        path += element.first->baseEffect->HasKeyword(kwpotion[Beneficial]) ? element.second * 5 : -((int)element.second * 3);

        element.first->effectItem.magnitude *= 0.2f * GetPerk(Perk::Alchemist) + 1.f;

        if(cum_count)
        {
            element.first->effectItem.magnitude *= (cum_count + 1);
            element.first->effectItem.duration *= (cum_count + 1);
        }

        effects[id] = element.first;
    }
    
    path = path > 0;

    std::vector<RE::FormID> erase;
    erase.reserve(12);

    for(auto& [id, effect] : effects)
    {
        if(!(effect->baseEffect->HasKeyword(kwpotion[Beneficial]) - path))
        {
            if(GetPerk(Perk::Purity) || ((uint32_t)rand() > ((RAND_MAX * GetSkill(RE::ActorValue::kAlchemy)) / 50)))
            {
                erase.push_back(id);
            }
        }
        else
        {
            // https://en.uesp.net/wiki/Skyrim:Alchemy Physician buff here
        }
    }

    for(RE::FormID id : erase) { effects.erase(id); }

    std::vector<RE::Effect*> cum_effects;

    for(auto& [id, effect] : effects)
    {
        for(auto ingridient : ingridients)
        {
            uint16_t mask = 0;
                
            for(uint32_t i = 0; i < ingridient->effects.size(); ++i)
            {
                if(id == ingridient->effects[i]->baseEffect->GetFormID())
                {
                    mask |= 1 << i;
                }
            }

            ingridient->gamedata.knownEffectFlags |= mask;

            if(mask) { ingridient->AddChange(RE::IngredientItem::ChangeFlags::kIngredientUse); }

            if(IsCum(ingridient))
            {
                cum_effects.push_back(ingridient->effects[0]);
            }
        }
    }

    for(RE::Effect* effect : cum_effects) { effects[effect->baseEffect->GetFormID()] = effect; }

    if((uint32_t)rand() > ((RAND_MAX * GetSkill(RE::ActorValue::kAlchemy)) / 100))
    {
        // add bonus effect
    }

    return effects.size();
}

RE::FormID CumAlchemist::GetID()
{
    const RE::FormID PI = esp->GetPartialIndex() << 24;
    RE::FormID ID;
    do
    {
        ID = PI | (rand() & 0xFFFF);
    }
    while(RE::TESForm::LookupByID(ID));

    return ID;
}

void CumAlchemist::AddItem(RE::TESBoundObject* item, int32_t count)
{
    Log::Notify("%s (%i) Added ", item->GetName(), count);
    player->AddObjectToContainer(item, nullptr, count, nullptr);
}

void CumAlchemist::AddExp(RE::ActorValue skill, float value)
{
    player->AddSkillExperience(skill, value);
}

void CumAlchemist::EventCreatePotion(RE::CraftingSubMenus::AlchemyMenu* menu)
{
    RE::AlchemyItem* potion = nullptr;
    std::map<RE::FormID, RE::Effect*> effects;
    if(!GetEffects(menu, effects) || !(potion = GetPotion(effects, GetID()))) { RE::PlaySound("UIAlchemyFail"); }
    else
    {
        RE::PlaySound("UIAlchemyCreatePotion");
        AddItem(potion, 1);
        AddExp(RE::ActorValue::kAlchemy, potion->GetGoldValue() * GetVariable<float>("Alchemy/Exp"));
    }

    refresh(menu, 0);
}

void CumAlchemist::EventOnLoad(std::vector<uint8_t>& buffer)
{
    ResetPotion();

    DeSerialize deserialize(buffer);
    for(size_t i = 0; i < deserialize.GetSize(); ++i)
    {
        auto [effects, id] = deserialize.GetEffects();
        GetPotion(effects, id);
    }
}

std::vector<uint8_t> CumAlchemist::EventOnSave()
{
    Serialize serialize;
    for(auto potion : potions) { serialize += Serialize(potion); }
    return serialize;
}

void CumAlchemist::EventOnCum(RE::TESForm*, bool, RE::TESForm* donor, float amount)
{
    float multiplier = 4.f;

    RE::Actor* actor = donor->As<RE::Actor>();
    RE::TESRace* race = actor->GetActorBase()->GetRace();

    RE::IngredientItem* icum = GetCum(race);
    if(!icum) { return; }

    RE::ActorPtr commanding = actor->GetCommandingActor();
    if(!commanding && commanding->IsPlayerRef())  { multiplier /= 2.f; }

    AddItem(icum, (uint32_t)sqrtf(multiplier * amount + 1.f));
}