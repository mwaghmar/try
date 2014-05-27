/** 
 * @file MemFunctor.h
 * @brief This file implements generic functors for class member functions.
 * @author Manojkumar M Waghmare
 * @version 1.0
 * @date 
 * @Note: As most of the funtions take 3-4 parameters, this file implements Functors for 
 * 			class member functions that take upto 4 parameters. However new functor classes can
 * 			be easily added to handle functions taking more parameters.
 */

#ifndef __MEMFUNCTOR_H__
#define __MEMFUNCTOR_H__

/** 
 * @brief Functor for a Class member function  that takes no parameter and return nothing.
 * 
 * @tparam PtrMemFunction: Class member function of type - void (class::*)()
 */
template<class PtrMemFunction>
class VoidMemFunctor0
{
	public:

		VoidMemFunctor0(PtrMemFunction ptrMemFun) : ptrMemFun_(ptrMemFun)
		{
		}

		template <class ObjType>
		int operator() (ObjType& obj)
		{
			(obj.*ptrMemFun_)();
			return 0;
		}

	private:

		PtrMemFunction  ptrMemFun_;
};

/** 
 * @brief Functor for a Class member function that takes 1 parameter and return nothing.
 * 
 * @tparam PtrMemFunction: Class member function of type - void (class::*)(Param1)
 */
template<class PtrMemFunction, class Param1>
class VoidMemFunctor1
{
	public:

		VoidMemFunctor1(PtrMemFunction ptrMemFun, Param1 p1) : 
				ptrMemFun_(ptrMemFun), p1_(p1)
		{
		}

		template <class ObjType>
		int operator() (ObjType& obj)
		{
			(obj.*ptrMemFun_)(p1_);
			return 0;
		}

	private:
		PtrMemFunction   ptrMemFun_;
		Param1          p1_;
};

/** 
 * @brief Functor for a Class member function that takes 2 parameter and return nothing.
 * 
 * @tparam PtrMemFunction: Class member function of type - void (class::*)(Param1, Param2)
 */
template<class PtrMemFunction, class Param1, class Param2>
class VoidMemFunctor2
{
	public:

		VoidMemFunctor2(PtrMemFunction ptrMemFun, Param1 p1, Param2 p2): 
				ptrMemFun_(ptrMemFun), p1_(p1),p2_(p2)
		{
		}

		template <class ObjType>
		int operator() (ObjType& obj)
		{
			(obj.*ptrMemFun_)(p1_,p2_);
			return 0;
		}

	private:
		PtrMemFunction  ptrMemFun_;
		Param1          p1_;
		Param2          p2_;

};

/** 
 * @brief Functor for a Class member function that takes 3 parameter and return nothing.
 * 
 * @tparam PtrMemFunction: Class member function of type - void (class::*)(Param1, Param2, Param3)
 */
template<class PtrMemFunction, class Param1, class Param2, class Param3>
class VoidMemFunctor3
{
	public:

		VoidMemFunctor3(PtrMemFunction ptrMemFun, Param1 p1, Param2 p2, Param3 p3):
			   	ptrMemFun_(ptrMemFun), p1_(p1),p2_(p2),p3_(p3)
		{
		}

		template <class ObjType>
		int operator() (ObjType& obj)
		{
			(obj.*ptrMemFun_)(p1_,p2_,p3_);
			return 0;
		}

	private:
		PtrMemFunction   ptrMemFun_;
		Param1          p1_;
		Param2          p2_;
		Param3          p3_;

};

/** 
 * @brief Functor for a Class member function that takes 4 parameter and return nothing.
 * 
 * @tparam PtrMemFunction: Class member function of type - void (class::*)(Param1, Param2, Param3, Param4)
 */
template<class PtrMemFunction, class Param1, class Param2, class Param3, class Param4>
class VoidMemFunctor4
{
	public:

		VoidMemFunctor4(PtrMemFunction ptrMemFun, Param1 p1, Param2 p2, Param3 p3, Param4 p4) 
				: ptrMemFun_(ptrMemFun), p1_(p1), p2_(p2), p3_(p3), p4_(p4)
		{
		}

		template <class ObjType>
		int operator() (ObjType& obj)
		{
			(obj.*ptrMemFun_)(p1_,p2_,p3_,p4_);
			return 0;
		}

	private:
		PtrMemFunction   ptrMemFun_;
		Param1          p1_;
		Param2          p2_;
		Param3          p3_;
		Param3          p4_;

};

/** 
 * @brief Functor for a Class member function that takes no parameter and returns value of type ReturnType.
 * 
 * @tparam PtrMemFunction: Class member function of type - ReturnType (class::*)()
 */
template<class ReturnType, class PtrMemFunction>
class MemFunctor0
{
	public:

		MemFunctor0(PtrMemFunction ptrMemFun) : ptrMemFun_(ptrMemFun)
		{
		}

		template <class ObjType>
		ReturnType operator() (ObjType& obj)
		{
			return (obj.*ptrMemFun_)();
		}

	private:
		PtrMemFunction  ptrMemFun_;
};

/** 
 * @brief Functor for a Class member function that takes 1 parameter and returns value of type ReturnType.
 * 
 * @tparam PtrMemFunction: Class member function of type - ReturnType (class::*)(Param1)
 */
template<class ReturnType, class PtrMemFunction, class Param1>
class MemFunctor1
{
	public:

		MemFunctor1(PtrMemFunction ptrMemFun, Param1 p1): 
				ptrMemFun_(ptrMemFun), p1_(p1)
		{
		}

		template <class ObjType>
		ReturnType operator() (ObjType& obj)
		{
			return (obj.*ptrMemFun_)(p1_);
		}

	private:

		PtrMemFunction  ptrMemFun_;
		Param1         p1_;
};

/** 
 * @brief Functor for a Class member function that takes 2 parameter and returns value of type ReturnType.
 * 
 * @tparam PtrMemFunction: Class member function of type - ReturnType (class::*)(Param1, Param2)
 */
template<class ReturnType, class PtrMemFunction, class Param1, class Param2>
class MemFunctor2
{
	public:

		MemFunctor2(PtrMemFunction ptrMemFun, Param1 p1, Param2 p2): 
				ptrMemFun_(ptrMemFun), p1_(p1),p2_(p2)
		{
		}

		template <class ObjType>
		ReturnType operator() (ObjType& obj)
		{
			return (obj.*ptrMemFun_)(p1_,p2_);
		}

	private:
		PtrMemFunction		ptrMemFun_;
		Param1          	p1_;
		Param2          	p2_;

};

/** 
 * @brief Functor for a Class member function that takes 3 parameter and returns value of type ReturnType.
 * 
 * @tparam PtrMemFunction: Class member function of type - ReturnType (class::*)(Param1, Param2, Param3)
 */
template<class ReturnType, class PtrMemFunction, class Param1, class Param2, class Param3>
class MemFunctor3
{
	public:

		MemFunctor3(PtrMemFunction ptrMemFun, Param1 p1, Param2 p2, Param3 p3): 
				ptrMemFun_(ptrMemFun), p1_(p1),p2_(p2),p3_(p3)
		{
		}

		template <class ObjType>
		ReturnType operator() (ObjType& obj)
		{
			return (obj.*ptrMemFun_)(p1_,p2_,p3_);
		}

	private:
		PtrMemFunction  ptrMemFun_;
		Param1          p1_;
		Param2          p2_;
		Param3          p3_;

};

/** 
 * @brief Functor for a Class member function that takes 4 parameter and returns value of type ReturnType.
 * 
 * @tparam PtrMemFunction: Class member function of type - ReturnType (class::*)(Param1,Param2,Param3,Param4)
 */
template<class ReturnType, class PtrMemFunction, class Param1, class Param2, class Param3, class Param4>
class MemFunctor4
{
	public:

		MemFunctor4(PtrMemFunction ptrMemFun, Param1 p1, Param2 p2, Param3 p3, Param4 p4): 
				ptrMemFun_(ptrMemFun), p1_(p1),p2_(p2),p3_(p3),p4_(p4)
		{
		}

		template <class ObjType>
		ReturnType operator() (ObjType& obj)
		{
			return (obj.*ptrMemFun_)(p1_,p2_,p3_,p4_);
		}

	private:
		PtrMemFunction  ptrMemFun_;
		Param1          p1_;
		Param2          p2_;
		Param3          p3_;
		Param3          p4_;
};


/** 
 * @brief Overloaded Memfuctor methods that instantiate VoidMemFunctor* functors based on
 * 			parameters passed in.
 * 
 * @return VoidMemFunctor* 
 */

template <class PtrMemFunction>
inline  VoidMemFunctor0< PtrMemFunction > MemFunctor(PtrMemFunction ptrMemFun)
{
	return VoidMemFunctor0< PtrMemFunction > (ptrMemFun);
}

template < class PtrMemFunction, class Param1 >
inline  VoidMemFunctor1< PtrMemFunction, Param1> MemFunctor(PtrMemFunction ptrMemFun, Param1 p1)
{
	return VoidMemFunctor1< PtrMemFunction, Param1 > (ptrMemFun,p1);
}

template < class PtrMemFunction, class Param1, class Param2 >
inline  VoidMemFunctor2< PtrMemFunction, Param1, Param2> MemFunctor(PtrMemFunction ptrMemFun, Param1 p1, Param2 p2)
{
	return VoidMemFunctor2< PtrMemFunction, Param1, Param2 > (ptrMemFun,p1,p2);
}

template < class PtrMemFunction, class Param1, class Param2, class Param3 >
inline  VoidMemFunctor3< PtrMemFunction, Param1, Param2, Param3 > MemFunctor(PtrMemFunction ptrMemFun, Param1 p1, Param2 p2, Param3 p3)
{
	return VoidMemFunctor3< PtrMemFunction, Param1, Param2, Param3 > (ptrMemFun,p1,p2,p3);
}

template < class PtrMemFunction, class Param1, class Param2, class Param3,class Param4>
inline  VoidMemFunctor4< PtrMemFunction, Param1, Param2, Param3, Param4 > MemFunctor(PtrMemFunction ptrMemFun, Param1 p1, Param2 p2, Param3 p3, Param4 p4)
{
	return VoidMemFunctor4< PtrMemFunction, Param1, Param2, Param3, Param4 > (ptrMemFun,p1,p2,p3,p4);
}

/** 
 * @brief Overloaded Memfuctor methods that instantiate MemFunctor* functors based on
 * 			parameters passed in.
 * 
 * @return MemFunctor* 
 */

template <class ReturnType, class PtrMemFunction>
inline  MemFunctor0<ReturnType, PtrMemFunction > RetMemFunctor(ReturnType ,PtrMemFunction ptrMemFun)
{
	return MemFunctor0<ReturnType, PtrMemFunction>(ptrMemFun);
}

template <class ReturnType, class PtrMemFunction, class Param1 >
inline  MemFunctor1<ReturnType, PtrMemFunction, Param1> RetMemFunctor(ReturnType ,PtrMemFunction ptrMemFun, Param1 p1)
{
	return MemFunctor1<ReturnType, PtrMemFunction, Param1>(ptrMemFun,p1);
}

template<class ReturnType, class PtrMemFunction, class Param1, class Param2>
inline  MemFunctor2<ReturnType, PtrMemFunction, Param1, Param2> RetMemFunctor(ReturnType ,PtrMemFunction ptrMemFun, Param1 p1, Param2 p2)
{
	return MemFunctor2<ReturnType, PtrMemFunction, Param1, Param2>(ptrMemFun, p1, p2);
}

template<class ReturnType, class PtrMemFunction, class Param1, class Param2, class Param3>
inline MemFunctor3<ReturnType, PtrMemFunction, Param1, Param2, Param3> RetMemFunctor(ReturnType ,PtrMemFunction ptrMemFun, Param1 p1, Param2 p2, Param3 p3)
{
	return MemFunctor3<ReturnType, PtrMemFunction, Param1, Param2, Param3>(ptrMemFun, p1, p2, p3);
}

template<class ReturnType, class PtrMemFunction, class Param1, class Param2, class Param3, class Param4>
inline MemFunctor4<ReturnType, PtrMemFunction, Param1, Param2, Param3, Param4> RetMemFunctor(ReturnType ,PtrMemFunction ptrMemFun, Param1 p1, Param2 p2, Param3 p3, Param4 p4)
{
	return MemFunctor4<ReturnType, PtrMemFunction, Param1, Param2, Param3, Param4>(ptrMemFun, p1, p2, p3, p4);
}

#endif /* __MEMFUNCTOR_H__ */
