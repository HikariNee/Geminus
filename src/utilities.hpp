#pragma once
#include <source_location>
#include <string_view>

auto printDebug(const std::string_view, const std::source_location = std::source_location::current()) -> void;
