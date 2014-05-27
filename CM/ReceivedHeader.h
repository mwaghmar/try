/** 
* @file ReceivedHeader.h
* @brief Class representing received header in mail headers.
* @author Manojkumar M Waghmare
* @version 1.0
* @date 2011-10-22
*/

#ifndef __RECEIVEDHEADER_H__
#define __RECEIVEDHEADER_H__

#include <string>

/** 
* @brief Class for representing Received Header from a set of Mail Headers.
*	 This class stores only 3 components namely 'from', 'by' and 'date' from received header.
*
*	 Received header may not contain one or more of these components.
*	 Missing component is represented by empty string i.e. "".
*
*	 In mail headers the received header looks as follows -
*
*	 Received: from [68.230.240.27] ([68.230.240.27:59584] helo=eastrmmtai108.cox.net)
*		by zest.cage.cloudmark.com (envelope-from <m4253525@spamnet.com>)
*		(ecelerity 2.2.2.34 r(26258)) with ESMTP
*		id 67/95-03818-2A6E64B4; Fri, 08 Jan 2010 00:02:42 -0800
*
* @Note 
*	Assumption 1 - Date component of received header is represented in format 
*			(Mon|Tue|Wed|Thu|Fri|Sat|Sun), DD MMM YYYY HH:MM:SS -Digits*        
*		       Thus Date string has total 6 tokens seperated by space.	
*/
class ReceivedHeader
{
    public:
    
	//Following are the tags that appear in received header except date tag
	static const char*	HeaderTag;	/*= "Received:";*/ 
	static const char*	FromTag;	/*= "from";	*/
	static const char*	ByTag;		/*= "by";	*/
	static const char*	DateTag;	/*= "date";	*/

	//Number of tokens in the date component of the received header 
	static const unsigned	NumDateTokens;	/*= 6;		*/

	//Enumeration for fields in the received header.
	typedef enum {	From = 0,	//<- from field
			By = 1,		//<- by field 
			Date = 2,	//<- date field	
			MaxFieldEnum    //<- Total number of fields in the class.
		    } Fields_t;

	/** 
	 * @brief Constructor for ReceivedHeader.
	 * 
	 * @param from [in] - from component of the received header.
	 * @param by   [in] - by component of the received header.
	 * @param date [in] - date component of the received header.
	 */
	ReceivedHeader(const std::string& from = "", 
			const std::string& by = "", 
			const std::string& date = "") : from_(from), by_(by), date_(date)
	{
	}

	/** 
	 * @brief Sets given field of the class with the given value.
	 * 
	 * @param field [in] - Enum of the field to be set.
	 * @param value [in] - Value to be assigned to the given field.
	 *
	 * @throws invalid_argument exception if field >= MaxFieldEnum 
	 */
	void set(const Fields_t field, const std::string& value);

	/** 
	 * @brief gets value of the given field.
	 * 
	 * @param field [in] - Enum of the field whose value is to be retrieved.
	 * 
	 * @return value of the requested field. Throws invalid_argument if field >= MaxFieldEnum
	 */
	std::string get(const Fields_t field) const;

	/** 
	 * @brief prints the header.
	 */
	void print() const;

	/** 
	 * @brief checks if the given word is first token of the Date format.
	 * 
	 * @param word [in] - word to be checked.
	 * 
	 * @return true if word is first token of Date, false otherwise.
	 */
	static bool isDateWord(const std::string& word);

	/** 
	 * @brief clears contents of all fields in the class.
	 */
	void clear()
	{
	    from_ = by_= date_ = "";
	}

	/** 
	 * @brief checks if all the fields of the class are empty/missing.
	 * 
	 * @return  true if all the fields are empty/missing, false otherwise
	 */
	bool empty() const
	{
	    return ((from_ == "") && (by_ == "") && (date_ == ""));
	}

	~ReceivedHeader() 
	{ } 

    private:
	
	//components of the received header. If any of the component is missing
	//it's value is set to empty string i.e. ""
	std::string	from_;	
	std::string	by_;
	std::string	date_; 
};

#endif /* __RECEIVEDHEADER_H__ */
