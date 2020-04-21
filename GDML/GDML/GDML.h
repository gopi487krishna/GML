#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <functional>
#include <variant>
#include <optional>


/**
 * @file GDML.h
 * @author Gopi Krishna Menon
 * @details Contains the main declarations of GML Classes
*/


/**
 * @brief This namespace contains all the declaration of GML Classes
 * @author Gopi Krishna Menon
*/
namespace gml {

	class GMLParser;
	using TagValue_T = std::variant<std::string, std::map<std::string, std::string>>;
	using TBE_function = std::function<bool(GMLParser * parser, std::string, TagValue_T, std::string)>;
	using Tag = std::pair<std::string, TagValue_T>;
	using Tag_Collection = std::vector<Tag>;


	struct Record {

		std::string     record_label;
		Tag_Collection  tags;
		std::string     inner_data;
	};

	class GDML_SYMBOL_PROFILE;

	struct RawRecord {

		std::string::const_iterator record_end_position;
		std::string open_tag_token_stream = "";
		std::string close_tag_token_stream = "";
		std::string inner_data;

	};

	class Schedule {


		bool status = false;
		size_t  current_location = 0;
		std::vector<Record>record_collection;

	public:

		bool isScheduled() { return status; }
		bool isValid() { return current_location <= record_collection.size() - 1 ? true : false; }
		bool stop_scheduling() {

			if (!record_collection.empty()&& current_location != record_collection.size() - 1) {

				current_location += 1;
				return false;
			}
			return !(status = false);
		}

		bool setLocation(size_t new_location) {

			if (new_location <= record_collection.size() - 1) {
				current_location = new_location;
				return true;
			}

			return false;
		}

		auto getLocation() { return current_location; }

		bool schedule(const std::string& record_label) {

			auto iter = std::find_if(record_collection.begin(), record_collection.end(), [=](const Record& record) {return record.record_label == record_label; });
			if (iter != record_collection.end()) {

				current_location = std::distance(record_collection.begin(), iter);
				return status = true;
			}
			return false;
		}
		
		bool record_present_in_cache(const std::string& record_label) {
			std::find_if(record_collection.begin(), record_collection.end(), [=](const Record& record) {return record.record_label == record_label; }) != record_collection.end();
		}
		auto& getRecordCollection() { return record_collection; }


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


	/**
	 * @brief Defines all the set of Symbols that control the Program Form and Style of GML code
	 * @author Gopi Krishna Menon
	 * @details
	 * This class contains a set of methods for manipulating the default Program Form and Style of <strong>GML</strong> code.
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
		GDML_SYMBOL_PROFILE() {}



		/// @brief  Sets the closing character that delimits the scope of a tag list
		/// @param _closing_character ASCII character for closing_symbol
		void setClosingCharacter(char _closing_character) { closing_character = _closing_character; }

		/// @brief  Gets the closing character that delimits the scope of a tag list
		/// @return char
		char getClosingCharacter() const { return  closing_character; }

		/// @brief  Sets the attribute seperator for seperating attributes in an attribute list
		/// @param _attribute_seperator ASCII character for attribute_seperator symbol
		void setAttributeSeperator(char _attribute_seperator) { attribute_seperator = _attribute_seperator; }


		/// @brief Gets the attribute_seperator for seperating attributes in an attribute list
		/// @return char
		char getAttributeSeperator() const { return attribute_seperator; }

		/// @brief Sets the open_attribute symbol for starting an attribute list for a tag
		/// @param open_attribute ASCII character for open_attribute symbol
		void setAttributelistOpen(char open_attribute) { attribute_open = open_attribute; }

		/// @brief Get the open_attribute symbol that starts an attribute list for a tag
		/// @return char
		char getAttributeListOpen() const { return attribute_open; }

		/// @brief Sets the close_attribute symbol that closes an attribute list for a tag
		/// @param close_attribute ASCII character for close_attribute symbol
		void setAttributeListClose(char close_attribute) { attribute_close = close_attribute; }

		/// @brief Gets the close_attribute symbol that closes an attribute list for a tag
		/// @return char
		char getAttributeListClose() const { return attribute_close; }

		/// @brief  Sets the tag_seperator that seperates different tags from each other in a record
		/// @param _tag_seperator ASCII character for seperating tags
		void setTagSeperator(char _tag_seperator) { tag_seperator = _tag_seperator; }

		/// @brief  Gets the tag_seperator symbol that seperates different tags from each other in a record
		/// @return char
		char getTagSeperator() const { return tag_seperator; }

		/// @brief  Sets the tag_value seperator symbol for splitting a tag into tag_value pairs 
		/// @param  _tag_value_seperator ASCII character for splitting into tag_value pairs
		void setTagValueSeperator(char _tag_value_seperator) { tag_value_seperator = _tag_value_seperator; }

		/// @brief  Gets the tag_value seperator symbol that splits a tag into tag_value pair
		/// @return char
		char getTagValueSeperator()const { return tag_value_seperator; }

		/// @brief  Sets the open_tag symbol that starts the tag list
		/// @param  opentag_character ASCII character for starting a tag list
		void setOpenTag(char opentag_character) { tag_open = opentag_character; }

		/// @brief	Gets the open_tag symbol used to start the tag list 
		/// @return char
		char getOpenTag() const { return tag_open; }

		/// @brief  Sets the closetag symbol used to end the tag list
		/// @param	closetag_character ASCII character for ending a tag list
		void setCloseTag(char closetag_character) { tag_close = closetag_character; }

		/// @brief  Gets the closetag symbol used to end the tag list
		/// @return char
		char getCloseTag() const { return tag_close; }


	};




	/**
	 * @brief Class for storing and managing the TBE functions
	 * @details This class provides an API for storing and managing the TBE functions that
				get invoked when token is encountered
	 * @author Gopi Krishna Menon
	 * @warning Users are advised never to use this class directly for creating token_cards.
	 *			In future this class will get deprecated and its functionality will be replaced completely
	 *			with the GMLTokenCard API.
	 * @see		GMLTokenCard
	*/
	class TBE_Profile
	{

		std::map<std::string, TBE_function> items;
		TBE_function tbe_filter = nullptr;


	public:


		/// @brief Used to install a token filter for a perticular TBE_profile.
		/// @details This function installs a token filter that gets invoked
		///			for every token. This functionality is similar to that of 
		///			event filters present in GUI systems.
		/// @note	The event filter being used must tell back the parser whether
		///			the data must be passed to the actual TBE_function after filtering or not
		///			through its return value. A value of <strong>true</strong> indicates that 
		///			the data must be dispatched back to the actual TBE_function
		/// @param	tbe_func A token based execution function
		void installFilter(TBE_function tbe_func) { tbe_filter = tbe_func; }

		/// @brief Removes the token_filter from the profile card
		void removeFilter() { tbe_filter = nullptr; }

		/// @brief Checks whether the token_filter is installed in the card or not
		/// @return bool
		bool is_filter_present() { return tbe_filter != nullptr; }

		/// @brief Gets the TBE_function for a specific tag_name
		/// @param tag_name Tag name for which the function needs to be obtained
		const TBE_function operator[](const std::string& tag_name);

		/// @brief Registers the TBE_function for a specific tag/token
		/// @param token_str Tag/Token name to which a TBE_function needs to be bound/added
		/// @param tbe_func	TBE_function that needs to get bound with <strong>token_str</strong>
		/// @note  If the tag name / token_ name has already been bounded to some other TBE_function
		///		  this function returns false. In order to bind a new function first unbind the existing
		///		  function using remove token and then call <strong>registerforToken</strong> with new
		///		  tbe_function as argument
		/// @return bool
		bool registerforToken(const std::string& token_str, TBE_function tbe_func);

		/// @brief Unbinds the tag name from its respective TBE_function
		/// @param token_str Tag to be Unbound
		/// @return bool
		bool removeToken(const std::string& token_str);

		/// @brief Returns the TBE_function for a perticular token/tag
		/// @param token_str Tag name for which the TBE_function is to be fetched
		/// @return TBE_function
		TBE_function getTBE_func(const std::string& token_str);

		/// @brief Executes the TBE_function which is bound to the tag.
		/// @param parser Pointer to the parser instance. Can be used for changing/ controlling the parsing strategy
		/// @param tag	 Tag name for which the respective TBE_function needs to be executed
		/// @param value	 The tag's value which can be either a single string or a collection of attributes for that tag
		/// @param data	 The record data
		/// @return bool
		/// @note  The return value is dependent upon either the token_filter or the TBE_function. If the token filter chooses
		///		  to not dispatch the data back to its corrosponding TBE_function then <strong>true</strong> is returned. Otherwise
		///		  if the tag is not bound to anyone then a value of false is returned. In simple terms this function tells whether
		///		  the execution went successful or not
		bool exec_func(GMLParser* parser, std::string& tag, gml::TagValue_T& value, std::string& data);

	};


	/**
	 * @brief	Public API for creating a token_card/ tag card
	 * @details	This class provides a public interface for the user by which he/she can create a token card
	 *			for binding various tags with TBE_functions
	 * @author	Gopi Krishna Menon
	 */
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

		/// @brief Conversion Operator for Implicitly/Explicitly converting GMLTokenCard into TBE_Profile card
		operator TBE_Profile();

		/// @brief Ties a function(TBE_function) to a tag
		/// @details Takes the tag name as argument and binds the tag  with the TBE_function 
		///			and registers the pair in the list of bindings
		/// @param token_name Tag name
		/// @note  Calling tie along will not bind a tag to the function. Rather one has to 
		///		  chain the tie function with the with function that is present inside TokenFunction
		///		  Only then the function will get bound with the tag
		/// @return TokenFunction
		TokenFunction tie(const std::string& token_name);

		/// @brief Unbinds the tag from its associated TBE_function
		/// @param token_name Tag name from which the associated TBE_function needs to be seperated
		void detachFunctionFrom(std::string& token_name);

		/// @brief Gets the TBE_function asssociated with a perticular tag
		/// @param token_name Tag name
		/// @return TBE_function
		/// @note	If no function is bound to a perticular tag name then it returns nullptr
		auto getFunctionFrom(const std::string& token_name);

		/// @brief Installs the tag_filter
		/// @param filterfunction TBE_function that will act as a filter for every tag
		/// @warning This function has the ability to prevent dispatching the data to 
		///			tags respective TBE_function hence the function must be written carefully
		bool setFilter(TBE_function filterfunction);

		/// @brief Removes the tag_filter
		void removeFilter();
	};


	//struct Schedule {
	//	bool status{ false };
	//	std::string jmp_loc_identifier{""};
	//	std::vector< std::vector < std::optional<std::pair<std::string, TagValue_T>>>::iterator > next;
	//	auto isScheduled() { return status; }
	//	auto getJmpLocIden() { return jmp_loc_identifier; }
	//	void setJmpLocIden(const std::string& jmplociden) { jmp_loc_identifier = jmplociden; }
	//	auto getNext() { return next; }
	//	auto setNext(std::vector< std::vector < std::optional<std::pair<std::string, TagValue_T>>>::iterator > iter) { next = iter; }
	//	static auto getSchedule(){
	//		static Schedule my_schedule;
	//		return my_schedule;
	//	}
	//private:
	//	Schedule() {}
	//};




	/**
	 * @brief The main parser class for parsing and running GML code.
	 * @author Gopi Krishna Menon
	*/
	class GMLParser
	{
		// Make constructors explicit

		GDML_SYMBOL_PROFILE syntax_profile;
		Record current_record;
	public:

		///@brief Default Constructor of GML Parser
		GMLParser() {}

		/// @brief Constructor for GML Parser used to set an external symbol profile 
		/// @param _syntax_profile Symbol/Syntax Profile card to be used
		/// @see	 GDML_SYMBOL_PROFILE
		explicit GMLParser(GDML_SYMBOL_PROFILE _syntax_profile) :syntax_profile(_syntax_profile) {}

		/// @brief Used to explicitly set the Syntax Profile
		/// @param _syntax_profile Symbol/Syntax Profile card to be used
		/// @see	 GDML_SYMBOL_PROFILE
		void setSyntaxProfile(GDML_SYMBOL_PROFILE _syntax_profile) { syntax_profile = _syntax_profile; }

		/// @brief	Parses and runs the entire GML code 
		/// @param	str GML Code in std::string format
		/// @param	profile GML Token Card that contains all the bindings of tags with their TBE_functions
		/// @return	int
		/// @note	The return value basically indicates a status of success or some error in the form of ERRROR Codes as defined in errorcodes.h.
		///			It is not advised to pass an object of TBE_Profile directly. Rather prefer to pass an object of GMLTokenCard as an argument
		///			(conversions take place automatically) containing the tags and their associations with TBE_functions
		/// @see		TBE_profile
		int exec(const std::string& str, TBE_Profile profile);

		auto getCurrentRecord() { return current_record; }


		Schedule parser_schedule;
	};

}
