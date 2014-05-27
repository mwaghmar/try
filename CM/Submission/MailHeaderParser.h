/** 
* @file MailHeaderParser.h
* @brief Facade class that implements parsing functionaliy for headers
* @author Manojkumar M Waghmare
* @version 1.0
* @date 2011-10-22
*/
#ifndef __MAILHEADERPARSER_H__
#define __MAILHEADERPARSER_H__

#include <string>
#include <vector>
#include "ReceivedHeader.h"


/** 
* @brief Facade that implements parsing functionaliy for headers.
*	 Currently it parses only Received Headers from mail headers.
*
* @Note 
*	Assumption 1 - Tokens are space seperated words of stream.
*	Assumption 2 - Header Tokens are the words ending with ':'
*/
class MailHeaderParser
{
   
    public: 
	//Typedef for List of parsed ReceivedHeader 
	typedef std::vector<ReceivedHeader> ReceivedHeaderList;
	
	/** 
	 * @brief parses received headers from the given input file that contains mail headers.
	 * 
	 * @param headerFile [in] - File containing mail headers.
	 * @param rhList [out] - upon return this parameter holds the list of parsed ReceivedHeader.
	 */
	static void parseReceivedHeader(const char* headerFile, ReceivedHeaderList& rhList);

	/** 
	 * @brief parses received headers from the given input string.
	 * 
	 * @param headerString [in] - String containing mail headers.
	 * @param rhList [out] - upon return this parameter holds the list of parsed ReceivedHeader.
	 */
	static void parseReceivedHeader(const std::string& headerString, ReceivedHeaderList& rhList);
	
	/** 
	 * @brief prints the given list of ReceivedHeader.
	 * 
	 * @param rhList [in] - list of ReceivedHeader to be printed.
	 */
	static void printHeaders(const ReceivedHeaderList& rhList);

    private:

	/** 
	 * @brief This is the work horse for parseReceivedHeader functions
	 * 
	 * @tparam StreamIter An iterator over tokens in the stream. 
	 * @param itr_word [in/out] - beginning iterator. upon return it is modified. 
	 * @param itr_word_end [in] - end iterator
	 * @param rhList [out] - upon return holds the list of parsed ReceivedHeader.
	 */
	template<class StreamIter>
	static void __parseReceivedHeader(StreamIter& itr_word, const StreamIter& itr_word_end,
						ReceivedHeaderList& rhList);

	/** 
	 * @brief checks if the given word is a header or not.
	 *	   A word is considered as Header if it ends with char ':'.
	 *
	 * @param word [in] - word to checked 
	 * 
	 * @return true if the given word is a header, false otherwise.
	 */
	static bool isHeader(const std::string& word);
};

#endif /* __MAILHEADERPARSER_H__ */ 
