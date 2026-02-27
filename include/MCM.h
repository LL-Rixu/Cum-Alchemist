#pragma once
#include <PCH.h>
#include <Log.h>

struct Option
{
    enum Type
    {
        Empty,
        Header,
        Text,
        Toggle,
        Slider,
        Menu,
        Color,
        Keymap,

        Image
    };

    Option() = default;
    Option(Option::Type _type, const char* a_text, int a_flags): type(_type), text(a_text), flags(a_flags) {};
    ~Option() = default;

    Type type = Option::Empty;
    const char* text = nullptr;
    const char* description = nullptr;
    const char* format = nullptr;
    int flags = 0;

    float min, max, interval;

    std::function<void(Option&)> callback;

    template<typename T>
    operator T&()
    {
        //if constexpr (!std::is_same_v<T, Toggle> && !std::is_same_v<T, Text> && !std::is_same_v<T, Slider>) { static_assert("Illegal Cast"); }
        return *static_cast<T*>(this);
    }

    template<typename T>
    T& GetData()
    {
        return *reinterpret_cast<T*>(&value);
    }

    Option& Description(const char* a_desc)
    {
        description = a_desc;
        return *this;
    };

    Option& Callback(std::function<void(Option&)> a_callback) 
    {
        callback = a_callback;
        return *this;
    }

    void Reset() 
    {
        if(type == Option::Text) { return; }

        value = reset; 
    }

protected:
    union Storage
    {
        float _float; // slider
        bool _bool; // toggle
        const char* _string; // text
    };

    Storage value;
    Storage reset;
};

struct Empty : public Option
{
    Empty(): Option() {};
};

struct Header : public Option
{
    Header(const char* a_text, int a_flags = 0): Option(Option::Header, a_text, a_flags) {};
};

struct Text : public Option
{
    Text(const char* a_text, const char* a_value, int a_flags = 0): Option(Option::Text, a_text, a_flags) 
    {
        char* tmp = new char[std::strlen(a_value) + 1];
        //strcpy_s(tmp, std::strlen(a_value), a_value);
        memcpy(tmp, a_value, std::strlen(a_value) + 1);
        value._string = tmp;

        reset = value;
    };

    void Set(const char* v) 
    {
        if(value._string != nullptr) { delete value._string; }
        char* tmp = new char[std::strlen(v) + 1];
        memcpy(tmp, v, std::strlen(v) + 1);
        value._string = tmp;
    }
    const char*& Get() { return value._string; }
};

struct Toggle : public Option
{
    Toggle(const char* a_text, bool a_checked, int a_flags = 0): Option(Option::Toggle, a_text, a_flags) { value._bool = a_checked; reset = value; };

    void Set(const bool v) { value._bool = v; }
    bool& Get() { return value._bool; }
};

struct Slider : public Option
{
    Slider(const char* a_text, float a_value, float a_default, float a_min, float a_max, float a_interval, const char* a_format = "{0}", int a_flags = 0): Option(Option::Slider, a_text, a_flags) 
    { 
        value._float = a_value;
        format = a_format;

        min = a_min;
        max = a_max;
        interval = a_interval;
        reset._float = a_default;
    };

    void Set(const float v) { value._float = v; }
    float& Get() { return value._float; }
    float& Default() { return reset._float; }
};

struct Image : public Option
{
    Image(const char* a_path, float a_x = 0.f, float a_y = 0.f): Option(Option::Image, a_path, 0) { min = a_x; max = a_y; }
};

struct Page
{
    const char* const name;
    std::unordered_map<int, Option> options;
};

class MCM
{
    static MCM* mcm;
    static Page pages[3];
    static std::unordered_map<std::string_view, int> lookUp;

    static void OnConfigInit(RE::TESQuest*);
    static void OnPageReset(RE::TESQuest*, RE::BSFixedString);
    static void OnOptionSelect(RE::TESQuest*, int);
    static void OnOptionHighLight(RE::TESQuest*, int);
    static void OnOptionDefault(RE::TESQuest*, int);
    static void OnOptionSliderOpen(RE::TESQuest*, int);
    static void OnOptionSliderAccept(RE::TESQuest*, int, float);

    static float GetFloat(RE::StaticFunctionTag*, RE::BSFixedString);
    static bool GetBool(RE::StaticFunctionTag*, RE::BSFixedString);
    static RE::BSFixedString GetString(RE::StaticFunctionTag*, RE::BSFixedString);

    static Page& GetPage(const char* const);
    static void Save();
    static void Load();

    void PageReset(RE::TESQuest*, RE::BSFixedString);
    void OptionSelect(RE::TESQuest*, int);
    void OptionHighLight(RE::TESQuest*, int);
    void OptionDefault(RE::TESQuest*, int);
    void OptionSliderOpen(RE::TESQuest*, int);
    void OptionSliderAccept(RE::TESQuest*, int, float);
public:
    MCM();
    ~MCM();

    template<typename T>
    static T& GetVariable(const char* name)
    {
        int pid = lookUp[name];
        return pages[pid >> 8].options[pid & 0x7F].GetData<T>();
    }

    static Option& GetOption(int pid) { return pages[pid >> 8].options[pid & 0x7F]; }
};
