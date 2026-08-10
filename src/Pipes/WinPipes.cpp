#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include "Windows.h"

#include <print>

HANDLE              s_engineStdin   = nullptr;
HANDLE              s_engineStdout  = nullptr;
HANDLE              s_engineProcess = nullptr;
HANDLE              s_engineThread  = nullptr;
PROCESS_INFORMATION pi;

namespace Pipes::WinPipes {

// ----- START -----

static std::wstring StrToWStr(const std::string& str)
{
    if (str.empty()) {
        return std::wstring();
    }

    std::wstring buffer;
    buffer.resize(str.length() + 1);

#if defined(_MSC_VER)
    size_t charsConverted = 0;
    mbstowcs_s(&charsConverted, buffer.data(), buffer.size(), str.c_str(), _TRUNCATE);
#else
    mbstowcs(buffer.data(), str.c_str(), str.length());
#endif

    return buffer;
}

static bool HandleClose(HANDLE handle)
{
    if (!handle) {
        bool ret = CloseHandle(handle);
        handle   = nullptr;
        return ret;
    }

    return true;
}

bool Start(const std::string& path)
{
    // Prepare pipes
    HANDLE tempStdin  = nullptr;
    HANDLE tempStdout = nullptr;

    SECURITY_ATTRIBUTES sAttrs{};
    sAttrs.nLength              = sizeof(sAttrs);
    sAttrs.lpSecurityDescriptor = nullptr;
    sAttrs.bInheritHandle       = TRUE;

    // file -> engine stdin
    if (!CreatePipe(&tempStdin, &s_engineStdin, &sAttrs, 0)) {
        std::println(stderr, "ERROR: Pipes::Start: Failed to start engine stdin");
        return false;
    }
    SetHandleInformation(s_engineStdin, HANDLE_FLAG_INHERIT, 0);

    // engine stdout -> file
    if (!CreatePipe(&s_engineStdout, &tempStdout, &sAttrs, 0)) {
        std::println(stderr, "ERROR: Pipes::Start: Failed to start engine stdout");
        HandleClose(tempStdout);
        HandleClose(s_engineStdin);

        s_engineStdin = nullptr;
        return false;
    }
    SetHandleInformation(s_engineStdout, HANDLE_FLAG_INHERIT, 0);

    // Info for startup
    STARTUPINFOW info{};
    info.cb         = sizeof(info);
    info.dwFlags    = STARTF_USESTDHANDLES;
    info.hStdInput  = tempStdin;
    info.hStdOutput = tempStdout;
    info.hStdError  = tempStdout;

    PROCESS_INFORMATION processInfo{};

    // Convert to wide
    std::wstring engine = StrToWStr(path);
    BOOL success = CreateProcess(engine.data(), nullptr, nullptr, nullptr, TRUE, CREATE_NO_WINDOW,
                                 nullptr, nullptr, &info, &processInfo);

    HandleClose(tempStdin);
    HandleClose(tempStdout);

    if (!success) {
        std::println(stderr, "ERROR: Pipes::Start: Failed to start engine process");
        HandleClose(s_engineStdin);
        HandleClose(s_engineStdout);

        return false;
    }

    s_engineProcess = processInfo.hProcess;
    s_engineThread  = processInfo.hThread;
    pi              = processInfo;

    return true;
}

// ----- READ -----

std::string Read()
{
    DWORD available = 0;
    if (!PeekNamedPipe(s_engineStdout, nullptr, 0, nullptr, &available, nullptr) ||
        available == 0) {
        return "";
    }

    std::string buffer;
    buffer.resize(available + 1);
    DWORD read;
    if (!ReadFile(s_engineStdout, buffer.data(), available, &read, nullptr) || read == 0) {
        return "";
    }

    return std::string(buffer.data(), read);
}

// ----- WRITE -----

bool Write(const std::string& data)
{
    std::string message = data;
    if (message.empty() || message.back() != '\n') {
        message += '\n';
    }

    DWORD written = 0;
    BOOL success = WriteFile(s_engineStdin, message.c_str(), (DWORD)message.size(), &written, NULL);
    return (success && written == message.size());
}

// ----- STOP -----

bool Stop()
{
    DWORD result = WaitForSingleObject(s_engineProcess, 1000);
    if (result == WAIT_TIMEOUT) {
        TerminateProcess(s_engineProcess, 0);
    }

    bool closure = true;
    if (!HandleClose(s_engineProcess)) {
        closure = false;
    }
    if (!HandleClose(s_engineThread)) {
        closure = false;
    }
    if (!HandleClose(s_engineStdout)) {
        closure = false;
    }
    if (!HandleClose(s_engineStdin)) {
        closure = false;
    }
    return closure;
}

} // namespace Pipes::WinPipes

#endif
