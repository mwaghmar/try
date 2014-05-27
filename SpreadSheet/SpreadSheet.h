/** 
* @file SpreadSheet.h
* @brief SpreadSheet class interface.
* @author Manojkumar M Waghmare
* @version 1.0
* @date 2011-11-22
*/
#ifndef __SPREADSHEET_H__
#define __SPREADSHEET_H__

#include <vector>
#include <string>

/** 
* @brief SpreadSheet class.
*/
class SpreadSheet
{

    public:

	typedef std::vector<std::string>    SheetRow;
	typedef std::vector<SheetRow>	    Sheet;

	/** 
	 * @brief Default Constructor
	 */
	SpreadSheet() {}

	/** 
	 * @brief Parameterized constructor. Given the csv spread sheet file it constructs
	 *	    the corresponding SpreadSheet object.
	 * 
	 * @param fileName [in] - Spread Sheet file in csv format.
	 * @throws std::invalid_argument in case if file does not exist or it is corrupt.
	 */
	SpreadSheet(const std::string& fileName);

	/** 
	 * @brief evaluates the SpreadSheet and returns the result of evaluation.
	 * 
	 * @param resultSheet [out] - upon return it holds the result of SpreadSheet evaluation.
	 * 
	 * @return evaluated spread sheet. Returns reference to param resultSheet.
	 * @note - throws invalid_argument, range_error or std::exception if it's not able to evaluate.
	 */
	SpreadSheet& evaluate(SpreadSheet& resultSheet);

	/** 
	 * @brief returns the contents of the given cell.
	 * 
	 * @param cellID [in] - id of the cell whose contents are to be retrieved.
	 *		cellID is of form A9 where A indicates column and 9 indicates row.
	 * 
	 * @return string at the given cell ID.
	 */
	std::string getCellString(const std::string& cellID);

	/** 
	 * @brief outputs the contents of SpreadSheet in CSV format.
	 * 
	 * @param ostr [in/out] - ostream
	 * @param ss [in] - spread sheet to be printed.
	 * 
	 * @return ostream
	 */
	friend std::ostream& operator<< (std::ostream& ostr, const SpreadSheet& ss);

	void swap(SpreadSheet& ss)
	{
	    spreadSheet_.swap(ss.spreadSheet_);	    
	}
	
	/** 
	 * @brief Destructor
	 */
	~SpreadSheet() {} 

    private:

	
	/** 
	 * @brief evaluates the given cell string.
	 * 
	 * @param cellString [in] - string to be evaluated.
	 * @param result [out] - upon return holds the result of evaluation.
	 * 
	 * @return evaluation result for the given cell string.
	 */
	std::string& evaluateCell(const std::string& cellString, std::string& result);

    private:

	Sheet	spreadSheet_;
};

#endif /* __SPREADSHEET_H__ */
