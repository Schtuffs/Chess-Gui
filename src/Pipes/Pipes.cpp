#include "Pipes/Pipes.h"

#include <atomic>
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

std::atomic<Pipes::ID> s_newId              = 0;
bool                   s_engineRunning[255] = {};

namespace Pipes {

Pipes::ID Start(const std::string& path)
{
    Pipes::ID id        = ++s_newId;
    s_engineRunning[id] = true;
    bool success        = Platform::Start(id, path);

    if (!success) {
        Pipes::Stop(id);
        --s_newId;
        id = Pipes::ID_INVALID;
    }

    return id;
}

bool IsValid(Pipes::ID id) { return id != Pipes::ID_INVALID; }

std::string Read(Pipes::ID id, bool isBlocking)
{
    if (!s_engineRunning[id]) {
        return "";
    }

    return Platform::Read(id, isBlocking);
}

bool Write(Pipes::ID id, const std::string& data)
{
    if (!s_engineRunning[id]) {
        return false;
    }

    return Platform::Write(id, data);
}

bool Stop(Pipes::ID id)
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
