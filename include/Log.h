#pragma once
#include <PCH.h>

namespace Log
{
    void Console(const char* fmt, ...);
    void Notify(const char* fmt, ...);
    void MessageBox(const char* fmt, ...);
}
