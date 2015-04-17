#ifndef VTL_SET_HPP
#define VTL_SET_HPP

#include "list.hpp"

namespace vtl{

template<class L,
         template<class,class> class P=std::is_same>
struct Distinct_t{
    using bits = List<>;
    using idx = List<>;
    using type = List<>;
};


template<template<class,class> class P,
         template<class...>class L,
         class...l>
struct Distinct_t<L<l...>,P>{
    using bits = List<Z<(Count<IndexOf<l, L<l...>,P>>()<2)>...>;
    using idx = Bits2Idx<bits>;
    using type = Copy< L<l...> , idx >  ;
};


template<class L,class S,template<class,class> class P=std::is_same>
using NotInIdx = Bits2Idx< Map<FindBits<Decay<L>,Decay<S>,P>,Not>>;

template<class L,class S,template<class,class> class P=std::is_same>
using NotIn    = Copy<S, NotInIdx<L,S,P> >;


template<class L,template<class,class> class P=std::is_same>
using Distinct = typename Distinct_t< Decay<L> , P >::type;

template<class L,template<class,class> class P=std::is_same>
using DistinctIdx = typename Distinct_t< Decay<L>, P >::idx;

template<class L,template<class,class>class P=std::is_same>
using UniqueIdx= typename Unique_t<Decay<L>,P>::idx;

template<class L,template<class,class>class P=std::is_same>
using UniqueBits= typename Unique_t<Decay<L>,P>::bits;

template<class L,template<class,class>class P=std::is_same>
using Unique= typename Unique_t<Decay<L>,P>::type;

template<class L,class Set,template<class,class> class P=std::is_same>
using Complement = NotIn<L,Set,P>;

template<class L,class Set,template<class,class> class P=std::is_same>
using ComplementIdx = NotInIdx<L,Set,P>;

template<class L,class R,template<class,class> class P=std::is_same>
using Union = Unique< Concat<L,R> , P >;

template<class L,class R,template<class,class> class P=std::is_same>
using UnionIdx = UniqueIdx< Concat<L,R>, P >;

template<class L,class R,template<class,class> class P=std::is_same>
using Meet = Find<L,R,P>;

template<class L,class R,template<class,class> class P=std::is_same>
using MeetIdx = FindIdx<L,R,P>;

template<class L,class R,template<class,class> class P=std::is_same>
using MeetBits = FindBits<L,R,P>;

template<class L,class R,template<class,class> class P=std::is_same>
using Diff = NotIn<R,L,P>;

template<class L,class R,template<class,class> class P=std::is_same>
using DiffIdx = NotInIdx<R,L,P>;

template<class L,class R,template<class,class> class P=std::is_same>
using SymDiff = Complement< Meet<L,R,P>  , Union<L,R,P> ,P>;

template<class L,class R,template<class,class> class P=std::is_same>
using SymDiffIdx = ComplementIdx< Meet<L,R,P>  , Union<L,R,P> ,P>;

template<class L,class R>
constexpr auto diff(L,R){
  return Diff<L,R>();
}

}

#endif
