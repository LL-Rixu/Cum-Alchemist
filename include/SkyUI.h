#pragma once
#include <PCH.h>

class SkyUI
{
    RE::BSScript::Variable* ModName;
    RE::BSScript::Variable* Pages;

    // int
    RE::BSScript::Variable* _state;
    RE::BSScript::Variable* _cursorPosition;
    RE::BSScript::Variable* _cursorFillMode;
    RE::BSScript::Variable* _currentPageNum;

    // Array
    RE::BSScript::Variable* _optionFlagsBuf;
    RE::BSScript::Variable* _textBuf;
    RE::BSScript::Variable* _strValueBuf;
    RE::BSScript::Variable* _numValueBuf;
    RE::BSScript::Variable* _sliderParams;
    
    // String
    RE::BSScript::Variable* _infoText;

public:
    SkyUI(RE::TESQuest* quest, const char* name);
    ~SkyUI() = default;

    void SetModName(RE::BSFixedString a_text);
    void SetPages(const std::vector<const char*>& a_pages);

    void SetCursorFillMode(int a_fillMode);
    void SetInfoText(RE::BSFixedString a_text);

    int AddOption(int a_optionType, RE::BSFixedString a_text, RE::BSFixedString a_strValue, float a_numValue, int a_flags);
    
    int AddEmptyOption();
    int AddHeaderOption(RE::BSFixedString a_text, int a_flags = 0);
    int AddTextOption(RE::BSFixedString a_text, RE::BSFixedString a_value, int a_flags = 0);
    int AddToggleOption(RE::BSFixedString a_text, bool a_checked, int a_flags = 0);
    int AddSliderOption(RE::BSFixedString a_text, float a_value, RE::BSFixedString a_formatString = "{0}", int a_flags = 0);
    int AddMenuOption(RE::BSFixedString a_text, RE::BSFixedString a_value, int a_flags = 0);
    int AddColorOption(RE::BSFixedString a_text, int a_color, int a_flags = 0);
    int AddKeyMapOption(RE::BSFixedString a_text, int a_keyCode, int a_flags = 0);

    void AddOption(int a_position, int a_optionType, RE::BSFixedString a_text, RE::BSFixedString a_strValue, float a_numValue, int a_flags);

    void AddEmptyOption(int a_position);
    void AddHeaderOption(int a_position, RE::BSFixedString a_text, int a_flags = 0);
    void AddTextOption(int a_position, RE::BSFixedString a_text, RE::BSFixedString a_value, int a_flags = 0);
    void AddToggleOption(int a_position, RE::BSFixedString a_text, bool a_checked, int a_flags = 0);
    void AddSliderOption(int a_position, RE::BSFixedString a_text, float a_value, RE::BSFixedString a_formatString = "{0}", int a_flags = 0);
    void AddMenuOption(int a_position, RE::BSFixedString a_text, RE::BSFixedString a_value, int a_flags = 0);
    void AddColorOption(int a_position, RE::BSFixedString a_text, int a_color, int a_flags = 0);
    void AddKeyMapOption(int a_position, RE::BSFixedString a_text, int a_keyCode, int a_flags = 0);

    void LoadCustomContent(RE::BSFixedString a_source, float a_x = 0.0, float a_y = 0.0);
    void UnloadCustomContent();

    void SetToggleOptionValue(int a_option, bool a_checked, bool a_noUpdate = false);
    void SetSliderOptionValue(int a_option, float a_value, RE::BSFixedString a_formatString = "{0}", bool a_noUpdate = false);
    void SetTextOptionValue(int a_option, RE::BSFixedString a_value, bool a_noUpdate = false);

    void SetSliderDialogValue(float a_value, float a_default, float a_minValue, float a_maxValue, float a_interval);
    

    void SetOptionNumValue(int a_index, float a_numValue, bool a_noUpdate);
    void SetOptionStrValue(int a_index, RE::BSFixedString a_strValue, bool a_noUpdate);
    void SetOptionValues(int a_index, RE::BSFixedString a_strValue, float a_numValue, bool a_noUpdate);

    static const char JOURNAL_MENU[];
    static const char MENU_ROOT[];

    static const int STATE_DEFAULT;
    static const int STATE_RESET;
    static const int STATE_SLIDER;
    static const int STATE_MENU;
    static const int STATE_COLOR;

    static const int OPTION_TYPE_EMPTY;
    static const int OPTION_TYPE_HEADER;
    static const int OPTION_TYPE_TEXT;
    static const int OPTION_TYPE_TOGGLE;
    static const int OPTION_TYPE_SLIDER;
    static const int OPTION_TYPE_MENU;
    static const int OPTION_TYPE_COLOR;
    static const int OPTION_TYPE_KEYMAP;

    static const int OPTION_FLAG_NONE;
    static const int OPTION_FLAG_DISABLED;
    static const int OPTION_FLAG_HIDDEN;
    static const int OPTION_FLAG_WITH_UNMAP;

    static const int LEFT_TO_RIGHT;
    static const int TOP_TO_BOTTOM;
};