/** 
* @file Trie.h
* @brief Trie Class that represent a Trie with no children.
* @author Manojkumar M Waghmare
* @version 1.0
* @date 2011-11-04
*/
#ifndef __TRIE_H__
#define __TRIE_H__

#include<ITrie.h>

class Trie : public ITrie
{

    public:

	/** 
	 * @brief Constructor. 
	 * 
	 * @param isEmpty [in] - sets/unsets empty state of the object. 
	 */
	Trie(const bool isEmpty = false) : isEmpty_(isEmpty)
	{ }

	virtual ITrie* Add(const std::string& word);

	virtual ITrie* Next(const char ch);

	virtual bool IsEmpty() const;

	virtual ~Trie()
	{}

    private:

	bool	isEmpty_;
};


#endif /* __TRIE_H__ */
