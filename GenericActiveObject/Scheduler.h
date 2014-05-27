#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include "ace/Task.h"
#include "ace/Method_Request.h"
#include "ace/Activation_Queue.h"

class Scheduler : public ACE_Task_Base
{
	public:

		Scheduler();
		virtual int svc();
		int enqueue(ACE_Method_Request* request);
		//int is_empty() { return activation_queue_.is_empty(); }
		~Scheduler();

	private:

		ACE_Activation_Queue activation_queue_;
};

#endif 	/* __SCHEDULER_H__ */
