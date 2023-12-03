#pragma once

#include <cstdint>
#include <cstddef>

enum class MemoryModel : int8_t { RLX, REL, ACQ, REL_ACQ, SEQ_CST };
enum class BinaryOperator : int8_t { PLUS, MINUS, MULTIPLY, DIVIDE };

using Word = int32_t;
using Register = int8_t;
constexpr size_t kRegistersCount = 16;