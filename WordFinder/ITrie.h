/** 
* @file ITrie.h
* @brief Interface for Trie Data Structure
* @author Manojkumar M Waghmare
* @version 1.0
* @date 2011-11-04
*/
#ifndef __ITRIE_H__
#define __ITRIE_H__

#include<string>

class ITrie
{

    public:

	/** 
	 * @brief Adds the given word to the Trie.
	 * 
	 * @param word [in] - word to be added to the Trie.
	 * 
	 * @return returns a Trie object that represents 
	 *	    Trie hierarchy containing newly added word
	 */
	virtual ITrie* Add(const std::string& word) = 0;


	/** 
	 * @brief Search if the Trie object has same label as the given char
	 * 
	 * @param ch [in] - character to be checked with the Trie's label
	 * 
	 * @return - returns Trie object if label of Trie mataches with given ch,
	 *		NULL otherwise	
	 */
	virtual ITrie* Next(const char ch) = 0;

	/** 
	 * @brief Checks if the Trie is Empty
	 * 
	 * @return - true if Trie is empty, false otherwise.
	 */
	virtual bool IsEmpty() const = 0;


	/** 
	 * @brief Virtual Desctrutor 
	 */
	virtual ~ITrie() 
	{
	}

};

#endif /* __ITRIE_H__ */
