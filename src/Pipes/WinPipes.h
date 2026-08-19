#pragma once

#include <cstdint>
#include <string>

using i8  = int8_t;
using u8  = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;

namespace Pipes::WinPipes {
bool Start(u8 id, const std::string& path);

std::string Read(u8 id, bool isBlocking);
bool        Write(u8 id, const std::string& data);

bool Stop(u8 id);
} // namespace Pipes::WinPipes
