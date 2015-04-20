#ifndef VTL_LIST_HPP
#define VTL_LIST_HPP

#include "utils.hpp"
#include "macro.hpp"
#include "sym.hpp"

namespace vtl{

template<class...X> struct List{
    using type = List<X...>;
    constexpr List(...){}
};



template<uint i,class...T>
struct iterator : Z<0>{
    using type = iterator<i>;
};

template<uint i,class T0,class...T>
struct iterator<i,T0,T...> {
    using type = typename iterator<i-1,T...>::type;
};

template<class T0,class...T>
struct iterator<0,T0,T...>{
    using type = T0;
};


template<uint i>
struct ListExtractor : Z<0>{

    template<class L>
    struct get {
        using type = ListExtractor<i>;
    };

    template<template<class...>class L>
    struct get<L<>> {
        using type = ListExtractor<i>;
    };

    template<template<class...>class L,class...T>
    struct get<L<T...>> : iterator<i,T...> {};


    template<class L>
    using type = typename get<Decay<L>>::type;

    template<class L>
    constexpr auto operator()(L){
        return type<L>();
    }

};


template<class L,uint i>
using Get = typename ListExtractor<i>::template type<L>;

template<uint i,class L>
constexpr static auto get(L){ return Get<L,i>(); }


template<class L,class F, template<class...>class R, class...I>
constexpr auto listFold(L,F f,R<I...>){
    return f( get(I(),L())... );
}



template<int...I>
using NumList = List<Z<I>...>;

template<int b,int e,int...x>
struct Range_t : Range_t<b+((b<e)?(1):(-1)),e,x...,b>
{};


template<int e,int...x>
struct Range_t<e,e,x...> {
  using type=List<Z<x>...>;
};

template<int b,int e>
using Range = typename Range_t<b,e>::type;

template<class B,class E>
constexpr auto range(B b,E e) {
  return Range<(int)b,(int)e>();
}

template<class E>
constexpr auto range(E e) {
  return Range<0,(int)e>();
}

template<class L>
struct Count_t {
    using type = Z<0>;
};

template<template<class...>class T,class...x>
struct Count_t<T<x...>> {
    using type = Z<sizeof...(x)>;
};



template<class L>
using Count = typename Count_t<Decay<L>>::type;

template<class L>
constexpr auto count(L){ return Count<L>();}

template<class L>
constexpr auto rangeOf(L l){
  return range(count(l));
}


template<
  class L,
  class idx, 
  template<class...>class Dest=List >
struct Copy_t{
    using type = Dest<>;
};

template<
  template<class...> class L,
  template<class...> class R,
  template<class...> class D,
  class...I,
  class l0,
  class...l>
struct Copy_t<L<l0,l...>,R<I...>,D>{
    using type = D< 
      Get<L<l0,l...>,
        I::value%Count<L<l0,l...>>()
      >... 
    >;
};



template<class L,class I, template<class...>class Dest=List>
using Copy = typename Copy_t<Decay<L>, Decay<I>,Dest> ::type;

template<class L,class I>
constexpr auto copy(L,I){
  return Copy<L,I>();
}


template<
  template<class...> class L,
  template<class...> class LI, 
  class...T,
  class...i>
constexpr auto remove( L<T...> List, LI<i...> removedIdx){
  return copy(List, diff( rangeOf(List), removedIdx) );
}




template<class...L>
struct Concat_t{
    using type = List<>;
};

template<class L>
struct Concat_t<L>{
    using type = L;
};

template<template<class...>class L,template<class...>class R, class...l,class...r>
struct Concat_t<L<l...>,R<r...>>{
    using type = L<l...,r...>;
};


template<class L0,class...L>
struct Concat_t<L0,L...> : Concat_t<L0,typename Concat_t<L...>::type>::type
{};


template<class...L>
using Concat = typename Concat_t<Decay<L>...>::type;

template<class...L>
constexpr auto listConcat(L...){
    return Concat<L...>();
}


template<class L, class E>
struct Append_t{
    using type = L;
};

template<template<class...>class L, class E,class...X>
struct Append_t<L<X...>,E>{
    using type = L<X...,E>;
};

template<class L, class E>
struct Prepend_t{
    using type = L;
};

template<template<class...>class L, class E,class...X>
struct Prepend_t<L<X...>,E>{
    using type = L<E,X...>;
};



template<class L,class E>
using Append = typename Append_t<Decay<L>,E>::type;

template<class L,class E>
using Prepend = typename Prepend_t<Decay<L>,E>::type;


template<class L,class E, template<class,class> class Extender, bool append=false>
struct ExtendIF_t {
    using type = L;
};

template<class L,class E,template<class,class> class Extender>
struct ExtendIF_t<L,E,Extender,true> {
    using type = Extender<L,E>;
};




template<class L,class E,template<class,class> class Extender,bool cond>
using ExtendIF = typename ExtendIF_t<Decay<L>,E,Extender,cond>::type;

template<class L,class E,bool cond>
using AppendIF = ExtendIF<L,E,Append,cond>;


template<class L,class E,bool cond>
using PrependIF = ExtendIF<L,E,Prepend,cond>;





template<class C, class Result=List<>, uint I = ((uint)count(C())?(uint)count(C()):1) -1>
struct Bits2Idx_t : Bits2Idx_t<C,PrependIF<Result,Z<I>,!!Get<C,I>()>,I-1>{};



template<class C,class Result>
struct Bits2Idx_t<C,Result,0> {
    using type = PrependIF<Result,Z<0>,!!Get<C,0>()>;
};


template<class L>
using Bits2Idx = typename Bits2Idx_t<Decay<L>>::type;

template<class L>
constexpr auto bits2Idx(L){
    return Bits2Idx<L>();
}



template<class T,
         class L,
         template<class,class> class P=std::is_same>
struct IndexOf_t{
    using bits = List<>;
    using type = List<>;
};

template<class T,
         template<class,class> class P,
         template<class...>class L,
         class...l>
struct IndexOf_t<T,L<l...>,P>{
    using bits = List<Z<P<T,l>::value>...>;
    using type = Bits2Idx<bits>;
};

template<class T,class L, template<class,class> class P=std::is_same>
using IndexOf = typename IndexOf_t<T,Decay<L>,P>::type;

template<template<class,class> class P=std::is_same, class L,class R>
constexpr auto indexOf(L,R){
  return IndexOf<L,R,P>();
} 

template<
  template<class,class> class P=std::is_same, 
  template<class...>class ListT,
class R,class...L>
constexpr auto indexOfEach(ListT<L...>,R){
  return Concat<IndexOf<L,R,P>...>();
} 

template<class L,
         class R,
         template<class,class> class P=std::is_same>
struct Find_t{
    using bits = List<>;
    using idx = List<>;
    using type = List<>;
};


template<template<class,class> class P,
         template<class...>class L,
         template<class...>class R,
         class...l,
         class...r>
struct Find_t<L<l...>,R<r...>,P>{
    using bits = List<Count< IndexOf< r, L<l...>,P >>...>;
    using idx = Bits2Idx<bits>;
    using type = Copy< R<r...> , idx >;
};

template<class L,class F>
struct Map_t{
  using type = List<>;
};


template<class F,template<class...>class L,class...l>
struct Map_t<L<l...>,F>{
  using type = List<decltype(F()(l()))...>;
};

struct Not{
  template<class T>
  constexpr auto operator()(T I){
    return !I;
  }
};

template<class L,class F>
using Map = typename Map_t<Decay<L>,F>::type;

template<class L,class R, template<class,class> class P=std::is_same>
using FindBits= typename Find_t<Decay<L>,Decay<R>,P>::bits;

template<class L,class R, template<class,class> class P=std::is_same>
using FindIdx= typename Find_t<Decay<L>,Decay<R>,P>::idx;

template<class L,class R, template<class,class> class P=std::is_same>
using Find= typename Find_t<Decay<L>,Decay<R>,P>::type;

template<template<class,class> class P=std::is_same, class L,class R>
constexpr auto find(L,R){
  return FindIdx<L,R,P>();
} 

template<class L,class R>
struct Zip_t{
    using type=List<>;
};

template<template<class...>class L,template<class...>class R,class...l,class...r>
struct Zip_t<L<l...>,R<r...>>{
    using type=List<List<l,r>...>;
};

template<class L,class R>
using Zip= typename Zip_t<Decay<L> , Decay<R> >::type;

template<class L>
using Enumerate = Zip< Range<0 , Count<L>::value >, Decay<L>>;

template<
  class U,
  template<class,class>class P=std::is_same>
struct Unique_t{

    template<class L,class B=List<>,class D=List<>>
    struct find{
      using type = B;
    };

    template< 
      template<class...>class L, 
      template<class...>class B, 
      template<class...>class D,
      class l0,class...l,
      class...b,
      class...d>
    struct find< L<l0,l...>, B<b...>, D<d...> >
      : find< L<l...>, 
              B<b..., N<!BinFold(+)(P<l0,d>::value...)> >,
              D<d...,l0>>
    {};


    template< 
      template<class...>class L, 
      template<class...>class B, 
      template<class...>class D,
      class l0,class...l,
      class...d>
    struct find< L<l0,l...>, B<>, D<d...> >
      : find< L<l...>, 
              B<N<1>>,
              D<d...,l0>>
    {};

    using bits = typename find< Decay<U> >::type;
    using idx = Bits2Idx<bits>;
    using type = Copy< Decay<U> , idx >  ;   

};


}

#endif
