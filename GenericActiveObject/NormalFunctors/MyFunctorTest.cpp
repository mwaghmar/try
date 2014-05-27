#include <iostream>
#include "Functor.h"

void voidfun0()
{
	std::cerr << "voidfun0" << std::endl;
}

void voidfun1(int p1)
{
	std::cerr << "voidfun1 p1: " << p1 << std::endl;
}


int fun0()
{
	std::cerr << "fun0" << std::endl;
	return 1;
}

int fun1(int p1)
{
	std::cerr << "fun1 p1: " << p1 << std::endl;
	return p1+1;
}

int main()
{
	Functor<void>* f = GetFunctor(VoidFunctor(voidfun0));
	//fv0();	

	return 0;
}
