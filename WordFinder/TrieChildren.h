/** 
* @file TrieChildren.h
* @brief Trie Class that represent a Trie with 26 children. A child for each alphabet.
* @author Manojkumar M Waghmare
* @version 1.0
* @date 2011-11-04
*/
#ifndef __TRIECHILDREN_H__
#define __TRIECHILDREN_H__

#include<ITrie.h>

#define NUM_CHARS	26

class TrieChildren : public ITrie
{

    public:

	/** 
	 * @brief Constructor
	 * 
	 * @param label [in] - label for the second parameter i.e. child
	 * @param child [in] - child to be added to this Trie object.
	 * @param word [in] - word for which the Trie object hierarchy is to be created.
	 * @param storeEmpty [in] - sets/unsets empty state of the object.
	 */
	TrieChildren(const char label, ITrie* child, 
		const std::string& word, const bool storeEmpty);

	virtual ITrie* Add(const std::string& word);

	virtual ITrie* Next(const char ch);

	virtual bool IsEmpty() const;

    private:

	virtual	~TrieChildren();

	/* prohibit copy construction and assignment */
	TrieChildren(const TrieChildren&);
	TrieChildren& operator=(const TrieChildren& rhs);

    private:

	bool		isEmpty_;
	ITrie*		children_[NUM_CHARS];
};


#endif /* __TRIECHILDREN_H__ */
