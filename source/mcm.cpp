#include <MCM.h>
#include <SkyUI.h>

#include <string>

MCM* MCM::mcm = nullptr;

MCM::MCM()
{
    mcm = this;

	SKSE::GetPapyrusInterface()->Register([](RE::BSScript::IVirtualMachine* vm)
	{
        vm->RegisterFunction("ConfigInit",         "CmAl_MCMQuestScript", MCM::OnConfigInit);
        vm->RegisterFunction("PageReset",          "CmAl_MCMQuestScript", MCM::OnPageReset);
        vm->RegisterFunction("OptionSelect",       "CmAl_MCMQuestScript", MCM::OnOptionSelect);
        vm->RegisterFunction("OptionHighLight",    "CmAl_MCMQuestScript", MCM::OnOptionHighLight);
        vm->RegisterFunction("OptionDefault",      "CmAl_MCMQuestScript", MCM::OnOptionDefault);
        vm->RegisterFunction("OptionSliderOpen",   "CmAl_MCMQuestScript", MCM::OnOptionSliderOpen);
        vm->RegisterFunction("OptionSliderAccept", "CmAl_MCMQuestScript", MCM::OnOptionSliderAccept);

        vm->RegisterFunction("GetFloat",    "CmAl", MCM::GetFloat);
        vm->RegisterFunction("GetBool",     "CmAl", MCM::GetBool);
        vm->RegisterFunction("GetString",   "CmAl", MCM::GetString);

        return true;
	});

    Load();
}

MCM::~MCM() { mcm = nullptr; }

void MCM::OnConfigInit(RE::TESQuest* quest)
{
    SkyUI skyui(quest, "CmAl_MCMQuestScript");

    skyui.SetModName("Cum Alchemist");

    std::vector<const char*> names;
    for(const Page& page : pages) 
    {
        if(page.name[0] == 0) { continue; }

        names.push_back(page.name);
    }
    skyui.SetPages(names);
}

void MCM::OnPageReset(RE::TESQuest* quest, RE::BSFixedString page)
{
    if(!mcm) { return; }
    mcm->PageReset(quest, page);
}

void MCM::OnOptionSelect(RE::TESQuest* quest, int id)
{
    if(!mcm) { return; }
    mcm->OptionSelect(quest, id);
}

void MCM::OnOptionHighLight(RE::TESQuest* quest, int id)
{
    if(!mcm) { return; }
    mcm->OptionHighLight(quest, id);
}

void MCM::OnOptionDefault(RE::TESQuest* quest, int id)
{
    if(!mcm) { return; }
    mcm->OptionDefault(quest, id);
}

void MCM::OnOptionSliderOpen(RE::TESQuest* quest, int id)
{
    if(!mcm) { return; }
    mcm->OptionSliderOpen(quest, id);
}

void MCM::OnOptionSliderAccept(RE::TESQuest* quest, int id, float value)
{
    if(!mcm) { return; }
    mcm->OptionSliderAccept(quest, id, value);
}

float MCM::GetFloat(RE::StaticFunctionTag*, RE::BSFixedString name)
{
    if(!mcm) { return NAN; }

    return mcm->GetVariable<float>(name.c_str());
}

bool MCM::GetBool(RE::StaticFunctionTag*, RE::BSFixedString name)
{
    if(!mcm) { return false; }

    return mcm->GetVariable<bool>(name.c_str());
}

RE::BSFixedString MCM::GetString(RE::StaticFunctionTag*, RE::BSFixedString name)
{
    if(!mcm) { return ""; }

    return mcm->GetVariable<const char*>(name.c_str());
}

Page& MCM::GetPage(const char* const name)
{
    for(Page& page : pages)
    {
        if(!strcmp(page.name, name)) { return page; }
    }

    return pages[0];
}

void MCM::PageReset(RE::TESQuest* quest, RE::BSFixedString name)
{
    SkyUI skyui(quest, "CmAl_MCMQuestScript");
    skyui.UnloadCustomContent();

    Page& page = GetPage(name.c_str());

    for(auto& [position, option] : page.options)
    {
        switch(option.type)
        {
        case Option::Empty:
            skyui.AddEmptyOption(position);
        break;
        case Option::Header:
            skyui.AddHeaderOption(position, option.text, option.flags);
        break;
        case Option::Text:
        {
            Text& text = option;
            skyui.AddTextOption(position, text.text, text.Get(), text.flags);
        }
        break;
        case Option::Toggle:
        {
            Toggle& toggle = option;
            skyui.AddToggleOption(position, toggle.text, toggle.Get(), toggle.flags);
        }
        break;
        case Option::Slider:
        {
            Slider& slider = option;
            skyui.AddSliderOption(position, slider.text, slider.Get(), slider.format, slider.flags);
        }
        break;
        case Option::Image:
        {
            Image& image = option;
            skyui.LoadCustomContent(image.text, image.min, image.max);
        }
        break;
        default: break;
        }
    }
}

void MCM::OptionSelect(RE::TESQuest* quest, int id)
{
    SkyUI skyui(quest, "CmAl_MCMQuestScript");

    Option& option = pages[id >> 8].options[id & 0x7F];

    if(option.callback) { option.callback(option); }

    switch(option.type)
    {
    case Option::Text:
    {
        Text& text = option;
        skyui.SetTextOptionValue(id, text.Get());
    }
    case Option::Toggle:
    {
        Toggle& toggle = option;
        bool& state = toggle.Get();
        state = !state;
        skyui.SetToggleOptionValue(id, state);
    }
    default: break;
    }

    Save();
}

void MCM::OptionHighLight(RE::TESQuest* quest, int id)
{
    SkyUI skyui(quest, "CmAl_MCMQuestScript");

    Option& option = pages[id >> 8].options[id & 0x7F];
    if(!option.description) { return; }
    skyui.SetInfoText(option.description);
}

void MCM::OptionDefault(RE::TESQuest*, int id)
{
    Option& option = pages[id >> 8].options[id & 0x7F];
    option.Reset();

    Save();
}

void MCM::OptionSliderOpen(RE::TESQuest* quest, int id)
{
    SkyUI skyui(quest, "CmAl_MCMQuestScript");
    Slider& slider = pages[id >> 8].options[id & 0x7F];
    skyui.SetSliderDialogValue(slider.Get(), slider.Default(), slider.min, slider.max, slider.interval);
}

void MCM::OptionSliderAccept(RE::TESQuest* quest, int id, float value)
{
    SkyUI skyui(quest, "CmAl_MCMQuestScript");
    Slider& slider = pages[id >> 8].options[id & 0x7F];

    slider.Set(value);
    skyui.SetSliderOptionValue(id, slider.Get(), slider.format);

    Save();
}

void MCM::Save()
{
    std::ofstream file("Data\\MCM.config");

    file << "Cum Alchemist MCM storage file\nYou can edit Cum Alchemist MCM settings here\n\n";

    for(auto& [key, pid] : lookUp)
    {
        Option& option = GetOption(pid);
        switch(option.type)
        {
        case Option::Text:
        {
            Text& text = option;
            file << key << ":" << text.Get() << "\n";
        }
        break;
        case Option::Toggle:
        {
            Toggle& toggle = option;
            file << key << ":" << std::to_string(toggle.Get()) << "\n";
        }
        break;
        case Option::Slider:
        {
            Slider& slider = option;
            file << key << ":" << std::to_string(slider.Get()) << "\n";
        }
        break;

        default: break;
        }
    }

    file.close();
}

void MCM::Load()
{
    std::ifstream file("Data\\MCM.config");

    std::string line;
    while(std::getline(file, line))
    {
        for(auto& [key, pid] : lookUp)
        {
            if(!line.contains(key)) { continue; }

            Option& option = GetOption(pid);

            std::string value = line.substr(line.find(':') + 1);

            switch(option.type)
            {
            case Option::Text:
            {
                Text& text = option;
                text.Set(value.c_str()); 
            }
            break;
            case Option::Toggle:
            {
                Toggle& toggle = option;
                toggle.Set(std::stoi(value));
            }
            break;
            case Option::Slider:
            {
                Slider& slider = option;
                slider.Set(std::stof(value));
            }
            break;

            default: break;
            }
        }
    }

    file.close();
}