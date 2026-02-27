#pragma once
#include <PCH.h>

class Perk
{
	RE::PlayerCharacter* player;

	std::array<RE::BGSPerk*, 5> alchemist = {};
	RE::BGSPerk* physician = nullptr;
	RE::BGSPerk* benefactor = nullptr;
	RE::BGSPerk* poisoner = nullptr;
	RE::BGSPerk* purity = nullptr;

public:
	enum Alchemy { Alchemist, Physician, Benefactor, Poisoner, Purity };

	Perk();
	~Perk() = default;

	uint32_t GetSkill(RE::ActorValue);
	uint32_t GetPerk(Perk::Alchemy);
};