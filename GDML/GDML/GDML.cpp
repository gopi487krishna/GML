#include "pch.h"
#include "GDML.h"
#include "error_codes.h"
#include <algorithm>
//GDML Parser



int gml::GMLParser::exec(const std::string& str,TBE_Profile profile)
{

	auto strbeg = str.begin();
	auto strend = str.end();

	while (strbeg != strend)
	{
	
		
		
		auto gml_record = ParsingTools::fetchRawRecord(strbeg, strend, syntax_profile);

		
		// If the next record could be fetched/parsed from the input stream
		if (gml_record.second == true)
		{


			auto tags = gml::ParsingTools::splitIntoTokens(gml_record.first.open_tag_token_stream, syntax_profile.getTagSeperator());
			
			if (tags.empty())
			{
				return NO_TAGS_FOUND;
			}



			
			/*std::vector<std::optional<std::pair<std::string, TagValue_T>>> processed_tags;
			processed_tags.reserve(tags.size());*/

		
			// Convert tags into tag_value pairs and store them.
			//std::transform(tags.begin(), tags.end(), processed_tags.begin(), [&]( std::string tag) { return std::move(gml::ParsingTools::processSplitToken(tag,syntax_profile)); });

			/*for (auto& tag : tags) {
			
			
				processed_tags.push_back(gml::ParsingTools::processSplitToken(tag, syntax_profile));
			
			
			}*/


			// Contains tags in the form of tag_value_pairs
			std::vector<std::optional<std::pair<std::string,TagValue_T>>>  processed_tags;

			for (auto& tag : tags) {
			
				if (auto tag_value_pair = gml::ParsingTools::processSplitToken(tag, syntax_profile); tag_value_pair.has_value()) {
					processed_tags.push_back(tag_value_pair);				
				}
				else {
				
					return NO_TAGS_FOUND;
				}
			}
			
			
			for (auto tag_value_pair : processed_tags) {
				profile.exec_func(this, tag_value_pair->first, tag_value_pair->second, gml_record.first.inner_data);		
			}
			tag_collection.push_back(processed_tags);
			strbeg = gml_record.first.record_end_position + 1;

		}
		else {
			return TAG_NOT_CLOSED;
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
bool gml::TBE_Profile::exec_func(GMLParser* parser, std::string& tag, gml::TagValue_T&value, std::string& data)
{
	// If there is some token_filter then execute it
	if (is_filter_present())
	{
		// Check if should redispatch it to their respective functions
		auto should_redispatch = tbe_filter(parser, tag, value, data);
		if (!should_redispatch){
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
