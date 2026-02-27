#include <Log.h>

constexpr auto REL_DebugNotification = RELOCATION_ID(52050, 52933);

static void DebugNotification(const char* a_notification, const char* a_soundToPlay = 0, bool a_cancelIfAlreadyQueued = true)
{
	using func_t = decltype(&DebugNotification);
	REL::Relocation<func_t> func{ REL_DebugNotification };
	return func(a_notification, a_soundToPlay, a_cancelIfAlreadyQueued);
}

void Log::Console(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	RE::ConsoleLog::GetSingleton()->VPrint(fmt, args);
	va_end(args);
}

void Log::Notify(const char* fmt, ...)
{
	char buffer[256];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	DebugNotification(buffer);
}

void Log::MessageBox(const char* fmt, ...)
{
	char buffer[256];

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, sizeof(buffer), fmt, args);
	va_end(args);

	RE::DebugMessageBox(buffer);
}