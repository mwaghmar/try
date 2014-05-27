#include <iostream>
#include <exception>
#include <iterator>
#include "MailHeaderParser.h"

int main(int argc, char* argv[])
{

    try {

	if( argc >= 2 ) { //if input files are given the just process them
	    MailHeaderParser::ReceivedHeaderList rhList;

	    for(int i = 1; i <= argc - 1; ++i) {
		std::cout << std::endl;
		std::cout << "======================================" << std::endl 
			  << "Parsing File: " << argv[i] << std::endl
			  << "======================================" << std::endl;
  
		MailHeaderParser::parseReceivedHeader(argv[i], rhList);
		MailHeaderParser::printHeaders(rhList);
	    }
	}
	else { //get the header string from standard input
	    std::cout << "Enter your header String terminated by EOF i.e. ^D: " << std::endl;
	    std::cin >> std::noskipws;

	    // use stream iterators to copy the stream to a string
	    std::istream_iterator<char> itr(std::cin);
	    std::istream_iterator<char> end;
	    std::string headerString(itr, end);
	    
	    if(headerString.length()) {
		MailHeaderParser::ReceivedHeaderList rhList;
		MailHeaderParser::parseReceivedHeader(headerString, rhList);
		MailHeaderParser::printHeaders(rhList);
	    }
	}
	return 0;

	/*const char* mailHeaderFile = "sample_email_headers.txt";
	MailHeaderParser::ReceivedHeaderList rhList;
	MailHeaderParser::parseReceivedHeader(mailHeaderFile, rhList);
	MailHeaderParser::printHeaders(rhList);
	return 0;*/
    }
    catch(std::exception& e) {
	std::cerr << "Caught Exception: " << e.what() << std::endl;
    }
}


