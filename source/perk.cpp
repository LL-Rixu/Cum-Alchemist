#include <Perk.h>

static const RE::FormID idAlchemist[5] = { 0xBE127, 0xC07CA, 0xC07CB, 0xC07CC, 0xC07CD };
static const RE::FormID idPhysician = 0x58215;
static const RE::FormID idBenefactor = 0x58216;
static const RE::FormID idPoisoner = 0x58217;
static const RE::FormID idPurity = 0x5821D;

Perk::Perk() 
{
	player = RE::PlayerCharacter::GetSingleton();

    RE::TESDataHandler* datahandler = RE::TESDataHandler::GetSingleton();
    if (!datahandler) { return; }

	for(size_t i = 0; i < alchemist.size(); ++i)
	{
		alchemist[i] = datahandler->LookupForm<RE::BGSPerk>(idAlchemist[i], "Skyrim.esm");
	}

	physician = datahandler->LookupForm<RE::BGSPerk>(idPhysician, "Skyrim.esm");
	benefactor = datahandler->LookupForm<RE::BGSPerk>(idBenefactor, "Skyrim.esm");
	poisoner = datahandler->LookupForm<RE::BGSPerk>(idPoisoner, "Skyrim.esm");
	purity = datahandler->LookupForm<RE::BGSPerk>(idPurity, "Skyrim.esm");
};

uint32_t Perk::GetSkill(RE::ActorValue skill)
{
	return static_cast<uint32_t>(player->GetActorValue(skill));
}

uint32_t Perk::GetPerk(Perk::Alchemy type)
{
	uint32_t value = 0;

	switch(type)
	{
	case Perk::Alchemist: 
		for(auto perk : alchemist) { value += player->HasPerk(perk); }
	break;

	case Perk::Physician:
		value = player->HasPerk(physician);
	break;

	case Perk::Benefactor:
		value = player->HasPerk(benefactor);
	break;

	case Perk::Poisoner:
		value = player->HasPerk(poisoner);
	break;

	case Perk::Purity:
		value = player->HasPerk(purity);
	break;

	}

	return value;
}