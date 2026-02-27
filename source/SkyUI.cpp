#include <SkyUI.h>
#include <Log.h>

constexpr const char SkyUI::JOURNAL_MENU[] = "Journal Menu";
constexpr const char SkyUI::MENU_ROOT[] = "_root.ConfigPanelFader.configPanel";

constexpr const int SkyUI::STATE_DEFAULT	= 0;
constexpr const int SkyUI::STATE_RESET      = 1;
constexpr const int SkyUI::STATE_SLIDER	    = 2;
constexpr const int SkyUI::STATE_MENU		= 3;
constexpr const int SkyUI::STATE_COLOR      = 4;

constexpr const int SkyUI::OPTION_TYPE_EMPTY	= 0x00;
constexpr const int SkyUI::OPTION_TYPE_HEADER	= 0x01;
constexpr const int SkyUI::OPTION_TYPE_TEXT	    = 0x02;
constexpr const int SkyUI::OPTION_TYPE_TOGGLE	= 0x03;
constexpr const int SkyUI::OPTION_TYPE_SLIDER	= 0x04;
constexpr const int SkyUI::OPTION_TYPE_MENU	    = 0x05;
constexpr const int SkyUI::OPTION_TYPE_COLOR	= 0x06;
constexpr const int SkyUI::OPTION_TYPE_KEYMAP	= 0x07;

constexpr const int SkyUI::OPTION_FLAG_NONE		    = 0x00;
constexpr const int SkyUI::OPTION_FLAG_DISABLED	    = 0x01;
constexpr const int SkyUI::OPTION_FLAG_HIDDEN		= 0x02;
constexpr const int SkyUI::OPTION_FLAG_WITH_UNMAP	= 0x04;

constexpr const int SkyUI::LEFT_TO_RIGHT	= 1;	
constexpr const int SkyUI::TOP_TO_BOTTOM	= 2;

static RE::GFxMovieView* GetUI(const char* a_menu)
{
    auto* ui = RE::UI::GetSingleton();
    assert(ui != nullptr && "UI was nullptr");

    auto menu = ui->GetMenu(a_menu);
    assert((!menu || !menu->uiMovie) && "UI returns nullptr");

    return menu->uiMovie.get();
}

static RE::BSScript::Object* GetScript(RE::TESQuest* quest, const char* name)
{
    RE::BSTSmartPointer<RE::BSScript::Object> script;
    auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();
    auto handle = vm->GetObjectHandlePolicy()->GetHandleForObject(static_cast<RE::VMTypeID>(quest->GetFormType()), quest);
    vm->FindBoundObject(handle, name, script);
    return script.get();
}

static RE::BSScript::Variable* GetVar(RE::BSScript::Object* obj, std::string_view name)
{
    std::vector<RE::BSScript::ObjectTypeInfo*> chain;
    auto* current = obj->GetTypeInfo();
    while (current)
    {
        chain.push_back(current);
        current = current->GetParent();
    }

    std::uint32_t index = 0;
    for (int i = (int)chain.size() - 1; i >= 0; --i)
    {
        auto* vars = chain[i]->GetVariableIter();
        uint32_t count = chain[i]->GetNumVariables();
        for (uint32_t j = 0; j < count; ++j)
        {
            if (vars[j].name == name.data())
            { 
                return &obj->variables[index + j]; 
            }
        }
        index += count;
    }

    return nullptr;
}

SkyUI::SkyUI(RE::TESQuest* quest, const char* name)
{
    RE::BSScript::Object* script = GetScript(quest, name);

    ModName = GetVar(script, "::ModName_var");
    Pages   = GetVar(script, "::Pages_var");

    _state          = GetVar(script, "_state");
    _cursorPosition = GetVar(script, "_cursorPosition");
    _cursorFillMode = GetVar(script, "_cursorFillMode");
    _currentPageNum = GetVar(script, "_currentPageNum");
    _optionFlagsBuf = GetVar(script, "_optionFlagsBuf");
    _textBuf        = GetVar(script, "_textBuf");
    _strValueBuf    = GetVar(script, "_strValueBuf");
    _numValueBuf    = GetVar(script, "_numValueBuf");
    _infoText       = GetVar(script, "_infoText");
    _sliderParams   = GetVar(script, "_sliderParams");
}

void SkyUI::SetModName(RE::BSFixedString a_text)
{
    ModName->SetString(a_text);
}

void SkyUI::SetPages(const std::vector<const char*>& a_pages)
{
    const RE::BSScript::TypeInfo type{ RE::BSScript::TypeInfo::RawType::kString };

    auto* vm = RE::BSScript::Internal::VirtualMachine::GetSingleton();

    RE::BSTSmartPointer<RE::BSScript::Array> array;
    vm->CreateArray(type, (uint32_t)a_pages.size(), array);
    Pages->SetArray(array);

    for(uint32_t i = 0; i < a_pages.size(); ++i)
    {
        (*Pages->GetArray())[i].SetString(a_pages[i]);
    }
}

void SkyUI::SetCursorFillMode(int a_fillMode)
{
    if (a_fillMode == LEFT_TO_RIGHT || a_fillMode == TOP_TO_BOTTOM)
    {
        _cursorFillMode->SetSInt(a_fillMode);
    }
}

void SkyUI::SetInfoText(RE::BSFixedString a_text)
{
    _infoText->SetString(a_text);
}

int SkyUI::AddEmptyOption()
{
    return AddOption(OPTION_TYPE_EMPTY, "", "", 0, 0);
}

int SkyUI::AddHeaderOption(RE::BSFixedString a_text, int a_flags)
{
    return AddOption(OPTION_TYPE_HEADER, a_text, "", 0, a_flags);
}

int SkyUI::AddTextOption(RE::BSFixedString a_text, RE::BSFixedString a_value, int a_flags)
{
    return AddOption(OPTION_TYPE_TEXT, a_text, a_value, 0, a_flags);
}

int SkyUI::AddToggleOption(RE::BSFixedString a_text, bool a_checked, int a_flags)
{
    return AddOption(OPTION_TYPE_TOGGLE, a_text, "", a_checked, a_flags);
}

int SkyUI::AddSliderOption(RE::BSFixedString a_text, float a_value, RE::BSFixedString a_formatString, int a_flags)
{
    return AddOption(OPTION_TYPE_SLIDER, a_text, a_formatString, a_value, a_flags);
}

int SkyUI::AddMenuOption(RE::BSFixedString a_text, RE::BSFixedString a_value, int a_flags)
{
    return AddOption(OPTION_TYPE_MENU, a_text, a_value, 0, a_flags);
}

int SkyUI::AddColorOption(RE::BSFixedString a_text, int a_color, int a_flags)
{
    return AddOption(OPTION_TYPE_COLOR, a_text, "", (float)a_color, a_flags);
}

int SkyUI::AddKeyMapOption(RE::BSFixedString a_text, int a_keyCode, int a_flags)
{
    return AddOption(OPTION_TYPE_KEYMAP, a_text, "", (float)a_keyCode, a_flags);
}

int SkyUI::AddOption(int a_optionType, RE::BSFixedString a_text, RE::BSFixedString a_strValue, float a_numValue, int a_flags)
{
    if(_state->GetSInt() != STATE_RESET)
    {
        Log::Console("Cannot add option %s outside of OnPageReset()", a_text.c_str());
        return -1;
    }

    int pos = _cursorPosition->GetSInt();
    if(pos == -1)
    {
        return -1; 
    }

    (*_optionFlagsBuf->GetArray())[pos].SetSInt(a_optionType + a_flags * 0x100);
    (*_textBuf->GetArray())[pos].SetString(a_text);
    (*_strValueBuf->GetArray())[pos].SetString(a_strValue); // Potential problem
    (*_numValueBuf->GetArray())[pos].SetFloat(a_numValue);

    _cursorPosition->SetSInt(pos + _cursorFillMode->GetSInt());
    if(_cursorPosition->GetSInt() >= 128)
    {
        _cursorPosition->SetSInt(-1);
    }

    return pos + _currentPageNum->GetSInt() * 0x100;
}

// Overloads

void SkyUI::AddOption(int a_pos, int a_optionType, RE::BSFixedString a_text, RE::BSFixedString a_strValue, float a_numValue, int a_flags)
{
    if(_state->GetSInt() != STATE_RESET)
    {
        Log::Console("Cannot add option %s outside of OnPageReset()", a_text.c_str());
        return;
    }

    int pos = a_pos & 0x7F;
    if(pos == -1) { return; }

    (*_optionFlagsBuf->GetArray())[pos].SetSInt(a_optionType + a_flags * 0x100);
    (*_textBuf->GetArray())[pos].SetString(a_text);
    (*_strValueBuf->GetArray())[pos].SetString(a_strValue);
    (*_numValueBuf->GetArray())[pos].SetFloat(a_numValue);

    _cursorPosition->SetSInt(pos + _cursorFillMode->GetSInt());
    if(_cursorPosition->GetSInt() >= 128)
    {
        _cursorPosition->SetSInt(-1);
    }
}

void SkyUI::AddEmptyOption(int a_pos)
{
    AddOption(a_pos, OPTION_TYPE_EMPTY, "", "", 0, 0);
}

void SkyUI::AddHeaderOption(int a_pos, RE::BSFixedString a_text, int a_flags)
{
    AddOption(a_pos, OPTION_TYPE_HEADER, a_text, "", 0, a_flags);
}

void SkyUI::AddTextOption(int a_pos, RE::BSFixedString a_text, RE::BSFixedString a_value, int a_flags)
{
    AddOption(a_pos, OPTION_TYPE_TEXT, a_text, a_value, 0, a_flags);
}

void SkyUI::AddToggleOption(int a_pos, RE::BSFixedString a_text, bool a_checked, int a_flags)
{
    AddOption(a_pos, OPTION_TYPE_TOGGLE, a_text, "", a_checked, a_flags);
}

void SkyUI::AddSliderOption(int a_pos, RE::BSFixedString a_text, float a_value, RE::BSFixedString a_formatString, int a_flags)
{
    AddOption(a_pos, OPTION_TYPE_SLIDER, a_text, a_formatString, a_value, a_flags);
}

void SkyUI::AddMenuOption(int a_pos, RE::BSFixedString a_text, RE::BSFixedString a_value, int a_flags)
{
    AddOption(a_pos, OPTION_TYPE_MENU, a_text, a_value, 0, a_flags);
}

void SkyUI::AddColorOption(int a_pos, RE::BSFixedString a_text, int a_color, int a_flags)
{
    AddOption(a_pos, OPTION_TYPE_COLOR, a_text, "", (float)a_color, a_flags);
}

void SkyUI::AddKeyMapOption(int a_pos, RE::BSFixedString a_text, int a_keyCode, int a_flags)
{
    AddOption(a_pos, OPTION_TYPE_KEYMAP, a_text, "", (float)a_keyCode, a_flags);
}

// Setters

void SkyUI::SetToggleOptionValue(int a_option, bool a_checked, bool a_noUpdate)
{
    int index = a_option % 0x100;
    int type = (*_optionFlagsBuf->GetArray())[index].GetSInt() % 0x100;

    if (type != OPTION_TYPE_TOGGLE)
    {
		int pageIdx = (a_option / 0x100) - 1;
		if (pageIdx != -1)
        {
            Log::Console("Option type mismatch. Expected toggle option, page \"%s\", index %i", (*Pages->GetArray())[pageIdx].GetString(), index);
        }
		else
        {
            Log::Console("Option type mismatch. Expected toggle option, page \"\", index %i", index);
        }
		return;
    }

    SetOptionNumValue(index, a_checked, a_noUpdate);
}

void SkyUI::SetSliderOptionValue(int a_option, float a_value, RE::BSFixedString a_formatString, bool a_noUpdate)
{
    int index = a_option % 0x100;
    int type = (*_optionFlagsBuf->GetArray())[index].GetSInt() % 0x100;

    if (type != OPTION_TYPE_SLIDER)
    {
		int pageIdx = (a_option / 0x100) - 1;
		if (pageIdx != -1)
        {
            Log::Console("Option type mismatch. Expected slider option, page \"%s\", index %i", (*Pages->GetArray())[pageIdx].GetString(), index);
        }
		else
        {
            Log::Console("Option type mismatch. Expected slider option, page \"\", index %i", index);
        }
		return;
    }

    SetOptionValues(index, a_formatString, a_value, a_noUpdate);
}

void SkyUI::SetTextOptionValue(int a_option, RE::BSFixedString a_value, bool a_noUpdate)
{
    int index = a_option % 0x100;
    int type = (*_optionFlagsBuf->GetArray())[index].GetSInt() % 0x100;

    if (type != OPTION_TYPE_TEXT)
    {
		int pageIdx = (a_option / 0x100) - 1;
		if (pageIdx != -1)
        {
            Log::Console("Option type mismatch. Expected text option, page  \"%s\", index %i", (*Pages->GetArray())[pageIdx].GetString(), index);
        }
		else
        {
            Log::Console("Option type mismatch. Expected text option, page  \"\", index %i", index);
        }
		return;
    }

    SetOptionStrValue(index, a_value, a_noUpdate);
}   

void SkyUI::SetSliderDialogValue(float a_value, float a_default, float a_minValue, float a_maxValue, float a_interval)
{
    if (_state->GetSInt() != STATE_SLIDER)
    {
        Log::Console("Cannot set slider dialog params while outside OnOptionSliderOpen()");
        return;
    }

    (*_sliderParams->GetArray())[0].SetFloat(a_value);
    (*_sliderParams->GetArray())[1].SetFloat(a_default);
    (*_sliderParams->GetArray())[2].SetFloat(a_minValue);
    (*_sliderParams->GetArray())[3].SetFloat(a_maxValue);
    (*_sliderParams->GetArray())[4].SetFloat(a_interval);
}

template <std::size_t N1, std::size_t N2>
static constexpr auto concat(const char (&a)[N1], const char (&b)[N2]) {
    std::array<char, N1 + N2 - 1> result{};
    for (std::size_t i = 0; i < N1 - 1; ++i) result[i] = a[i];
    for (std::size_t i = 0; i < N2;     ++i) result[N1 - 1 + i] = b[i];
    return result;
}

void SkyUI::SetOptionNumValue(int a_index, float a_numValue, bool a_noUpdate)
{
    constexpr auto MENU_ROOT_OPTIONCURSORINDEX = concat(MENU_ROOT, ".optionCursorIndex");
    constexpr auto MENU_ROOT_OPTIONCURSORNUMVALUE = concat(MENU_ROOT, ".optionCursor.numValue");
    constexpr auto MENU_ROOT_INVALIDATEOPTIONDATA = concat(MENU_ROOT, ".invalidateOptionData");

    auto* UI = GetUI(JOURNAL_MENU);

    RE::GFxValue index{ a_index };
    UI->SetVariable(MENU_ROOT_OPTIONCURSORINDEX.data(), index);

    RE::GFxValue numValue{ a_numValue };
    UI->SetVariable(MENU_ROOT_OPTIONCURSORNUMVALUE.data(), numValue);

    if(!a_noUpdate) { UI->Invoke(MENU_ROOT_INVALIDATEOPTIONDATA.data(), nullptr, nullptr, 0); }
}

void SkyUI::SetOptionStrValue(int a_index, RE::BSFixedString a_strValue, bool a_noUpdate)
{
    constexpr auto MENU_ROOT_OPTIONCURSORINDEX = concat(MENU_ROOT, ".optionCursorIndex");
    constexpr auto MENU_ROOT_OPTIONCURSOR_STRVALUE = concat(MENU_ROOT, ".optionCursor.strValue");
    constexpr auto MENU_ROOT_INVALIDATEOPTIONDATA = concat(MENU_ROOT, ".invalidateOptionData");

	if (_state->GetSInt() == STATE_RESET)
    {
		Log::Console("Cannot modify option data while in OnPageReset()");
		return;
    }

    auto* UI = GetUI(JOURNAL_MENU);

    RE::GFxValue index{ a_index };
    UI->SetVariable(MENU_ROOT_OPTIONCURSORINDEX.data(), index);

    RE::GFxValue strValue{ a_strValue };
    UI->SetVariable(MENU_ROOT_OPTIONCURSOR_STRVALUE.data(), strValue);

	if (!a_noUpdate)
    {
        UI->Invoke(MENU_ROOT_INVALIDATEOPTIONDATA.data(), nullptr, nullptr, 0);
    }
}

void SkyUI::SetOptionValues(int a_index, RE::BSFixedString a_strValue, float a_numValue, bool a_noUpdate)
{
    constexpr auto MENU_ROOT_OPTIONCURSORINDEX = concat(MENU_ROOT, ".optionCursorIndex");
    constexpr auto MENU_ROOT_OPTIONCURSOR_STRVALUE = concat(MENU_ROOT, ".optionCursor.strValue");
    constexpr auto MENU_ROOT_OPTIONCURSORNUMVALUE = concat(MENU_ROOT, ".optionCursor.numValue");
    constexpr auto MENU_ROOT_INVALIDATEOPTIONDATA = concat(MENU_ROOT, ".invalidateOptionData");

	if (_state->GetSInt() == STATE_RESET)
    {
		Log::Console("Cannot modify option data while in OnPageReset()");
		return;
    }

    auto* UI = GetUI(JOURNAL_MENU);

    RE::GFxValue index{ a_index };
    UI->SetVariable(MENU_ROOT_OPTIONCURSORINDEX.data(), index);

    RE::GFxValue strValue{ a_strValue };
    UI->SetVariable(MENU_ROOT_OPTIONCURSOR_STRVALUE.data(), strValue);

    RE::GFxValue numValue{ a_numValue };
    UI->SetVariable(MENU_ROOT_OPTIONCURSORNUMVALUE.data(), numValue);

	if (!a_noUpdate)
    {
        UI->Invoke(MENU_ROOT_INVALIDATEOPTIONDATA.data(), nullptr, nullptr, 0);
    }
}

// Image

void SkyUI::LoadCustomContent(RE::BSFixedString a_source, float a_x, float a_y)
{
    constexpr auto MENU_ROOT_SETCUSTOMCONTENTPARAMS = concat(MENU_ROOT, ".setCustomContentParams");
    constexpr auto MENU_ROOT_LOADCUSTOMCONTENT = concat(MENU_ROOT, ".loadCustomContent");

    auto* UI = GetUI(JOURNAL_MENU);
    RE::GFxValue params[2] = { a_x, a_y };
    UI->Invoke(MENU_ROOT_SETCUSTOMCONTENTPARAMS.data(), nullptr, params, 2);

    RE::GFxValue source{ a_source.c_str() };
    UI->Invoke(MENU_ROOT_LOADCUSTOMCONTENT.data(), nullptr, &source, 1);
}

void SkyUI::UnloadCustomContent()
{
    constexpr auto MENU_ROOT_UNLOADCUSTOMCONTENT = concat(MENU_ROOT, ".unloadCustomContent");

    auto* UI = GetUI(JOURNAL_MENU);
    UI->Invoke(MENU_ROOT_UNLOADCUSTOMCONTENT.data(), nullptr, nullptr, 0);
}