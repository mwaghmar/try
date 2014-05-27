/** 
* @file TrieChild.h
* @brief Trie Class that represent a Trie with just one child.
* @author Manojkumar M Waghmare
* @version 1.0
* @date 2011-11-04
*/
#ifndef __TRIECHILD_H__
#define __TRIECHILD_H__

#include<ITrie.h>

class TrieChild : public ITrie
{

    public:

	/** 
	 * @brief Constructor
	 * 
	 * @param word [in] - word for which the Trie object hierarchy is to be created.
	 * @param storeEmpty [in] - sets/unsets empty state of the object.
	 */
	TrieChild(const std::string& word, const bool storeEmpty);

	virtual ITrie* Add(const std::string& word);

	virtual ITrie* Next(const char ch);

	virtual bool IsEmpty() const;

    private:

	virtual	~TrieChild();

	/* prohibit copy construction and assignment */
	TrieChild(const TrieChild&);
	TrieChild& operator=(const TrieChild& rhs);

    private:

	bool		isEmpty_;
	char		label_;
	ITrie*		child_;
};


#endif /* __TRIECHILD_H__ */
