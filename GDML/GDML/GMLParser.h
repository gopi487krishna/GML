#pragma once
#include "GDML.h"

namespace gml {


	struct RawRecord {

		std::string::const_iterator record_end_position;
		std::string open_tag_token_stream = "";
		std::string close_tag_token_stream = "";
		std::string inner_data;

	};

	/**
		 * @brief	Defines a small parsing API for token parsing
		 * @details	This class contains a small set of static functions that assist the main interpreter for parsing and validating GML code
		 * @author	Gopi Krishna Menon
		 * @note	This class is not intended to be used directly by the Users of GML. Although the functions in the API do not
					have any dependency with the main Interpreter class, I advise users not to use them for any other purpose as
					the details may change with time
		*/
	class ParsingTools {

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
		/// @param		forbidden_list  A collection of characters that must not appear before the seperator
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



		static std::pair<RawRecord, bool> fetchRawRecord(std::string::const_iterator stream_pos, const std::string::const_iterator end_pos, const GDML_SYMBOL_PROFILE& syntax_profile);

		static std::optional<std::pair<std::string, gml::TagValue_T>> processSplitToken(const std::string& text, const GDML_SYMBOL_PROFILE& syntax_profile);



	};
}