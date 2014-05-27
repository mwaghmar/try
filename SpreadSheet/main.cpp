#include <stdexcept>
#include <iostream>
#include <fstream>
#include "SpreadSheet.h"

int main(int argc, char* argv[])
{
    if(argc < 2) {
	std::cerr << "\nUsage: kontagent_ss <CSV File Name> <Target File>\n";
	return -1;
    }
    
    std::string targetFile = "";    
    if( argc > 2 ) {
	targetFile = argv[2];
    }

    try {
	SpreadSheet spreadSheet(argv[1]);
	std::cout << "\nInput Sheet: \n";
	std::cout << spreadSheet;

	SpreadSheet resultSheet;
	spreadSheet.evaluate(resultSheet);
	std::cout << "\nEvaluated Sheet: \n";
	std::cout << resultSheet;
    
	if(targetFile.length()) {
	    std::ofstream otarget(targetFile.c_str());
	    otarget << resultSheet;
	    otarget.close();
	    std::cout << "\nWritten the result to file: " << targetFile << std::endl;
	}
    }
    catch(const std::invalid_argument& e) {
	std::cerr << e.what() << std::endl;
    }
    catch(const std::range_error& e) {
	std::cerr << e.what() << std::endl;
    }
    catch(const std::exception& e) {
	std::cerr << e.what() << std::endl;
    }

    return 0;
}


#if 0
//referred from http://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring
struct tokens: std::ctype<char> 
{
    tokens(): std::ctype<char>(get_table()) {}

    static std::ctype_base::mask const* get_table()
    {
        typedef std::ctype<char> cctype;
        static const cctype::mask *const_rc= cctype::classic_table();

        static cctype::mask rc[cctype::table_size];
        std::memcpy(rc, const_rc, cctype::table_size * sizeof(cctype::mask));

        rc[','] = std::ctype_base::space; 
        //rc[' '] = std::ctype_base::space; 
        rc[' '] = std::ctype_base::alpha;   //consider space as alpha char
        return &rc[0];
    }
};

    std::string s = "right way, wrong way, correct way\na,b,c,d";
    ss.imbue(std::locale(std::locale(), new tokens()));

    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> vstrings(begin, end);
    std::copy(vstrings.begin(), vstrings.end(), std::ostream_iterator<std::string>(std::cout, "\n"));

#endif

