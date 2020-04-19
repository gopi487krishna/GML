#include "pch.h"
#include "GDML.h"
#include "error_codes.h"
#include <algorithm>
//GDML Parser



int gml::GMLParser::exec(const std::string& str, TBE_Profile profile)
{

	auto strbeg = str.begin();
	auto strend = str.end();

	while (strbeg != strend)
	{

		

		if (parser_schedule.isScheduled() && parser_schedule.isValid()) {

			current_record = parser_schedule.getRecordCollection()[parser_schedule.getLocation()];
			parser_schedule.setLocation(parser_schedule.getLocation() + 1);

		}
		else {
			// If not scheduled then no effect
			parser_schedule.stop_scheduling();
			auto gml_record = ParsingTools::fetchRawRecord(strbeg, strend, syntax_profile);
			// If the next record could be fetched/parsed from the input stream
			if (gml_record.second == true) {
				auto tags = gml::ParsingTools::splitIntoTokens(gml_record.first.open_tag_token_stream, syntax_profile.getTagSeperator());
				if (tags.empty())
				{
					return NO_TAGS_FOUND;
				}
				// Contains tags in the form of tag_value_pairs
				Tag_Collection  processed_tags;

				for (auto& tag : tags) {

					if (auto tag_value_pair = gml::ParsingTools::processSplitToken(tag, syntax_profile); tag_value_pair.has_value()) {
						processed_tags.push_back(*tag_value_pair);
					}
					else {
						return NO_TAGS_FOUND;
					}
				}

				// Cached

				// This will change in future
				current_record.record_label = processed_tags[0].first;
				current_record.tags = processed_tags;
				current_record.inner_data = gml_record.first.inner_data;
				parser_schedule.getRecordCollection().push_back(current_record);



				strbeg = gml_record.first.record_end_position + 1;
			}
			else {
				return TAG_NOT_CLOSED;
			}
		}

		// Common dispatching for all 
		for (auto& tag_value_pair : current_record.tags) {
			profile.exec_func(this, tag_value_pair.first, tag_value_pair.second, current_record.inner_data);
		}
	}
}



const gml::TBE_function gml::TBE_Profile::operator[](const std::string& tag_name)
{
	try
	{
		TBE_function val = items.at(tag_name);
		return val;
	}
	catch (...)
	{
		return nullptr;
	}


}

//TBE_Profile
bool gml::TBE_Profile::registerforToken(const std::string& token_str, TBE_function tbe_func)
{
	auto key_loc = items.find(token_str);
	if (key_loc != items.end())
	{
		return false;
	}
	else
	{
		items[token_str] = tbe_func;
		return true;
	}
}
bool gml::TBE_Profile::removeToken(const std::string& token_str)
{
	return items.erase(token_str);
}
gml::TBE_function gml::TBE_Profile::getTBE_func(const std::string& token_str)
{
	TBE_function func;
	try
	{
		func = items.at(token_str);

	}
	catch (...)
	{
		func = nullptr;
	}
	return func;

}
bool gml::TBE_Profile::exec_func(GMLParser* parser, std::string& tag, gml::TagValue_T& value, std::string& data)
{
	// If there is some token_filter then execute it
	if (is_filter_present())
	{
		// Check if should redispatch it to their respective functions
		auto should_redispatch = tbe_filter(parser, tag, value, data);
		if (!should_redispatch) {
			return true;
		}
	}
	// Get the tag function,
	auto func = getTBE_func(tag);
	if (func != nullptr)
	{
		func(parser, tag, value, data);
		return true;
	}
	return false;


}

void gml::GMLTokenCard::TokenFunction::with(TBE_function functionfortoken)
{
	m_functionfortoken = functionfortoken;
	m_tokenprofile_id->registerforToken(m_token_name, m_functionfortoken);

}

gml::GMLTokenCard::TokenFunction gml::GMLTokenCard::tie(const std::string& token_name)
{
	TokenFunction func;
	func.setTokenName(token_name);
	func.setTbeProfileAddress(&m_tokenprofile);
	return func;

}

void gml::GMLTokenCard::detachFunctionFrom(std::string& token_name)
{
	m_tokenprofile.removeToken(token_name);
}

auto gml::GMLTokenCard::getFunctionFrom(const std::string& token_name)
{

	return m_tokenprofile[token_name];
}

bool gml::GMLTokenCard::setFilter(TBE_function filterfunction)
{
	if (!filter_installed)
	{
		m_tokenprofile.installFilter(filterfunction);
		return true;
	}
	else
	{
		return false;
	}

}

void gml::GMLTokenCard::removeFilter()
{
	m_tokenprofile.removeFilter();
}

gml::GMLTokenCard::operator gml::TBE_Profile()
{
	return m_tokenprofile;
}
