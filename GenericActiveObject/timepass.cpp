#include<iostream>
#include"MemFunctor.h"

#define MEMFUNCTOR(PtrMemFunction, ...)					MemFunctor(PtrMemFunction, __VA_ARGS__)

#define RETMEMFUNCTOR(RetType, ...)						RetMemFunctor<ReturnType>(__VA_ARGS__)

class A
{

	public:
		int oneParamFun(int a)
		{
			return 100;
		}

		void voidOneParamFun(int a)
		{
			std::cerr << "voidOneParamFun"  << a << std::endl;
		}
	
};


template <class ObjType>
class CommandObject
{
	public:
		
		CommandObject(ObjType& obj): obj_(obj)
		{ }

		template<typename Functor>
		void operator() (Functor f)
		{
			f(obj_);
		}

	private:

		CommandObject(const CommandObject&);

	private:

		ObjType		obj_;	
};

class B
{

	public:
		void operator()(A a)
		{
			std::cerr << "voidOneParamFun"  <<  std::endl;
		}
};

int main()
{
	A a;
	B b;

	CommandObject<A> coa(a); 
	//coa.invoke(b);
	//coa.invoke(MEMFUNCTOR(&A::voidOneParamFun, 100));
	coa(MEMFUNCTOR(&A::voidOneParamFun, 100));
}

