/** 
 * @file Functor.h
 * @brief This file implements generic functors.
 * @author Manojkumar M Waghmare
 * @version 1.0
 * @date 
 * @Note: This file implements Functors for 
 * 			functions that take upto 4 parameters. However new functor classes can
 * 			be easily added to handle functions taking more parameters.
 */

#ifndef __FUNCTOR_H__
#define __FUNCTOR_H__

template <class RetType>
class Functor
{
	public:

		virtual RetType operator() () = 0;
};


template <class FunctorType, class RetType = typename FunctorType::ReturnType>
class FunctorImpl : public Functor<RetType>
{
	public:

		FunctorImpl(FunctorType fctr) : fctr_(fctr)
		{
		}

		RetType operator() ()
		{
			return	fctr_();
		}

	private:
		FunctorType									fctr_;
		
};

template <class FunctorType> //FunctorImpl class partial specialization for void functions
class FunctorImpl<FunctorType, void> : public Functor<void>
{
	public:

		FunctorImpl(FunctorType fctr) : fctr_(fctr)
		{
		}

		void operator() ()
		{
			fctr_();
		}

	private:

		FunctorType									fctr_;
		
};

template<class FunctorType>
Functor<typename FunctorType::ReturnType>* GetFunctor(FunctorType f)
{
	return new FunctorImpl<FunctorType, typename FunctorType::ReturnType>(f);
}

/** 
 * @brief Functor for a function that takes no parameter and return nothing.
 * 
 * @tparam PtrFunction: function of type - void(*ptrFunc)()
 */
template<class PtrFunction>
class VoidFunctor0
{
	public:

		typedef void			ReturnType;
		typedef PtrFunction		FunctionType;

		VoidFunctor0(PtrFunction ptrFun) : ptrFun_(ptrFun)
		{
		}

		int operator() ()
		{
			(*ptrFun_)();
			return 0;
		}

	private:

		PtrFunction		ptrFun_;
};

/** 
 * @brief Functor for a function that takes 1 parameter and return nothing.
 * 
 * @tparam PtrFunction: function of type - void (*ptrFunc)(Param1)
 */
template<class PtrFunction, class Param1>
class VoidFunctor1
{
	public:

		typedef void			ReturnType;
		typedef PtrFunction		FunctionType;

		VoidFunctor1(PtrFunction ptrFun, Param1 p1) : 
				ptrFun_(ptrFun), p1_(p1)
		{
		}

		
		int operator() ()
		{
			(*ptrFun_)(p1_);
			return 0;
		}

	private:
		PtrFunction		ptrFun_;
		Param1          p1_;
};

/** 
 * @brief Functor for a function that takes 2 parameter and return nothing.
 * 
 * @tparam PtrFunction: function of type - void (*ptrFunc)(Param1, Param2)
 */
template<class PtrFunction, class Param1, class Param2>
class VoidFunctor2
{
	public:

		typedef void			ReturnType;
		typedef PtrFunction		FunctionType;

		VoidFunctor2(PtrFunction ptrFun, Param1 p1, Param2 p2): 
				ptrFun_(ptrFun), p1_(p1),p2_(p2)
		{
		}

		
		int operator() ()
		{
			(*ptrFun_)(p1_,p2_);
			return 0;
		}

	private:
		PtrFunction		ptrFun_;
		Param1          p1_;
		Param2          p2_;

};

/** 
 * @brief Functor for a function that takes 3 parameter and return nothing.
 * 
 * @tparam PtrFunction: function of type - void (*ptrFunc)(Param1, Param2, Param3)
 */
template<class PtrFunction, class Param1, class Param2, class Param3>
class VoidFunctor3
{
	public:

		typedef void			ReturnType;
		typedef PtrFunction		FunctionType;

		VoidFunctor3(PtrFunction ptrFun, Param1 p1, Param2 p2, Param3 p3):
			   	ptrFun_(ptrFun), p1_(p1),p2_(p2),p3_(p3)
		{
		}

		
		int operator() ()
		{
			(*ptrFun_)(p1_,p2_,p3_);
			return 0;
		}

	private:
		PtrFunction		ptrFun_;
		Param1          p1_;
		Param2          p2_;
		Param3          p3_;

};

/** 
 * @brief Functor for a function that takes 4 parameter and return nothing.
 * 
 * @tparam PtrFunction: function of type - void (*ptrFunc)(Param1, Param2, Param3, Param4)
 */
template<class PtrFunction, class Param1, class Param2, class Param3, class Param4>
class VoidFunctor4
{
	public:

		typedef void			ReturnType;
		typedef PtrFunction		FunctionType;

		VoidFunctor4(PtrFunction ptrFun, Param1 p1, Param2 p2, Param3 p3, Param4 p4) 
				: ptrFun_(ptrFun), p1_(p1), p2_(p2), p3_(p3), p4_(p4)
		{
		}

		int operator() ()
		{
			(*ptrFun_)(p1_,p2_,p3_,p4_);
			return 0;
		}

	private:
		PtrFunction		ptrFun_;
		Param1          p1_;
		Param2          p2_;
		Param3          p3_;
		Param3          p4_;

};

/** 
 * @brief Functor for a function that takes no parameter and returns value of type ReturnType.
 * 
 * @tparam PtrFunction: function of type - ReturnType (*ptrFunc)()
 */
template<class RetType, class PtrFunction>
class RetFunctor0
{
	public:

		typedef RetType			ReturnType;
		typedef PtrFunction		FunctionType;

		RetFunctor0(PtrFunction ptrFun) : ptrFun_(ptrFun)
		{
		}

		
		ReturnType operator() ()
		{
			return (*ptrFun_)();
		}

	private:
		PtrFunction  ptrFun_;
};

/** 
 * @brief Functor for a function that takes 1 parameter and returns value of type ReturnType.
 * 
 * @tparam PtrFunction: function of type - ReturnType (*ptrFunc)(Param1)
 */
template<class RetType, class PtrFunction, class Param1>
class RetFunctor1
{
	public:

		typedef RetType			ReturnType;
		typedef PtrFunction		FunctionType;

		RetFunctor1(PtrFunction ptrFun, Param1 p1): 
				ptrFun_(ptrFun), p1_(p1)
		{
		}

		
		ReturnType operator() ()
		{
			return (*ptrFun_)(p1_);
		}

	private:

		PtrFunction  ptrFun_;
		Param1         p1_;
};

/** 
 * @brief Functor for a function that takes 2 parameter and returns value of type ReturnType.
 * 
 * @tparam PtrFunction: function of type - ReturnType (*ptrFunc)(Param1, Param2)
 */
template<class RetType, class PtrFunction, class Param1, class Param2>
class RetFunctor2
{
	public:

		typedef RetType			ReturnType;
		typedef PtrFunction		FunctionType;

		RetFunctor2(PtrFunction ptrFun, Param1 p1, Param2 p2): 
				ptrFun_(ptrFun), p1_(p1),p2_(p2)
		{
		}

		
		ReturnType operator() ()
		{
			return (*ptrFun_)(p1_,p2_);
		}

	private:
		PtrFunction		ptrFun_;
		Param1          	p1_;
		Param2          	p2_;

};

/** 
 * @brief Functor for a function that takes 3 parameter and returns value of type ReturnType.
 * 
 * @tparam PtrFunction: function of type - ReturnType (*ptrFunc)(Param1, Param2, Param3)
 */
template<class RetType, class PtrFunction, class Param1, class Param2, class Param3>
class RetFunctor3
{
	public:

		typedef RetType			ReturnType;
		typedef PtrFunction		FunctionType;

		RetFunctor3(PtrFunction ptrFun, Param1 p1, Param2 p2, Param3 p3): 
				ptrFun_(ptrFun), p1_(p1),p2_(p2),p3_(p3)
		{
		}

		
		ReturnType operator() ()
		{
			return (*ptrFun_)(p1_,p2_,p3_);
		}

	private:
		PtrFunction  ptrFun_;
		Param1          p1_;
		Param2          p2_;
		Param3          p3_;

};

/** 
 * @brief Functor for a function that takes 4 parameter and returns value of type ReturnType.
 * 
 * @tparam PtrFunction: function of type - ReturnType (*ptrFunc)(Param1,Param2,Param3,Param4)
 */
template<class RetType, class PtrFunction, class Param1, class Param2, class Param3, class Param4>
class RetFunctor4
{
	public:

		typedef RetType			ReturnType;
		typedef PtrFunction		FunctionType;

		RetFunctor4(PtrFunction ptrFun, Param1 p1, Param2 p2, Param3 p3, Param4 p4): 
				ptrFun_(ptrFun), p1_(p1),p2_(p2),p3_(p3),p4_(p4)
		{
		}

		ReturnType operator() ()
		{
			return (*ptrFun_)(p1_,p2_,p3_,p4_);
		}

	private:

		PtrFunction  ptrFun_;
		Param1          p1_;
		Param2          p2_;
		Param3          p3_;
		Param3          p4_;
};


/** 
 * @brief Overloaded Fuctor methods that instantiate VoidFunctor* classes based on
 * 			parameters passed in.
 * 
 * @return VoidFunctor* (where * stands for 0, 1, 2, 3, 4 ...)
 */

template <class PtrFunction>
inline  Functor< void >*
VoidFunctor(PtrFunction ptrFun)
{
	return GetFunctor(VoidFunctor0< PtrFunction > (ptrFun));
}

template < class PtrFunction, class Param1 >
inline  Functor< void >*
VoidFunctor(PtrFunction ptrFun, Param1 p1)
{
	return GetFunctor(VoidFunctor1< PtrFunction, Param1 > (ptrFun,p1));
}

template < class PtrFunction, class Param1, class Param2 >
inline  Functor< void >*
VoidFunctor(PtrFunction ptrFun, Param1 p1, Param2 p2)
{
	return GetFunctor(VoidFunctor2< PtrFunction, Param1, Param2 > (ptrFun,p1,p2));
}

template < class PtrFunction, class Param1, class Param2, class Param3 >
inline  Functor< void >*
VoidFunctor(PtrFunction ptrFun, Param1 p1, Param2 p2, Param3 p3)
{
	return GetFunctor(VoidFunctor3< PtrFunction, Param1, Param2, Param3 > (ptrFun,p1,p2,p3));
}

template < class PtrFunction, class Param1, class Param2, class Param3,class Param4>
inline  Functor< void >*
VoidFunctor(PtrFunction ptrFun, Param1 p1, Param2 p2, Param3 p3, Param4 p4)
{
	return GetFunctor(VoidFunctor4< PtrFunction, Param1, Param2, Param3, Param4 > (ptrFun,p1,p2,p3,p4));
}

/** 
 * @brief Overloaded RetFunctor methods that instantiate RetFunctor* classes based on
 * 			parameters passed in.
 * 
 * @return RetFunctor* 
 */

template <class ReturnType, class PtrFunction>
inline  Functor< ReturnType >*
RetFunctor_(ReturnType ,PtrFunction ptrFun)
{
	return GetFunctor(RetFunctor0<ReturnType, PtrFunction>(ptrFun));
}

template <class ReturnType, class PtrFunction, class Param1 >
inline  Functor< ReturnType >*
RetFunctor_(ReturnType ,PtrFunction ptrFun, Param1 p1)
{
	return GetFunctor(RetFunctor1<ReturnType, PtrFunction, Param1>(ptrFun,p1));
}

template<class ReturnType, class PtrFunction, class Param1, class Param2>
inline  Functor< ReturnType >*
RetFunctor_(ReturnType ,PtrFunction ptrFun, Param1 p1, Param2 p2)
{
	return GetFunctor(RetFunctor2<ReturnType, PtrFunction, Param1, Param2>(ptrFun, p1, p2));
}

template<class ReturnType, class PtrFunction, class Param1, class Param2, class Param3>
inline  Functor< ReturnType >*
RetFunctor_(ReturnType ,PtrFunction ptrFun, Param1 p1, Param2 p2, Param3 p3)
{
	return GetFunctor(RetFunctor3<ReturnType, PtrFunction, Param1, Param2, Param3>(ptrFun, p1, p2, p3));
}

template<class ReturnType, class PtrFunction, class Param1, class Param2, class Param3, class Param4>
inline  Functor< ReturnType >*
RetFunctor_(ReturnType ,PtrFunction ptrFun, Param1 p1, Param2 p2, Param3 p3, Param4 p4)
{
	return GetFunctor(RetFunctor4<ReturnType, PtrFunction, Param1, Param2, Param3, Param4>(ptrFun, p1, p2, p3, p4));
}

#define RetFunctor(RetType, ...)  RetFunctor_(RetType(), __VA_ARGS__)

#endif /* __FUNCTOR_H__ */
