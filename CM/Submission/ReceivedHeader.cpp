#include <stdexcept>
#include <iostream>

#include "ReceivedHeader.h"

const char* ReceivedHeader::HeaderTag		= "Received:";
const char* ReceivedHeader::FromTag		= "from";
const char* ReceivedHeader::ByTag		= "by";
const char* ReceivedHeader::DateTag		= "date";
const unsigned ReceivedHeader::NumDateTokens	= 6;

void ReceivedHeader::set(const Fields_t field, const std::string& value)
{
    switch(field) 
    {
	case From:
	    from_ = value;
	    break;
	case By:
	    by_ = value;
	    break;
	case Date:
	    date_ = value;
	    break;
	default:
	    throw std::invalid_argument("Invalid ReceivedHeader::Field_t value");
    }
}	

std::string ReceivedHeader::get(const Fields_t field) const
{
    switch(field) 
    {
	case From:
	    return from_;
	case By:
	    return by_;
	case Date:
	    return date_;
    }
    throw std::invalid_argument("Invalid ReceivedHeader::Field_t value");
}

void ReceivedHeader::print() const
{
    std::cout << "##### Received Header #####\n" << std::endl;   

    std::cout	<< FromTag << "=" << from_ << std::endl
		<< ByTag << "=" << by_ << std::endl 
		<< DateTag << "=" << date_ << std::endl
		<< std::endl;   
}

/*Function assumes the Date format as - 
  (Mon|Tue|Wed|Thu|Fri|Sat|Sun), DD MMM YYYY HH:MM:SS -Digits */        
bool ReceivedHeader::isDateWord(const std::string& word) 
{
    static const char* Mon = "Mon,";
    static const char* Tue = "Tue,";
    static const char* Wed = "Wed,";
    static const char* Thu = "Thu,";
    static const char* Fri = "Fri,";
    static const char* Sat = "Sat,";
    static const char* Sun = "Sun,";
 
    return word == Mon || word == Tue || word == Wed || 
	    word == Thu || word == Fri || word == Sat 
	    || word == Sun;
}
