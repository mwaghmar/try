#include "Scheduler.h"
#include <memory>

Scheduler::Scheduler()
{
	//ACE_TRACE("Scheduler::Scheduler");
	this->activate();
}

Scheduler::~Scheduler()
{
	while( ! (activation_queue_.is_empty()) ) {
		ACE_Time_Value tv(1);
		ACE_Thread_Manager::instance()->wait(&tv);
	}
}

int Scheduler::svc()
{
	//ACE_TRACE("Scheduler::svc");
	while(1)
	{
		// Dequeue the next method object
		auto_ptr<ACE_Method_Request>
			request(this->activation_queue_.dequeue ());
		// Dequeue the next method object
		if(request->call() == -1)
			break;
				//invoke the method request
	}
	return 0;
}

int Scheduler::enqueue(ACE_Method_Request* request)
{
	//ACE_TRACE("Scheduler::enqueue");
	return this->activation_queue_.enqueue(request);
}

