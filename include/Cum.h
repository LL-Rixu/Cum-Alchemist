#pragma once
#include <PCH.h>

class Cum
{
	static Cum* cum;
    std::array<std::pair<const RE::BGSKeyword*, RE::IngredientItem*>, 11> kwcum;
    std::unordered_set<RE::IngredientItem*> cummap;

	static void OnCum(RE::StaticFunctionTag*, RE::TESForm* actor, bool swallowed, RE::BSFixedString, RE::TESForm* cumDonor, float cumAmount, int);
public:
	enum Type
	{
		Human,
		Elf,
		Argonian,
		Khajiit,
		Canine,
		Arachnid,
		Dragon,
		Beast,
		Giant,
		Rotten,
		Arcane
	};

    RE::IngredientItem* GetCum(RE::TESRace*);
    bool IsCum(RE::IngredientItem*);

	virtual void EventOnCum(RE::TESForm* actor, bool swallowed, RE::TESForm* donor, float amount) = 0; 

	Cum();
	~Cum();
};