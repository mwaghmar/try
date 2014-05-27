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

void voidfun2(int p1, int p2)
{
	std::cerr << "voidfun2 p1: " << p1 << " p2: " << p2 << std::endl;
}

void voidfun3(int p1, int p2, int p3)
{
	std::cerr << "voidfun3 p1: " << p1 << " p2: " << p2 << " p3: " << p3 << std::endl;
}

void voidfun4(int p1, int p2, int p3, int p4)
{
	std::cerr << "voidfun4 p1: " << p1 << " p2: " << p2 << " p3: " << p3 << " p4: " << p4 << std::endl;
}

int fun0()
{
	std::cerr << "fun0()" << std::endl;
	return 0;
}

int fun1(int p1)
{
	std::cerr << "fun1 p1: " << p1 << std::endl;
	return p1+1;
}

int fun2(int p1, int p2)
{
	std::cerr << "fun2 p1: " << p1 << " p2: " << p2 << std::endl;
	return p1+p2+1;
}

int fun3(int p1, int p2, int p3)
{
	std::cerr << "fun3 p1: " << p1 << " p2: " << p2 << " p3: " << p3 << std::endl;
	return p1+p2+p3+1;
}

int fun4(int p1, int p2, int p3, int p4)
{
	std::cerr << "fun4 p1: " << p1 << " p2: " << p2 << " p3: " << p3 << " p4: " << p4 << std::endl;
	return p1+p2+p3+p4+1;
}

int main()
{
	Functor<void>* vf0 = VoidFunctor(voidfun0);
	vf0->operator()();	

	Functor<void>* vf1 = VoidFunctor(voidfun1, 100);
	vf1->operator()();	

	Functor<void>* vf2 = VoidFunctor(voidfun2, 100, 200);
	vf2->operator()();	

	Functor<void>* vf3 = VoidFunctor(voidfun3, 100, 200, 300);
	vf3->operator()();	

	Functor<void>* vf4 = VoidFunctor(voidfun4, 100, 200, 300, 400);
	vf4->operator()();	

	delete vf0; delete vf1; delete vf2; delete vf3; delete vf4;

	Functor<int>* if0 = RetFunctor(int, fun0);
	int ret = if0->operator()();	
	std::cerr << "fun0 returns: " << ret << std::endl;

	Functor<int>* if1 = RetFunctor(int, fun1, 100);
	ret = if1->operator()();	
	std::cerr << "fun1 returns: " << ret << std::endl;

	Functor<int>* if2 = RetFunctor(int, fun2, 100, 200);
	ret = if2->operator()();	
	std::cerr << "fun2 returns: " << ret << std::endl;

	Functor<int>* if3 = RetFunctor(int, fun3, 100, 200, 300);
	ret = if3->operator()();	
	std::cerr << "fun3 returns: " << ret << std::endl;

	Functor<int>* if4 = RetFunctor(int, fun4, 100, 200, 300, 400);
	ret = if4->operator()();	
	std::cerr << "fun4 returns: " << ret << std::endl;

	delete if0; delete if1; delete if2; delete if3; delete if4;

	return 0;
}
