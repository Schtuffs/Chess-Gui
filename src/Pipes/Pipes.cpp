#include "Pipes/Pipes.h"

#include <vector>

// clang-format off
#ifdef _WIN32
    #include "Pipes/WinPipes.h"
    namespace Platform = Pipes::WinPipes;
#else
#error INVALID PLATFORM
#endif
// clang-format on

#include <print>

static u8   s_newId              = 0;
static bool s_engineRunning[255] = {};

namespace Pipes {

u8 Start(const std::string& path)
{
    u8   id      = ++s_newId;
    bool success = Platform::Start(id, path);

    if (success) {
        s_engineRunning[id] = true;
    } else {
        Pipes::Stop(id);
    }

    return id;
}

bool IsValid(u8 id) { return id; }

std::string Read(u8 id, bool isBlocking)
{
    if (!s_engineRunning[id]) {
        return "";
    }

    return Platform::Read(id, isBlocking);
}

bool Write(u8 id, const std::string& data)
{
    if (!s_engineRunning[id]) {
        return false;
    }

    return Platform::Write(id, data);
}

bool Stop(u8 id)
{
    if (!s_engineRunning[id]) {
        return true;
    }

    // Generic quit message
    Pipes::Write(id, "quit");
    s_engineRunning[id] = false;

    return Platform::Stop(id);
}

void StopAll()
{
    for (u8 i = 0; i < 255; i++) {
        Pipes::Stop(i);
    }
}

} // namespace Pipes
