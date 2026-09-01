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

namespace Pipes {
using ID = u8;

constexpr Pipes::ID ID_INVALID = 0;

ID   Start(const std::string& path);
bool IsValid(ID id);

std::string Read(ID id, bool isBlocking);
bool        Write(ID id, const std::string& data);

bool Stop(ID id);
void StopAll();
} // namespace Pipes
