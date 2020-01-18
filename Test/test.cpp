#define BOOST_TEST_MODULE test
#include <boost/test/unit_test.hpp>
#include "../GDML/GDML/GDML.h"

BOOST_AUTO_TEST_CASE(basic_parser_test){

    std::string test_output_string="";
    auto basic_test_func=[&](gml::GDMLParser*parser,std::string tag,std::string value, std::string data){
        test_output_string=data;
        return true;};
    
    gml::GMLTokenCard card;
    gml::GDMLParser parser;
    
    card.tie("print").with(basic_test_func);
    parser.exec("[print:]Hello[/]",card);
    
    BOOST_CHECK(test_output_string=="Hello");

}
