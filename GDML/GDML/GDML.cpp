#include "pch.h"
#include "GDML.h"
#include "error_codes.h"

//GDML Parser

std::optional<std::pair<std::string,std::variant<std::string,std::map<std::string,std::string>>>> gml::GDMLParser::processSplitToken(const std::string& text)
{
	auto tag_value_pair = gml::ParsingApi::splitIntoToken(text, syntax_profile.getTagValueSeperator(), {syntax_profile.getAttributeListOpen(),syntax_profile.getAttributeListClose()});
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
			auto attributelist = gml::ParsingApi::splitIntoTokens(parse_string, syntax_profile.getAttributeSeperator());
			std::map<std::string, std::string> my_map;
			for (auto& tag : attributelist){
					
				auto x = gml::ParsingApi::splitIntoToken(tag,syntax_profile.getTagValueSeperator(),{});
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
		auto OTAG_openbrace = std::find(strbeg, strend, syntax_profile.getOpenTag());

		//Error/SpecialCase
		if (OTAG_openbrace == strend)
		{
			//Normal Text which means the entire program is made of just comments
			return NO_CODE_ONLY_COMMENT;
		}

		//Comment Text: strbeg------OTAG_openbrace

		auto OTAG_closebrace = std::find(OTAG_openbrace, strend, syntax_profile.getCloseTag());

		//Error/SpecialCase
		if (OTAG_closebrace == strend)
		{
		
			return BRACE_MISMATCH_ERROR;
		}

		//Here we obtain the tag information inside the [] brackets
		std::string OTAG_attribtext(OTAG_openbrace + 1, OTAG_closebrace);

		//--------------------------------------------------------------------------------------------------------------------------

		auto CTAG_openbrace = std::find(OTAG_closebrace + 1 , strend, syntax_profile.getOpenTag());



		//Error/SpecialCase
		if (CTAG_openbrace == strend)
		{
			return BRACE_MISMATCH_ERROR;
			
		} //Error/SpecialCase


		auto CTAG_closebrace = std::find(CTAG_openbrace, strend, syntax_profile.getCloseTag());

		//Error/SpecialCase
		if (CTAG_closebrace == strend)
		{
			return BRACE_MISMATCH_ERROR;
		}

		std::string CTAG_attribtext(CTAG_openbrace + 1, CTAG_closebrace);

		//***********************
		gml::ParsingApi::trim(OTAG_attribtext);
		gml::ParsingApi::trim(CTAG_attribtext);
		//***********************

		//Check whether the tags are properly closed and then dispatch them
		if (gml::ParsingApi::isClosed(CTAG_attribtext,syntax_profile.getClosingCharacter()) || gml::ParsingApi::isClosed(OTAG_attribtext, CTAG_attribtext,syntax_profile.getClosingCharacter(),syntax_profile.getTagValueSeperator()))
		{

			
			std::string inner_text(OTAG_closebrace + 1 , CTAG_openbrace );
			auto tags = gml::ParsingApi::splitIntoTokens(OTAG_attribtext, syntax_profile.getTagSeperator());
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

				auto state=profile.exec_func(this,tag_value_pair->first, tag_value_pair->second, inner_text);
				
				
			}
			

			strbeg = CTAG_closebrace + 1;
		}
		else
		{
			return TAG_NOT_CLOSED;
		}


	}

}



const gml::TBE_function gml::TBE_Profile::operator[](std::string index)
{
	try
	{
		TBE_function val = items.at(index);
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
bool gml::TBE_Profile::exec_func(GDMLParser* parser, std::string& tag, std::variant<std::string,std::map<std::string,std::string>>&value, std::string& data)
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
