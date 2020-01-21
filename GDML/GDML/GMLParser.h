#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <optional>


namespace gml {

	/*Parsing Api: Parsing API is used for ...*/

	class ParsingApi {

	public:

		static const std::string& trim(std::string& tag_text, char trimchar = ' ');
		static std::vector<std::string> splitIntoTokens(const std::string& tag_text, const char seperator = ' ');
		static std::optional<std::pair<std::string, std::string>>splitIntoToken(const std::string& text, const char seperator, std::vector<char> forbidden_list);
		static bool isClosed(const std::string& s1, const std::string& s2, char closing_character, char seperation_character);
		static bool isClosed(std::string& str, char closing_character);

	};

}