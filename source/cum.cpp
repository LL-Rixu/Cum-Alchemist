#include <Cum.h>

Cum* Cum::cum = nullptr;

void Cum::OnCum(RE::StaticFunctionTag*, RE::TESForm* actor, bool swallowed, RE::BSFixedString, RE::TESForm* cumDonor, float cumAmount, int)
{
    if(!cum) { return; }
    cum->EventOnCum(actor, swallowed, cumDonor, cumAmount);
}

Cum::Cum()
{
    cum = this;

    RE::TESDataHandler* datahandler = RE::TESDataHandler::GetSingleton();

	kwcum[Cum::Human]	 = {RE::TESForm::LookupByEditorID<RE::BGSKeyword>("CmAl_HumanRace"), datahandler->LookupForm<RE::IngredientItem>(0xD62, "Cum Alchemist.esp")};
	kwcum[Cum::Elf]		 = {RE::TESForm::LookupByEditorID<RE::BGSKeyword>("CmAl_ElfRace"), datahandler->LookupForm<RE::IngredientItem>(0xD63, "Cum Alchemist.esp")};
	kwcum[Cum::Argonian] = {RE::TESForm::LookupByEditorID<RE::BGSKeyword>("CmAl_ArgonianRace"), datahandler->LookupForm<RE::IngredientItem>(0xD65, "Cum Alchemist.esp")};
	kwcum[Cum::Khajiit]	 = {RE::TESForm::LookupByEditorID<RE::BGSKeyword>("CmAl_KhajiitRace"), datahandler->LookupForm<RE::IngredientItem>(0xD67, "Cum Alchemist.esp")};
	kwcum[Cum::Canine]	 = {RE::TESForm::LookupByEditorID<RE::BGSKeyword>("CmAl_CanineRace"), datahandler->LookupForm<RE::IngredientItem>(0xD69, "Cum Alchemist.esp")};
	kwcum[Cum::Arachnid] = {RE::TESForm::LookupByEditorID<RE::BGSKeyword>("CmAl_ArachnidRace"), datahandler->LookupForm<RE::IngredientItem>(0xD6B, "Cum Alchemist.esp")};
	kwcum[Cum::Dragon]	 = {RE::TESForm::LookupByEditorID<RE::BGSKeyword>("CmAl_DragonRace"), datahandler->LookupForm<RE::IngredientItem>(0xD6D, "Cum Alchemist.esp")};
	kwcum[Cum::Beast]	 = {RE::TESForm::LookupByEditorID<RE::BGSKeyword>("CmAl_DragonRace"), datahandler->LookupForm<RE::IngredientItem>(0xD6F, "Cum Alchemist.esp")};
	kwcum[Cum::Giant]    = {RE::TESForm::LookupByEditorID<RE::BGSKeyword>("CmAl_BeastRace"), datahandler->LookupForm<RE::IngredientItem>(0xD71, "Cum Alchemist.esp")};
	kwcum[Cum::Rotten]	 = {RE::TESForm::LookupByEditorID<RE::BGSKeyword>("CmAl_GiantRace"), datahandler->LookupForm<RE::IngredientItem>(0xD73, "Cum Alchemist.esp")};
	kwcum[Cum::Arcane]	 = {RE::TESForm::LookupByEditorID<RE::BGSKeyword>("CmAl_ArcaneRace"), datahandler->LookupForm<RE::IngredientItem>(0xD75, "Cum Alchemist.esp")};

    cummap = 
    {
        kwcum[Cum::Human].second,
        kwcum[Cum::Elf].second,
        kwcum[Cum::Argonian].second,
        kwcum[Cum::Khajiit].second,
        kwcum[Cum::Canine].second,
        kwcum[Cum::Arachnid].second,
        kwcum[Cum::Dragon].second,
        kwcum[Cum::Beast].second,
        kwcum[Cum::Giant].second,
        kwcum[Cum::Rotten].second,
        kwcum[Cum::Arcane].second,
    };

	SKSE::GetPapyrusInterface()->Register([](RE::BSScript::IVirtualMachine* vm)
	{
		vm->RegisterFunction("CmAl_CSN_native_handler", "CmAl_CSN_interface", OnCum);

		return true;
	});
}

Cum::~Cum() { cum = nullptr; }

RE::IngredientItem* Cum::GetCum(RE::TESRace* race)
{
	for(size_t i = 0; i < kwcum.size(); ++i)
	{
		if(race->HasKeyword(kwcum[i].first)) { return kwcum[i].second; }
	}

	return nullptr;
}

bool Cum::IsCum(RE::IngredientItem* a_cum) { return cummap.contains(a_cum); }
