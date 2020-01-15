#include "pch.h"
#include "GDML.h"
#include "error_codes.h"

//GDML Parser
const std::string& gml::GDMLParser::trim(std::string& tag_text, char trimchar)
{
	auto it = std::remove(tag_text.begin(), tag_text.end(), trimchar);
	tag_text.erase(it, tag_text.end());
	return tag_text;
}
std::vector<std::string> gml::GDMLParser::splitIntoTokens(const std::string& tag_text, const char seperator)
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
std::optional<std::pair<std::string, std::string>> gml::GDMLParser::splitIntoToken(const std::string& text, const char seperator)
{
	auto txtbeg = text.begin();
	auto txtend = text.end();
	auto seperator_pos = std::find(txtbeg, txtend, seperator);

	if (seperator_pos == txtend)
	{
		return std::nullopt;
	}
	else if (seperator_pos == txtbeg)
	{
		/*return std::make_pair(std::string(), std::string(txtbeg + 1, txtend));*/
		// Consider revisiting the case for above statement
		return std::nullopt;
	}
	else if (seperator_pos == txtend - 1)
	{
		return std::make_pair(std::string(txtbeg, txtend - 1), std::string());

	}
	else
	{
		return std::make_pair(std::string(txtbeg, seperator_pos), std::string(seperator_pos + 1, txtend));
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
		trim(OTAG_attribtext);
		trim(CTAG_attribtext);
		//***********************

		//Check whether the tags are properly closed and then dispatch them
		if (isClosed(CTAG_attribtext) || isClosed(OTAG_attribtext, CTAG_attribtext))
		{

			
			std::string inner_text(OTAG_closebrace + 1 , CTAG_openbrace );
			auto tags = splitIntoTokens(OTAG_attribtext, syntax_profile.getTagSeperator());
			if (tags.empty())
			{
				return NO_TAGS_FOUND;
			}

			
			//This loop basically recurses through all the tags that enclose the inner text
			for (auto& tag : tags)
			{
				auto tag_value_pair = splitIntoToken(tag, syntax_profile.getTagValueSeperator());
							
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
bool gml::GDMLParser::isClosed(const std::string& s1, const std::string& s2)
{


	if (*s2.begin() != '/'){
		return false;
	}
	auto split_tag_value = splitIntoToken(s1, syntax_profile.getTagValueSeperator());
	return std::equal(split_tag_value->first.begin(), split_tag_value->first.end(), s2.begin()+1, s2.end());
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
bool gml::TBE_Profile::exec_func(GDMLParser* parser, std::string tag, std::string& value, std::string& data)
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
	// Get the tag function
	auto func = getTBE_func(tag);
	func != nullptr ? (func(parser, tag, value, data), true) : false;	
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
