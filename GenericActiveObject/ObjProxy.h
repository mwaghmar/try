/** 
 * @file ObjProxy.h
 * @brief Facade for Generic ActiveObject pattern implementation.
 * @author Manojkumar M Waghmare
 * @version 1.0
 * @date 2011-04-20
 */
#ifndef __OBJPROXY_H__
#define __OBJPROXY_H__

#include <iostream>
#include "CommandObject.h"
#include "ExitCommand.h"
#include "Scheduler.h"

template <class TestObjType>
class ObjProxy
{
	//This acts as a proxy to the controller impl object.
	public:
		
		template <class Functor, class RetType = int> 
		ACE_Future<RetType> operator()(Functor f)	
		{
			ACE_Future<RetType> result;		
			this->scheduler_.enqueue(new CommandObject<TestObjType,Functor,RetType>(obj_,f, result));

			return result;
		}

		void exit()
		{
			this->scheduler_.enqueue(new ExitCommand());
		}

		~ObjProxy()
		{

		}

	private:

		Scheduler 		scheduler_;
		TestObjType 	obj_;
};

#if 0
		template <class Functor, class RetType = int> 
		ACE_Future<RetType> execute(Functor f)	
		{
			ACE_Future<RetType> result;		
			this->scheduler_.enqueue(new CommandObject<TestObjType,Functor,RetType>(obj_,f, result));

			return result;
		}
#endif
#endif 	/* __OBJPROXY_H__ */
