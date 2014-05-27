/** 
 * @file CommandObject.h
 * @brief File that implements Generic Command Object Pattern for ACE_Method_Request
 * @author Manojkumar M Waghmare
 * @version 1.0
 * @date 2011-04-11
 */
#ifndef __COMMANDOBJECT_H__
#define __COMMANDOBJECT_H__

#include "ace/Method_Request.h"
#include "ace/Future.h"

template<class Obj, class Functor, class RetType = int>
class CommandObject : public ACE_Method_Request
{
	public:

		CommandObject(Obj& obj, Functor functor, ACE_Future<RetType>& retVal) 
				: obj_(obj), functor_(functor), retVal_(retVal)
		{
		}

		virtual int call()
		{
			retVal_.set(functor_(obj_));
			return 0;
		}

	private:

		Obj&					obj_;
		Functor					functor_;
		ACE_Future<RetType>		retVal_;
};

#endif 	/* __COMMANDOBJECT_H__ */
