#define BOOST_TEST_MODULE test
#include <boost/test/unit_test.hpp>
#include "../GDML/GDML/GDML.h"
#include <sstream>
BOOST_AUTO_TEST_CASE(basic_parser_test){

    std::string test_output_string="";
    auto basic_test_func=[&](gml::GDMLParser*parser,std::string tag,std::variant<std::string,std::map<std::string,std::string>> value, std::string data){
        test_output_string=data;
        return true;};
    
    gml::GMLTokenCard card;
    gml::GDMLParser parser;
    
    card.tie("print").with(basic_test_func);
    parser.exec("[print:]Hello[/]",card);
    
    BOOST_CHECK(test_output_string=="Hello");

}
BOOST_AUTO_TEST_CASE(object_structuring_syntax_test)
{
    std::string output_string="";
    std::string expected_string="roll-20056semester-6";
    
    auto token_func=[&](gml::GDMLParser*parser,std::string tag,std::variant<std::string,std::map<std::string,std::string>> value, std::string data){
      
       
        
        if (const auto mapptr(std::get_if<std::map<std::string, std::string>>(&value)); mapptr)
        {

        std::stringstream my_stream;
		for (auto [x, y] : *mapptr)
            {

			my_stream<< x << "-" << y;
            }
        output_string=my_stream.str();

        }
        
        return true;};
        
       
    gml::GMLTokenCard card;
    gml::GDMLParser parser;
    card.tie("Student").with(token_func);
    parser.exec("[Student:{semester:6,roll:20056}]Hello[/]",card);
    BOOST_CHECK(output_string==expected_string);

    
}
