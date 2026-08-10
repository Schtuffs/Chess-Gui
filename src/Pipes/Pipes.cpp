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

static bool s_engineRunning = false;

namespace Pipes {

bool Start(const std::string& path)
{
    if (s_engineRunning) {
        Pipes::Stop();
    }

    bool success = Platform::Start(path);

    if (!success) {
        Pipes::Stop();
    }
    s_engineRunning = success;
    return success;
}

std::string Read()
{
    if (!s_engineRunning) {
        return "";
    }

    return Platform::Read();
}

bool Write(const std::string& data)
{
    if (!s_engineRunning) {
        return false;
    }

    return Platform::Write(data);
}

bool Stop()
{
    if (!s_engineRunning) {
        return true;
    }

    // Generic quit message
    Pipes::Write("quit");
    s_engineRunning = false;

    return Platform::Stop();
}

} // namespace Pipes
