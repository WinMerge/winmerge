//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2012-2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////

//! \file

#ifndef BOOST_MOVE_DETAIL_META_UTILS_HPP
#define BOOST_MOVE_DETAIL_META_UTILS_HPP

#include <boost/move/detail/config_begin.hpp>
#include <cstddef>   //for std::size_t

//Small meta-typetraits to support move

namespace boost {

//Forward declare boost::rv
template <class T> class rv;

namespace move_detail {

//////////////////////////////////////
//              nat
//////////////////////////////////////
struct nat{};

//////////////////////////////////////
//            natify
//////////////////////////////////////
template <class T> struct natify{};

//////////////////////////////////////
//             if_c
//////////////////////////////////////
template<bool C, typename T1, typename T2>
struct if_c
{
   typedef T1 type;
};

template<typename T1, typename T2>
struct if_c<false,T1,T2>
{
   typedef T2 type;
};

//////////////////////////////////////
//             if_
//////////////////////////////////////
template<typename T1, typename T2, typename T3>
struct if_
{
   typedef typename if_c<0 != T1::value, T2, T3>::type type;
};

//enable_if_
template <bool B, class T = nat>
struct enable_if_c
{
   typedef T type;
};

//////////////////////////////////////
//          enable_if_c
//////////////////////////////////////
template <class T>
struct enable_if_c<false, T> {};

//////////////////////////////////////
//           enable_if
//////////////////////////////////////
template <class Cond, class T = nat>
struct enable_if : public enable_if_c<Cond::value, T> {};

//////////////////////////////////////
//          disable_if
//////////////////////////////////////
template <class Cond, class T = nat>
struct disable_if : public enable_if_c<!Cond::value, T> {};

//////////////////////////////////////
//          integral_constant
//////////////////////////////////////
template<class T, T v>
struct integral_constant
{
   static const T value = v;
   typedef T value_type;
   typedef integral_constant<T, v> type;
};

typedef integral_constant<bool, true >  true_type;
typedef integral_constant<bool, false > false_type;

//////////////////////////////////////
//             identity
//////////////////////////////////////
template <class T>
struct identity
{
   typedef T type;
};

//////////////////////////////////////
//          remove_reference
//////////////////////////////////////
template<class T>
struct remove_reference
{
   typedef T type;
};

template<class T>
struct remove_reference<T&>
{
   typedef T type;
};

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template<class T>
struct remove_reference<T&&>
{
   typedef T type;
};

#else

template<class T>
struct remove_reference< rv<T> >
{
   typedef T type;
};

template<class T>
struct remove_reference< rv<T> &>
{
   typedef T type;
};

template<class T>
struct remove_reference< const rv<T> &>
{
   typedef T type;
};


#endif

//////////////////////////////////////
//             add_const
//////////////////////////////////////
template<class T>
struct add_const
{
   typedef const T type;
};

template<class T>
struct add_const<T&>
{
   typedef const T& type;
};

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template<class T>
struct add_const<T&&>
{
   typedef T&& type;
};

#endif

//////////////////////////////////////
//      add_lvalue_reference
//////////////////////////////////////
template<class T>
struct add_lvalue_reference
{
   typedef T& type;
};

template<class T>
struct add_lvalue_reference<T&>
{
   typedef T& type;
};

template<>
struct add_lvalue_reference<void>
{
   typedef void type;
};

template<>
struct add_lvalue_reference<const void>
{
   typedef const void type;
};

template<>
struct add_lvalue_reference<volatile void>
{
   typedef volatile void type;
};

template<>
struct add_lvalue_reference<const volatile void>
{
   typedef const volatile void type;
};

template<class T>
struct add_const_lvalue_reference
{
   typedef typename remove_reference<T>::type         t_unreferenced;
   typedef typename add_const<t_unreferenced>::type   t_unreferenced_const;
   typedef typename add_lvalue_reference
      <t_unreferenced_const>::type                    type;
};


//////////////////////////////////////
//             is_same
//////////////////////////////////////
template<class T, class U>
struct is_same
{
   static const bool value = false;
};
 
template<class T>
struct is_same<T, T>
{
   static const bool value = true;
};

//////////////////////////////////////
//             is_lvalue_reference
//////////////////////////////////////
template<class T>
struct is_lvalue_reference
{
    static const bool value = false;
};

template<class T>
struct is_lvalue_reference<T&>
{
    static const bool value = true;
};

//////////////////////////////////////
//          is_class_or_union
//////////////////////////////////////
template<class T>
struct is_class_or_union
{
   struct twochar { char _[2]; };
   template <class U>
   static char is_class_or_union_tester(void(U::*)(void));
   template <class U>
   static twochar is_class_or_union_tester(...);
   static const bool value = sizeof(is_class_or_union_tester<T>(0)) == sizeof(char);
};

//////////////////////////////////////
//             addressof
//////////////////////////////////////
template<class T>
struct addr_impl_ref
{
   T & v_;
   inline addr_impl_ref( T & v ): v_( v ) {}
   inline operator T& () const { return v_; }

   private:
   addr_impl_ref & operator=(const addr_impl_ref &);
};

template<class T>
struct addressof_impl
{
   static inline T * f( T & v, long )
   {
      return reinterpret_cast<T*>(
         &const_cast<char&>(reinterpret_cast<const volatile char &>(v)));
   }

   static inline T * f( T * v, int )
   {  return v;  }
};

template<class T>
inline T * addressof( T & v )
{
   return ::boost::move_detail::addressof_impl<T>::f
      ( ::boost::move_detail::addr_impl_ref<T>( v ), 0 );
}

//////////////////////////////////////
//          has_pointer_type
//////////////////////////////////////
template <class T>
struct has_pointer_type
{
   struct two { char c[2]; };
   template <class U> static two test(...);
   template <class U> static char test(typename U::pointer* = 0);
   static const bool value = sizeof(test<T>(0)) == 1;
};

//////////////////////////////////////
//           is_convertible
//////////////////////////////////////
#if defined(_MSC_VER) && (_MSC_VER >= 1400)

//use intrinsic since in MSVC
//overaligned types can't go through ellipsis
template <class T, class U>
struct is_convertible
{
   static const bool value = __is_convertible_to(T, U);
};

#else

template <class T, class U>
class is_convertible
{
   typedef typename add_lvalue_reference<T>::type t_reference;
   typedef char true_t;
   class false_t { char dummy[2]; };
   static false_t dispatch(...);
   static true_t  dispatch(U);
   static t_reference       trigger();
   public:
   static const bool value = sizeof(dispatch(trigger())) == sizeof(true_t);
};

#endif

//////////////////////////////////////////////////////////////////////////////
//
//                               has_move_emulation_enabled_impl
//
//////////////////////////////////////////////////////////////////////////////
template<class T>
struct has_move_emulation_enabled_impl
   : is_convertible< T, ::boost::rv<T>& >
{};

template<class T>
struct has_move_emulation_enabled_impl<T&>
{  static const bool value = false;  };

template<class T>
struct has_move_emulation_enabled_impl< ::boost::rv<T> >
{  static const bool value = false;  };

//////////////////////////////////////////////////////////////////////////////
//
//                            is_rv_impl
//
//////////////////////////////////////////////////////////////////////////////

template <class T>
struct is_rv_impl
{  static const bool value = false;  };

template <class T>
struct is_rv_impl< rv<T> >
{  static const bool value = true;  };

template <class T>
struct is_rv_impl< const rv<T> >
{  static const bool value = true;  };

// Code from Jeffrey Lee Hellrung, many thanks

template< class T >
struct is_rvalue_reference
{  static const bool value = false;  };

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template< class T >
struct is_rvalue_reference< T&& >
{  static const bool value = true;  };

#else // #ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template< class T >
struct is_rvalue_reference< boost::rv<T>& >
{  static const bool value = true;  };

template< class T >
struct is_rvalue_reference< const boost::rv<T>& >
{  static const bool value = true;  };

#endif // #ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template< class T >
struct add_rvalue_reference
{ typedef T&& type; };

#else // #ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

namespace detail_add_rvalue_reference
{
   template< class T
            , bool emulation = has_move_emulation_enabled_impl<T>::value
            , bool rv        = is_rv_impl<T>::value  >
   struct add_rvalue_reference_impl { typedef T type; };

   template< class T, bool emulation>
   struct add_rvalue_reference_impl< T, emulation, true > { typedef T & type; };

   template< class T, bool rv >
   struct add_rvalue_reference_impl< T, true, rv > { typedef ::boost::rv<T>& type; };
} // namespace detail_add_rvalue_reference

template< class T >
struct add_rvalue_reference
   : detail_add_rvalue_reference::add_rvalue_reference_impl<T>
{ };

template< class T >
struct add_rvalue_reference<T &>
{  typedef T & type; };

#endif // #ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

template< class T > struct remove_rvalue_reference { typedef T type; };

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   template< class T > struct remove_rvalue_reference< T&& >                  { typedef T type; };
#else // #ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
   template< class T > struct remove_rvalue_reference< rv<T> >                { typedef T type; };
   template< class T > struct remove_rvalue_reference< const rv<T> >          { typedef T type; };
   template< class T > struct remove_rvalue_reference< volatile rv<T> >       { typedef T type; };
   template< class T > struct remove_rvalue_reference< const volatile rv<T> > { typedef T type; };
   template< class T > struct remove_rvalue_reference< rv<T>& >               { typedef T type; };
   template< class T > struct remove_rvalue_reference< const rv<T>& >         { typedef T type; };
   template< class T > struct remove_rvalue_reference< volatile rv<T>& >      { typedef T type; };
   template< class T > struct remove_rvalue_reference< const volatile rv<T>& >{ typedef T type; };
#endif // #ifndef BOOST_NO_CXX11_RVALUE_REFERENCES

// Ideas from Boost.Move review, Jeffrey Lee Hellrung:
//
//- TypeTraits metafunctions is_lvalue_reference, add_lvalue_reference, and remove_lvalue_reference ?
//  Perhaps add_reference and remove_reference can be modified so that they behave wrt emulated rvalue
//  references the same as wrt real rvalue references, i.e., add_reference< rv<T>& > -> T& rather than
//  rv<T>& (since T&& & -> T&).
//
//- Add'l TypeTraits has_[trivial_]move_{constructor,assign}...?
//
//- An as_lvalue(T& x) function, which amounts to an identity operation in C++0x, but strips emulated
//  rvalue references in C++03.  This may be necessary to prevent "accidental moves".

}  //namespace move_detail {
}  //namespace boost {

#include <boost/move/detail/config_end.hpp>

#endif //#ifndef BOOST_MOVE_DETAIL_META_UTILS_HPP
