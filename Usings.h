#pragma once

#include <cstdint>
#include <vector>

using Price = std::int32_t;      // signed: could go negative in edge cases
using Quantity = std::uint32_t;  // only positive quantities allowed
using OrderId = std::uint64_t;

struct LevelInfo
{
    Price price_;
    Quantity quantity_;
};

using LevelInfos = std::vector<LevelInfo>;
