#include "pch.h"
#include "gml_symbol_profile.h"

gml::GDML_SYMBOL_PROFILE::GDML_SYMBOL_PROFILE() {}

gml::GDML_SYMBOL_PROFILE::GDML_SYMBOL_PROFILE(const  char _tag_seperator, const  char _tag_value_seperator, const  char _tag_open, const  char _tag_close)
	:tag_seperator(_tag_seperator), tag_value_seperator(_tag_value_seperator), tag_open(_tag_open), tag_close(_tag_close) {}


//----------------------------------------
void gml::GDML_SYMBOL_PROFILE::setClosingCharacter(char _closing_character) { closing_character = _closing_character; }
char gml::GDML_SYMBOL_PROFILE::getClosingCharacter() { return  closing_character; }
//----------------------------------------

//----------------------------------------
void gml::GDML_SYMBOL_PROFILE::setAttributeSeperator(char _attribute_seperator) { attribute_seperator = _attribute_seperator; }
char gml::GDML_SYMBOL_PROFILE::getAttributeSeperator() { return attribute_seperator; }
//----------------------------------------

//----------------------------------------
void gml::GDML_SYMBOL_PROFILE::setAttributelistOpen(char open_attribute) { attribute_open = open_attribute; }
char gml::GDML_SYMBOL_PROFILE::getAttributeListOpen() { return attribute_open; }
//----------------------------------------

//----------------------------------------
void gml::GDML_SYMBOL_PROFILE::setAttributeListClose(char close_attribute) { attribute_close = close_attribute; }
char gml::GDML_SYMBOL_PROFILE::getAttributeListClose() { return attribute_close; }
//----------------------------------------

//----------------------------------------
void gml::GDML_SYMBOL_PROFILE::setTagSeperator(char _tag_seperator) { tag_seperator = _tag_seperator; }
char gml::GDML_SYMBOL_PROFILE::getTagSeperator() { return tag_seperator; }
//----------------------------------------
void gml::GDML_SYMBOL_PROFILE::setTagValueSeperator(char _tag_value_seperator) { tag_value_seperator = _tag_value_seperator; }
char gml::GDML_SYMBOL_PROFILE::getTagValueSeperator() { return tag_value_seperator; }
//----------------------------------------
void gml::GDML_SYMBOL_PROFILE::setOpenTag(char opentag_character) { tag_open = opentag_character; }
char gml::GDML_SYMBOL_PROFILE::getOpenTag() { return tag_open; }
//----------------------------------------
void gml::GDML_SYMBOL_PROFILE::setCloseTag(char closetag_character) { tag_close = closetag_character; }
char gml::GDML_SYMBOL_PROFILE::getCloseTag() { return tag_close; }
//----------------------------------------
