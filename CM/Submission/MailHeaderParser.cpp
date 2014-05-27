#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <functional>
#include "MailHeaderParser.h"

bool MailHeaderParser::isHeader(const std::string& word)
{
    return word[word.length() - 1] == ':';
}

void MailHeaderParser::printHeaders(const ReceivedHeaderList& rhList)
{
    for_each(rhList.begin(), rhList.end(), std::const_mem_fun_ref_t<void, ReceivedHeader>(&ReceivedHeader::print));
}

void MailHeaderParser::parseReceivedHeader(const std::string& headerString, ReceivedHeaderList& rhList)
{
    std::istringstream istr(headerString);
    std::istream_iterator<std::string> itr_word(istr), itr_word_end;

    __parseReceivedHeader(itr_word, itr_word_end, rhList);
}

void MailHeaderParser::parseReceivedHeader(const char* headerFile, MailHeaderParser::ReceivedHeaderList& rhList)
{
    std::fstream mailHeaderStream(headerFile);
    std::istream_iterator<std::string> itr_word(mailHeaderStream), itr_word_end;

    __parseReceivedHeader(itr_word, itr_word_end, rhList);
}

template<class StreamIter>
void MailHeaderParser::__parseReceivedHeader(StreamIter& itr_word, const StreamIter& itr_word_end,
						ReceivedHeaderList& rhList)
{
    rhList.clear();
    ReceivedHeader rcvdHeader;    

    for(bool headerStarted=false; itr_word != itr_word_end; ++itr_word) {

	if( *itr_word == ReceivedHeader::HeaderTag ) {
	    headerStarted = true;
	    if( !rcvdHeader.empty() ) {
		rhList.push_back(rcvdHeader);
		rcvdHeader.clear();
	    }    
	    continue;
	}

	if( headerStarted ) { //ReceivedHeader started
	    if( *itr_word == ReceivedHeader::FromTag ) {
		++itr_word;
		rcvdHeader.set(ReceivedHeader::From, *itr_word);
		continue;
	    }

	    if( *itr_word == ReceivedHeader::ByTag ) {
		++itr_word;
		rcvdHeader.set(ReceivedHeader::By, *itr_word);
		continue;
	    }

	    if( ReceivedHeader::isDateWord(*itr_word) ) {
		std::string date = *itr_word;
		for(unsigned i = 1; i < ReceivedHeader::NumDateTokens; ++i) {
		    if( isHeader(*(++itr_word)) ) break;	//If date format is corrupt
		    date += " ";
		    date += *itr_word;
		}
		rcvdHeader.set(ReceivedHeader::Date, date);
		continue;
	    }

	    if( isHeader(*itr_word) ) { //some header starts
		headerStarted = false;
	    }
	}
	//std::cerr << *itr_word << std::endl;
    } //for ends

    if( !rcvdHeader.empty() ) {
	rhList.push_back(rcvdHeader);
    }
}
