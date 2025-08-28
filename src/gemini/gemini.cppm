module;

export module Gemini;
import std;
import TLS;
import GeminiErr;

export class GeminiSession;
export using enum GeminiErrors;

template<Store T>
auto validateHeader(const TLS::Session<T>& session) -> std::expected<void, GeminiErrors>
{

}
