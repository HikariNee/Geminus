#pragma once
#include <source_location>
#include <string_view>

auto print_debug(const std::string_view, const std::source_location = std::source_location::current()) -> void;
