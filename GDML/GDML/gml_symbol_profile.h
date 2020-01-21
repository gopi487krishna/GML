#pragma once

namespace gml {
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
		GDML_SYMBOL_PROFILE(const  char _tag_seperator, const  char _tag_value_seperator, const  char _tag_open, const  char _tag_close);

		//----------------------------------------
		void setClosingCharacter(char _closing_character);
		char getClosingCharacter();
		//----------------------------------------

		void setAttributeSeperator(char _attribute_seperator);
		char getAttributeSeperator();
		//----------------------------------------

		void setAttributelistOpen(char open_attribute);
		char getAttributeListOpen();
		//----------------------------------------

		void setAttributeListClose(char close_attribute);
		char getAttributeListClose();
		
		//----------------------------------------
		void setTagSeperator(char _tag_seperator);
		char getTagSeperator();
		
		//----------------------------------------
		void setTagValueSeperator(char _tag_value_seperator);
		char getTagValueSeperator();
		
		//----------------------------------------
		void setOpenTag(char opentag_character);
		char getOpenTag();
		
		//----------------------------------------
		void setCloseTag(char closetag_character);
		char getCloseTag();
		

	};
}