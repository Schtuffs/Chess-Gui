#pragma once

#include <string>

namespace Pipes::WinPipes {
bool Start(const std::string& path);

std::string Read();
bool        Write(const std::string& data);

bool Stop();
} // namespace Pipes::WinPipes
