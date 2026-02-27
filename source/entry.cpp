#include <CumAlchemist.h>

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    SKSE::Init(skse);

    SKSE::GetMessagingInterface()->RegisterListener(CumAlchemist::interface);

	return true;
}

SKSEPluginVersion = std::invoke([]()
{
	SKSE::PluginVersionData version;

	version.PluginVersion(REL::Version(0, 0, 1));
	version.PluginName("CumAlchemist");
	version.AuthorName("Rixu");
	version.AuthorEmail("HelloWorld@gmail.com");
	version.UsesAddressLibrary();
	version.UsesUpdatedStructs();
	version.CompatibleVersions({ SKSE::RUNTIME_SSE_1_6_1170 });

	return version;
});