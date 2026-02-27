#include <MCM.h>

#define POSITION(R, C) (2*R + C)
#define INDEX(page, id) ((page << 8) | id)

Page MCM::pages[3] =
{
    {
        .name = "",

        .options = 
        {
            { POSITION(0, 0), Image("CmAl.dds") }
        },
    },

    {
        .name = "Alchemy",

        .options =
        {
            { POSITION(0, 0),   Header("Alchemy") },
            { POSITION(1, 0),   Toggle("Enable", true).Description("Not implmented") },
            { POSITION(2, 0),   Slider("Exp multiplier", 1.f, 1.f, 0.f, 2.f, 0.05f, "{2}").Description("Alchemy Experience multiplier") },

            { POSITION(0,  1),  Header("Inflation") },
            { POSITION(1,  1),  Slider("Human",    0.5f, 0.5f, 0.f, 2.f, 0.05f, "{2}").Description("FHU inflation gained from drinking human cum")    },
            { POSITION(2,  1),  Slider("Elf",      0.5f, 0.5f, 0.f, 2.f, 0.05f, "{2}").Description("FHU inflation gained from drinking Elf cum")      },
            { POSITION(3,  1),  Slider("Khajiit",  0.5f, 0.5f, 0.f, 2.f, 0.05f, "{2}").Description("FHU inflation gained from drinking Khajiit cum")  },
            { POSITION(4,  1),  Slider("Argonian", 0.5f, 0.5f, 0.f, 2.f, 0.05f, "{2}").Description("FHU inflation gained from drinking Argonian cum") },
            { POSITION(5,  1),  Slider("Canine",   0.5f, 0.5f, 0.f, 2.f, 0.05f, "{2}").Description("FHU inflation gained from drinking Canine cum")   },
            { POSITION(6,  1),  Slider("Arachnid", 0.5f, 0.5f, 0.f, 2.f, 0.05f, "{2}").Description("FHU inflation gained from drinking Arachnid cum") },
            { POSITION(7,  1),  Slider("Dragon",   0.5f, 0.5f, 0.f, 2.f, 0.05f, "{2}").Description("FHU inflation gained from drinking Dragon cum")   },
            { POSITION(8,  1),  Slider("Beast",    0.5f, 0.5f, 0.f, 2.f, 0.05f, "{2}").Description("FHU inflation gained from drinking Beast cum")    },
            { POSITION(9,  1),  Slider("Giant",    0.5f, 0.5f, 0.f, 2.f, 0.05f, "{2}").Description("FHU inflation gained from drinking Giant cum")    },
            { POSITION(10, 1),  Slider("Rotten",   0.5f, 0.5f, 0.f, 2.f, 0.05f, "{2}").Description("FHU inflation gained from drinking Rotten cum")   },
            { POSITION(11, 1),  Slider("Arcane",   0.5f, 0.5f, 0.f, 2.f, 0.05f, "{2}").Description("FHU inflation gained from drinking Arcane cum")   },
        },
    },

    {
        .name = "Debug",

        .options =
        {
            { POSITION(0, 0), Header("Debug") },
            { POSITION(1, 0), Text("Load MCM", "Click").Description("Loads MCM config live from Data/CmAl.config").Callback([](Option&) { MCM::Load(); }) }
        },
    },
};

std::unordered_map<std::string_view, int> MCM::lookUp = 
{
    { "Alchemy/Enabled",    INDEX(1, POSITION(1,  0)) },
    { "Alchemy/Exp",        INDEX(1, POSITION(2,  0)) },

    { "Inflation/Human",    INDEX(1, POSITION(1,  1)) },
    { "Inflation/Elf",      INDEX(1, POSITION(2,  1)) },
    { "Inflation/Khajiit",  INDEX(1, POSITION(3,  1)) },
    { "Inflation/Argonian", INDEX(1, POSITION(4,  1)) },
    { "Inflation/Canine",   INDEX(1, POSITION(5,  1)) },
    { "Inflation/Arachnid", INDEX(1, POSITION(6,  1)) },
    { "Inflation/Dragon",   INDEX(1, POSITION(7,  1)) },
    { "Inflation/Beast",    INDEX(1, POSITION(8,  1)) },
    { "Inflation/Giant",    INDEX(1, POSITION(9,  1)) },
    { "Inflation/Rotten",   INDEX(1, POSITION(10, 1)) },
    { "Inflation/Arcane",   INDEX(1, POSITION(11, 1)) },
};