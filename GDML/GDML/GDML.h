#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <functional>
#include <variant>
#include <optional>


// Forward Declaration
namespace gml {

	class GDMLParser;
	using TBE_function = std::function<bool(GDMLParser * parser, std::string, std::variant<std::string,std::map<std::string,std::string>>, std::string)>;




	/**
	 * @brief	Defines a small parsing API for token parsing
	 * @details	This class contains a small set of static functions that assist the main interpreter for parsing and validating GML code
	 * @author	Gopi Krishna Menon
	 * @note	This class is not intended to be used directly by the Users of GML. Although the functions in the API do not
				have any dependency with the main Interpreter class, I advise users not to use them for any other purpose as 
				the details may change with time
	*/
	class ParsingApi {

	public:

		/// @brief	 Trims the given piece of text from the trimcharacter
		/// @details Removes all the occurences of trimcharacter from the given string
		/// @param	 tag_text Text that needs to be trimmed
		/// @param	 trimchar Unwanted character that needs to be removed from <strong> tag_text </strong>
		/// @return  const String&
		static const std::string& trim(std::string& tag_text, char trimchar = ' ');
	
		
		/// @brief		Splits a token stream into a collection of Tags
		/// @details	Takes a token stream and stores the tokens into a vector by spltting them on the basis of seperator
		/// @param		tag_text  Token Stream
		/// @param		seperator ASCII character used to seeprate the tokens
		/// @return     vector<string>
		static std::vector<std::string> splitIntoTokens(const std::string& tag_text, const char seperator = ' ');

		/// @brief		 Splits a given token into name:value pairs
		/// @details	Splits a given token into <strong>name:value</strong> pairs based on the seperator
		/// @param		text			 text that needs to be split into name value pair
		/// @param		seperator		 character used for seperating the text into name:value pairs
		/// @param		forbiddden_list  A collection of characters that must not appear before the seperator
		/// @return     optional< pair<string,string> >
		/// @note		As you can see this function is tightly integrated to Interpreter's context ( not physically but conceptually ) The forbidden list is used to 
		///				make sure that certain blacklisted characters/symbols do not appear before seperator ( so that the interpreter does not parse the code incorrectly )

		static std::optional<std::pair<std::string, std::string>>splitIntoToken(const std::string& text, const char seperator, std::vector<char> forbidden_list);
		
		/// @brief		Checks whether the GML record was closed
		/// @details	Checks whether the GML record was closed. But this function instead of checking only for closing character
		///				also looks out for whether the record was closed by the first tag of the record.
		/// @note		This function is present in order to support a feature of GML that allows closing the record using the closing tag followed by the first tag.
		/// @param		s1  String containing first tag with its value
		/// @param		s2  String to be checked whether record is closed properly or not
		/// @param		closing_character  Symbol that indicates the closing tag
		/// @param		seperation_character  Used for seperating s1 into tag_value pair
		/// @return     bool

		static bool isClosed(const std::string& s1, const std::string& s2, char closing_character, char seperation_character);
		
		
		/// @brief		Checks whether the GML record was closed
		/// @details	Checks whether the GML record was closed using the closing_character
		/// @param		str  String to be checked whether record is closed properly or not
		/// @param		closing_character  Symbol that indicates the closing tag
		/// @return     bool
		static bool isClosed(std::string& str, char closing_character);

	};


	/**
	 * @brief Defines all the set of Symbols that control the Program Form and Style of GML code
	 * @author Gopi Krishna Menon
	 * @details
	 * This class contains a set of methods for manipulating the default Program Form and Style of <strong>GML<strong> code.
	 * The object of this code with new set of symbols should be passed as an argument to the Parser. The parser parses the code
	 * stricly on the symbols defined.
	 * @note The symbols/characters accepted should be purely ASCII in nature.
	 * @warning Currently the parser does not warn if the symbols are same for different contexts. This may result in faliure to parse the GML code
	 * or may parse it	incorrectly
	*/
	class GDML_SYMBOL_PROFILE
	{
		char tag_seperator{ ';' };
		char tag_value_seperator{ ':' };
		char tag_open{ '[' };
		char tag_close{ ']' };
		char closing_character{ '/' };
		char attribute_open{ '{' };
		char attribute_seperator{ ',' };
		char attribute_close{ '}' };


	public:
		GDML_SYMBOL_PROFILE();

		/// @brief Construct Syntax Card object with the following parameters.
		/// @param _tag_seperator			Symbol for seperating tags
		/// @param _tag_value_seperator	    Symbol for seperating tags into  tag:value pair
		/// @param _tag_open				Symbol for starting tag list
		/// @param _tag_close			    Symbol for closing tags list
		/// @param _closing_character		Symbol for delimiting the scope of  tag list
		/// @param _attribute_open			Symbol for starting an attribute list of a tag
		/// @param _attribute_seperator		Symbol for seperating attributes in an attribute list
		/// @param _attribute_close			Symbol for ending an attribute list of a tag
		GDML_SYMBOL_PROFILE(
			const  char _tag_seperator,
			const  char _tag_value_seperator,
			const  char _tag_open,
			const  char _tag_close,
			const  char _closing_character,
			const  char _attribute_open,
			const  char _attribute_seperator,
			const  char _attribute_close

		);


		/// @brief  Sets the closing character that delimits the scope of a tag list
		/// @param _closing_character ASCII character for closing_symbol
		void setClosingCharacter(char _closing_character);

		/// @brief  Gets the closing character that delimits the scope of a tag list
		/// @return char
		char getClosingCharacter();

		/// @brief  Sets the attribute seperator for seperating attributes in an attribute list
		/// @param _attribute_seperator ASCII character for attribute_seperator symbol
		void setAttributeSeperator(char _attribute_seperator);


		/// @brief Gets the attribute_seperator for seperating attributes in an attribute list
		/// @return char
		char getAttributeSeperator();

		/// @brief Sets the open_attribute symbol for starting an attribute list for a tag
		/// @param open_attribute ASCII character for open_attribute symbol
		void setAttributelistOpen(char open_attribute);

		/// @brief Get the open_attribute symbol that starts an attribute list for a tag
		/// @return char
		char getAttributeListOpen();

		/// @brief Sets the close_attribute symbol that closes an attribute list for a tag
		/// @param close_attribute ASCII character for close_attribute symbol
		void setAttributeListClose(char close_attribute);

		/// @brief Gets the close_attribute symbol that closes an attribute list for a tag
		/// @return char
		char getAttributeListClose();

		/// @brief  Sets the tag_seperator that seperates different tags from each other in a record
		/// @param _tag_seperator ASCII character for seperating tags
		void setTagSeperator(char _tag_seperator);

		/// @brief  Gets the tag_seperator symbol that seperates different tags from each other in a record
		/// @return char
		char getTagSeperator();

		/// @brief  Sets the tag_value seperator symbol for splitting a tag into tag_value pairs 
		/// @param  _tag_value_seperator ASCII character for splitting into tag_value pairs
		void setTagValueSeperator(char _tag_value_seperator);

		/// @brief  Gets the tag_value seperator symbol that splits a tag into tag_value pair
		/// @return char
		char getTagValueSeperator();

		/// @brief  Sets the open_tag symbol that starts the tag list
		/// @param  opentag_character ASCII character for starting a tag list
		void setOpenTag(char opentag_character);

		/// @brief	Gets the open_tag symbol used to start the tag list 
		/// @return char
		char getOpenTag();

		/// @brief  Sets the closetag symbol used to end the tag list
		/// @param	closetag_character ASCII character for ending a tag list
		void setCloseTag(char closetag_character);

		/// @brief  Gets the closetag symbol used to end the tag list
		/// @return char
		char getCloseTag();


	};





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

	class GDMLParser
	{

		GDML_SYMBOL_PROFILE syntax_profile;
	public:
		GDMLParser() {}
		GDMLParser(GDML_SYMBOL_PROFILE _syntax_profile) :syntax_profile(_syntax_profile) {}
		void setSyntaxProfile(GDML_SYMBOL_PROFILE _syntax_profile) { syntax_profile = _syntax_profile; }

		int exec(const std::string& str, TBE_Profile profile);

	private:

		std::optional<std::pair<std::string, std::variant<std::string, std::map<std::string, std::string>>>> processSplitToken(const std::string& text);
		//*********************************************************
	};

}