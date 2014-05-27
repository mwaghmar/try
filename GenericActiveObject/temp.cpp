#include<iostream>

#define RETMEMFUNCTOR(RetType, PtrMemFunction, ...)		TimePass<RetType>(PtrMemFunction, __VA_ARGS__)

template<class RetType>
int TimePass(void* ptrToFun, ...)
{
	std::cerr << "Timepass: " << ptrToFun << std::endl;
	return 1;
}

int main()
{
	int a = RETMEMFUNCTOR(int, NULL, 2, 3);

	std::cerr << "a: " << a << std::endl;

	return 0;
}

