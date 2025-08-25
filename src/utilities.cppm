export module Utils;
import std;

export auto print_debug(const std::string_view str, const std::source_location location = std::source_location::current()) -> void
{
  std::string file = location.file_name();
  std::string file_name = file.substr(file.find_last_of("/") + 1);
  std::uint32_t line_number = location.line();

  std::cout << std::format("[{}:{}] {}.\n", file_name, line_number, str);
}
