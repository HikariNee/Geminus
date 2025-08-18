#include "utilities.hpp"
#include <cstdint>
#include <format>
#include <iostream>

auto printDebug(const std::string_view str, const std::source_location location) -> void
{
  std::string file = location.file_name();
  std::string file_name = file.substr(file.find_last_of("/") + 1);
  std::uint32_t line_number = location.line();

  std::cout << std::vformat("[{}:{}] {}.\n", std::make_format_args(file_name, line_number, str));
}
