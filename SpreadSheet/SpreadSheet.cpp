#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <stack>
#include <cstdlib>
#include <ctype.h>
#include "SpreadSheet.h"


namespace { //helper functions

    inline std::string trim_right(const std::string &source , const std::string& t = " ")
    {
	std::string str = source;
	return str.erase(str.find_last_not_of(t) + 1);
    }

    inline std::string trim_left( const std::string& source, const std::string& t = " ")
    {
	std::string str = source;
	return str.erase(0 , source.find_first_not_of(t) );
    }

    /** 
     * @brief Trims spaces at the beginning and at the end of the given string
     * 
     * @param source [in] - string to be trimmed
     * @param t [in] - characters to be trimmed
     * 
     * @return trimmed string.
     */
    inline std::string trim(const std::string& source, const std::string& t = " ")
    {
	std::string str = source;
	return trim_left(trim_right(str,t), t);
    } 

    //checks if given string is operator. 
    inline bool isOperator(const std::string& str) 
    {
	return (str.length() && (str[0] == '+' || str[0] == '-' || str[0] == '*' || str[0] == '/'));
    }

    /** 
     * @brief Evaluates given operands using given operator
     * 
     * @param oper [in] - operator.
     * @param exp1 [in] - left hand side of the operator.
     * @param exp2 [in] - right hand side of the operator.
     * 
     * @return result of exp1 oper exp2 in string format.
     */
    std::string evaluateExp(const std::string oper, const std::string& exp1, const std::string& exp2)
    {
	const double e1 = atof(exp1.c_str());
	const double e2 = atof(exp2.c_str());
	std::ostringstream ostr;

	switch(oper[0]) {
	    case '+':
		ostr << (e1 + e2);
		break;
	    case '-':
		ostr << (e1 - e2);
		break;
	    case '*':
		ostr << (e1 * e2);
		break;
	    case '/':
		ostr << (e1 / e2);
		break;
	}

	return ostr.str();
    }

    /** 
     * @brief Evaluates the given postfix expression string.
     * 
     * @param exp [in] - postfix expression string to be evaluated.
     * @param result [out] - upon return holds the result of the postfix expression evaluation 
     *                       in string format.
     * 
     * @return result of the postfix expression evaluation in string format
     */
    std::string& evaluatePostfixExpression(const std::string& exp, std::string& result)
    {
	std::istringstream istr(exp);
	std::istream_iterator<std::string> itr_word(istr), itr_word_end;    
	std::stack<std::string> operandStack;

	for( ;itr_word != itr_word_end; ++itr_word) {
	    if(isOperator(*itr_word)) {
		const std::string exp2 = operandStack.top();  operandStack.pop();
		const std::string exp1 = operandStack.top();  operandStack.pop();
		operandStack.push(evaluateExp(*itr_word, exp1, exp2));
	    }		
	    else {
		operandStack.push(*itr_word);
	    }
	}

	result.swap(operandStack.top());
	return result;
    }
};

//class member functions and friend functions

std::ostream& operator<< (std::ostream& ostr, const SpreadSheet& ss)
{
    for(SpreadSheet::Sheet::const_iterator sitr = ss.spreadSheet_.begin(); sitr != ss.spreadSheet_.end(); ++sitr) {
	for(SpreadSheet::SheetRow::const_iterator ritr = sitr->begin(); ritr != sitr->end(); ++ritr) {
	    ostr << *ritr << ", ";
	}
	ostr << "\n";
    }
    return ostr;
}

SpreadSheet::SpreadSheet(const std::string& fileName)
{
    std::fstream csvFileStream(fileName.c_str());
    if( csvFileStream.fail() ) {
	throw std::invalid_argument("Error Reading File: " + fileName);
    }

    Sheet tmpSpreadSheet;
    std::string line;
    size_t rowNum = 0;
    while( csvFileStream ) {

	if(!getline(csvFileStream, line)) break;
	std::stringstream ss(line);
	SheetRow row;
	while(ss) {
	    std::string str;
	    if( !getline(ss, str, ',') ) break;
	    row.push_back(str);
	}
	++rowNum;
	tmpSpreadSheet.push_back(row);
    }

    if(!csvFileStream.eof()) {
	throw std::invalid_argument("Corrupt File: " + fileName);
    }

    csvFileStream.close(); 
    spreadSheet_.swap(tmpSpreadSheet);
}

std::string SpreadSheet::getCellString(const std::string& cellID)
{
    std::string cID = trim(cellID);
    if( !isalpha(cID[0]) ) {
	throw std::invalid_argument("Bad Cell ID: " + cellID);
    }
   
    const size_t spreadSheetColNum = tolower(cID[0]) - 'a';
    const size_t spreadSheetRowNum = atoi(cID.substr(1).c_str()) - 1;

    if( spreadSheet_.size() < spreadSheetRowNum || 
	spreadSheet_[spreadSheetRowNum].size() < spreadSheetColNum ) {
	throw std::range_error("Cell Row/Col Num Out of Range: " + cellID);
    }

    return trim((spreadSheet_[spreadSheetRowNum])[spreadSheetColNum]);
}

std::string& SpreadSheet::evaluateCell(const std::string& cellString, std::string& result)
{
    std::string tmpResult = trim(cellString); 
    if( tmpResult[0] == '=' ) {
	const std::string exp =  trim(tmpResult.substr(1));
	if( isalpha(exp[0]) ) { 
	    //tmpResult = getCellString(exp);  //@TODO make it recursive
	    evaluateCell(getCellString(exp), tmpResult);  
	} else {
	    evaluatePostfixExpression(exp, tmpResult);
	}
    }

    result.swap(tmpResult);
    return result;
}

SpreadSheet& SpreadSheet::evaluate(SpreadSheet& resultSheet)
{
    Sheet rs;
    SheetRow row;

    for(Sheet::const_iterator sitr = spreadSheet_.begin(); sitr != spreadSheet_.end(); ++sitr) {
	row.clear();
	for(SheetRow::const_iterator ritr = sitr->begin(); ritr != sitr->end(); ++ritr) {
	    std::string result;
	    row.push_back(evaluateCell(*ritr, result));     
	}
	rs.push_back(row);
    }

    resultSheet.spreadSheet_.swap(rs);
    return resultSheet;
}


