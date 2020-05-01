#include "GMLParser.h"
const std::string& gml::ParsingTools::trim(std::string& tag_text, char trimchar)
{
	auto it = std::remove(tag_text.begin(), tag_text.end(), trimchar);
	tag_text.erase(it, tag_text.end());
	return tag_text;
}
std::vector<std::string> gml::ParsingTools::splitIntoTokens(const std::string& tag_text, const char seperator)
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
std::optional<std::pair<std::string, std::string>> gml::ParsingTools::splitIntoToken(const std::string& text, const char seperator, const std::vector<char>forbidden_list)
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
bool gml::ParsingTools::isClosed(const std::string& s1, const std::string& s2, char closing_character, char seperation_character)
{


	if (*s2.begin() != closing_character) {
		return false;
	}
	auto split_tag_value = splitIntoToken(s1, seperation_character, {});
	return std::equal(split_tag_value->first.begin(), split_tag_value->first.end(), s2.begin() + 1, s2.end());
}

bool gml::ParsingTools::isClosed(std::string& str, char closing_character)
{


	return str[0] == closing_character;
}



std::pair<gml::RawRecord, bool> gml::ParsingTools::fetchRawRecord(std::string::const_iterator stream_pos, const std::string::const_iterator end_pos, const gml::GDML_SYMBOL_PROFILE& syntax_profile) {

	RawRecord gml_record;

	// Gets the OpenBrace and CloseBrace position from the token_stream
	auto getBraceOpenandClosePos = [&syntax_profile](std::string::const_iterator stream_start, std::string::const_iterator stream_end) {


		auto open_brace_pos = std::find(stream_start, stream_end, syntax_profile.getOpenTag());
		if (!(open_brace_pos == stream_end)) {
			auto close_brace_pos = std::find(open_brace_pos, stream_end, syntax_profile.getCloseTag());
			if (!(close_brace_pos == stream_end)) {
				return std::make_pair(open_brace_pos, close_brace_pos);
			}
		}

		return std::make_pair(stream_end, stream_end);


	};

	auto OTAG_brace_positions = getBraceOpenandClosePos(stream_pos, end_pos);

	if (!(OTAG_brace_positions.first == end_pos || OTAG_brace_positions.second == end_pos)) {


		// All the tokens inside [] are obtained
		gml_record.open_tag_token_stream = std::string(OTAG_brace_positions.first + 1, OTAG_brace_positions.second);
		trim(gml_record.open_tag_token_stream);

		//***********************************
		// Part for closing tag
		//***********************************

		//--------------------------------------------------------------------------------------------------------------------------
		auto CTAG_brace_positions = getBraceOpenandClosePos(OTAG_brace_positions.second + 1, end_pos);

		if (!(CTAG_brace_positions.first == end_pos || CTAG_brace_positions.second == end_pos)) {

			// All the tokens inside [/] are obtained
			gml_record.close_tag_token_stream = std::string(CTAG_brace_positions.first + 1, CTAG_brace_positions.second);
			trim(gml_record.close_tag_token_stream);


			if (gml::ParsingTools::isClosed(
				gml_record.close_tag_token_stream,
				syntax_profile.getClosingCharacter()
			) ||
				gml::ParsingTools::isClosed(
					gml_record.open_tag_token_stream, gml_record.close_tag_token_stream,
					syntax_profile.getClosingCharacter(),
					syntax_profile.getTagValueSeperator()
				)
				) {

				gml_record.inner_data = std::string(OTAG_brace_positions.second + 1, CTAG_brace_positions.first);
				gml_record.record_end_position = CTAG_brace_positions.second;
				return { gml_record,true };
			}

		}


	}

	return { RawRecord(),false };

}

std::optional<std::pair<std::string, gml::TagValue_T>> gml::ParsingTools::processSplitToken(const std::string& text, const GDML_SYMBOL_PROFILE& syntax_profile)
{
	auto tag_value_pair = splitIntoToken(text, syntax_profile.getTagValueSeperator(), { syntax_profile.getAttributeListOpen(),syntax_profile.getAttributeListClose() });
	if (tag_value_pair.has_value())
	{
		if ((!tag_value_pair->second.empty()) && *tag_value_pair->second.begin() == syntax_profile.getAttributeListOpen()) {

			auto attribute_list_close_pos = std::find(tag_value_pair->second.begin(), tag_value_pair->second.end(), syntax_profile.getAttributeListClose());

			//Covers the case if along with attribute list some other text was also specified like [print:{color:blue} This is not allowed] Hello [/]
			if (!(attribute_list_close_pos == tag_value_pair->second.end() || attribute_list_close_pos != tag_value_pair->second.end() - 1)) {
				std::string parse_string(tag_value_pair->second.begin() + 1, attribute_list_close_pos);
				auto attributelist = splitIntoTokens(parse_string, syntax_profile.getAttributeSeperator());
				std::map<std::string, std::string> my_map;
				for (auto& tag : attributelist) {

					auto x = splitIntoToken(tag, syntax_profile.getTagValueSeperator(), {});
					my_map[x->first] = x->second;


				}

				return std::make_pair(tag_value_pair->first, std::move(my_map));

			}
			return std::nullopt;


		}
		return std::make_pair(tag_value_pair->first, tag_value_pair->second);

	}
	
	return std::nullopt;

}
