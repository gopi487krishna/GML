#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <functional>
#include <map>
#include <optional>
#include <variant>
// Forward Declaration
namespace gml {

	class GDMLParser;
	using TBE_function = std::function<bool(GDMLParser * parser, std::string, std::variant<std::string,std::map<std::string,std::string>>, std::string)>;

	class TBE_Profile
	{

		//*********************************************************
		std::map<std::string, TBE_function> items;
		TBE_function tbe_filter = nullptr;
		//*********************************************************


	public:

		//*********************************************************
		void installFilter(TBE_function tbe_func) { tbe_filter = tbe_func; }
		void removeFilter() { tbe_filter = nullptr; }
		bool is_filter_present() { return tbe_filter != nullptr; }
		//*********************************************************

		const TBE_function operator[](std::string index);
		bool registerforToken(std::string token_str, TBE_function tbe_func);
		bool removeToken(std::string token_str);

		TBE_function getTBE_func(std::string token_str);
		bool exec_func(GDMLParser* parser, std::string& tag, std::variant<std::string,std::map<std::string,std::string>>& value, std::string& data);
		//*********************************************************
	};

	class GMLTokenCard
	{
		TBE_Profile m_tokenprofile;
		bool filter_installed{ false };
		struct TokenFunction
		{
			TBE_Profile* m_tokenprofile_id;
			std::string m_token_name;
			TBE_function m_functionfortoken;

		public:
			void setTokenName(const std::string& token_name) { m_token_name = token_name; }
			void setTbeProfileAddress(TBE_Profile* tbe_profile) { m_tokenprofile_id = tbe_profile; }
			void with(TBE_function functionfortoken);
		};

	public:
		operator TBE_Profile();
		TokenFunction tie(const std::string& token_name);
		void detachFunctionFrom(std::string& token_name);
		auto getFunctionFrom(const std::string& token_name);
		bool setFilter(TBE_function filterfunction);
		void removeFilter();
	};

	class GDML_SYMBOL_PROFILE
	{
		char tag_seperator{ ';' };
		char tag_value_seperator{ ':' };
		char tag_open{ '[' };
		char tag_close{ ']' };

	public:
		GDML_SYMBOL_PROFILE() {}
		GDML_SYMBOL_PROFILE(const  char _tag_seperator, const  char _tag_value_seperator, const  char _tag_open, const  char _tag_close)
			:tag_seperator(_tag_seperator), tag_value_seperator(_tag_value_seperator), tag_open(_tag_open), tag_close(_tag_close) {}
		//----------------------------------------
		void setTagSeperator(char _tag_seperator) { tag_seperator = _tag_seperator; }
		char getTagSeperator() { return tag_seperator; }
		//----------------------------------------
		void setTagValueSeperator(char _tag_value_seperator) { tag_value_seperator = _tag_value_seperator; }
		char getTagValueSeperator() { return tag_value_seperator; }
		//----------------------------------------
		void setOpenTag(char opentag_character) { tag_open = opentag_character; }
		char getOpenTag() { return tag_open; }
		//----------------------------------------
		void setCloseTag(char closetag_character) { tag_close = closetag_character; }
		char getCloseTag() { return tag_close; }
		//----------------------------------------

	};

	class GDMLParser
	{

		GDML_SYMBOL_PROFILE syntax_profile;
	public:
		GDMLParser() {}
		GDMLParser(GDML_SYMBOL_PROFILE _syntax_profile) :syntax_profile(_syntax_profile) {}
		void setSyntaxProfile(GDML_SYMBOL_PROFILE _syntax_profile) { syntax_profile = _syntax_profile; }

		int exec(const std::string& str, TBE_Profile profile);

	private:

		bool isClosed(const std::string& s1, const std::string& s2);
		bool isClosed(std::string& str)
		{
			return str == "/";
		}
		const std::string& trim(std::string& tag_text, char trimchar = ' ');
		std::vector<std::string> splitIntoTokens(const std::string& tag_text, const char seperator = ' ');
		std::optional<std::pair<std::string, std::string>>splitIntoToken(const std::string& text, const char seperator = ' ');
		std::optional<std::pair<std::string, std::variant<std::string, std::map<std::string, std::string>>>> processSplitToken(const std::string& text, const char seperator);
		//*********************************************************
	};

}