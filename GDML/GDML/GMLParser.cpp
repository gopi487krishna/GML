
#include "pch.h"
#include "GMLParser.h"

const std::string& gml::ParsingApi::trim(std::string& tag_text, char trimchar)
{
	auto it = std::remove(tag_text.begin(), tag_text.end(), trimchar);
	tag_text.erase(it, tag_text.end());
	return tag_text;
}
std::vector<std::string> gml::ParsingApi::splitIntoTokens(const std::string& tag_text, const char seperator)
{

	std::vector<std::string> tag_collection;
	std::string tag;
	if (tag_text.empty())
	{
		return tag_collection;

	}
	for (auto character : tag_text)
	{

		if (character == seperator)
		{
			tag_collection.push_back(tag); tag.clear(); continue;
		}
		tag += character;

	}
	if (!tag.empty())
	{
		tag_collection.push_back(tag);
	}
	return tag_collection;
}
std::optional<std::pair<std::string, std::string>> gml::ParsingApi::splitIntoToken(const std::string& text, const char seperator, const std::vector<char>forbidden_list)
{

	auto textbeg = text.begin();
	auto textend = text.end();

	//default position
	auto seperator_pos = textend;


	for (auto iterator = textbeg; iterator != textend; iterator++)
	{

		if (!isalpha(*iterator))
		{
			if (*iterator == seperator && seperator_pos == textend)
			{
				seperator_pos = iterator;

			}
			// For any other kind of character this function should not work

			else if ((std::find(forbidden_list.begin(), forbidden_list.end(), *iterator) != forbidden_list.end()) && seperator_pos == textend)
			{
				return std::nullopt;
			}



		}
	}


	if (seperator_pos == textend)
	{
		return std::nullopt;
	}
	else if (seperator_pos == textbeg)
	{
		return std::nullopt;
	}
	else if (seperator_pos == textend - 1)
	{

		return std::make_pair(std::string(textbeg, textend - 1), std::string());
	}
	else
	{
		return std::make_pair(std::string(textbeg, seperator_pos), std::string(seperator_pos + 1, textend));
	}

}
bool gml::ParsingApi::isClosed(const std::string& s1, const std::string& s2,char closing_character, char seperation_character)
{


	if (*s2.begin() != closing_character) {
		return false;
	}
	auto split_tag_value = splitIntoToken(s1, seperation_character, {});
	return std::equal(split_tag_value->first.begin(), split_tag_value->first.end(), s2.begin() + 1, s2.end());
}

bool gml::ParsingApi::isClosed(std::string& str, char closing_character)
{

	
	return str[0] == closing_character;
}
