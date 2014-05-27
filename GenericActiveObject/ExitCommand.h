/** 
 * @file ExitCommand.h
 * @brief Implements ExitCommand which is required to shutdown an Active Object.
 * @author Manojkumar M Waghmare
 * @version 1.0
 * @date 2011-04-20
 */
#ifndef __EXITCOMMAND_H__
#define __EXITCOMMAND_H__

#include "ace/Method_Request.h"

class ExitCommand : public ACE_Method_Request
{
	public:

		virtual int call()
		{
			return -1;
		}
};

#endif 	/* __EXITCOMMAND_H__ */
