#include "pch.h"
#include "GDML.h"
#include "error_codes.h"

//GDML Parser

std::optional<std::pair<std::string,gml::TagValue_T>> gml::GDMLParser::processSplitToken(const std::string& text)
{
	auto tag_value_pair = gml::ParsingTools::splitIntoToken(text, syntax_profile.getTagValueSeperator(), {syntax_profile.getAttributeListOpen(),syntax_profile.getAttributeListClose()});
	if (!tag_value_pair.has_value())
	{
		return std::nullopt;
	}

	if (*tag_value_pair->second.begin() == syntax_profile.getAttributeListOpen())	
	{

			auto attribute_list_close_pos = std::find(tag_value_pair->second.begin(), tag_value_pair->second.end(),syntax_profile.getAttributeListClose());
			
			//Covers the case if along with attribute list some other text was also specified like [print:{color:blue} This is not allowed] Hello [/]
			if (attribute_list_close_pos == tag_value_pair->second.end() || attribute_list_close_pos!= tag_value_pair->second.end()-1)
			{
				return std::nullopt;

			}
			
			std::string parse_string(tag_value_pair->second.begin()+1, attribute_list_close_pos);
			auto attributelist = gml::ParsingTools::splitIntoTokens(parse_string, syntax_profile.getAttributeSeperator());
			std::map<std::string, std::string> my_map;
			for (auto& tag : attributelist){
					
				auto x = gml::ParsingTools::splitIntoToken(tag,syntax_profile.getTagValueSeperator(),{});
				my_map[x->first] = x->second;
					

			}

			return std::make_pair(tag_value_pair->first, std::move(my_map));		
	}
	else{
			return std::make_pair(tag_value_pair->first, tag_value_pair->second);
		}
}




int gml::GDMLParser::exec(const std::string& str,TBE_Profile profile)
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


			//This loop basically recurses through all the tags that enclose the inner text
			for (auto& tag : tags)
			{
				auto tag_value_pair = processSplitToken(tag);

				if (!tag_value_pair)
				{

					return NO_TAGS_FOUND;
				}

				// TODO : Find a use for how to utilize state
				profile.exec_func(this, tag_value_pair->first, tag_value_pair->second, gml_record.first.inner_data);


			}

			strbeg = gml_record.first.record_end_position + 1;




		}
		else {
			return TAG_NOT_CLOSED;
		}

		


	}

}



const gml::TBE_function gml::TBE_Profile::operator[](std::string tag_name)
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
bool gml::TBE_Profile::registerforToken(std::string token_str, TBE_function tbe_func)
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
bool gml::TBE_Profile::removeToken(std::string token_str)
{
	return items.erase(token_str);
}
gml::TBE_function gml::TBE_Profile::getTBE_func(std::string token_str)
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
bool gml::TBE_Profile::exec_func(GDMLParser* parser, std::string& tag, gml::TagValue_T&value, std::string& data)
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
