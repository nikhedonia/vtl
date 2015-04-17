#ifndef VTL_SYM_HPP
#define VTL_SYM_HPP

#include "utils.hpp"
#include "macro.hpp"

namespace vtl {

template<class T, T v>
struct Sym{
  constexpr static T value=v;
  constexpr Sym(){}
  constexpr Sym(Sym const&){}

  constexpr T operator()(...)const{ return value;}
  constexpr operator T()const{ return value; }

  constexpr auto operator!()const{ return Sym<decltype(!v),!v>(); }
  constexpr auto operator+()const{ return Sym<decltype(+v),v>(); }
  constexpr auto operator-()const{ return Sym<decltype(-v),v>(); }
};

template<class T, T v>
constexpr T Sym<T,v>::value;




#define CREATE_SYM_OP(OP)\
template<class L, L l, class R, R r>\
constexpr auto operator OP( Sym<L,l>, Sym<R,r> ){ return Sym<decltype((l OP r)),(l OP r)>(); }

CREATE_ALL_OPERATORS(CREATE_SYM_OP)

template<int v>
using Z = Sym<int,v>;

template<uint v>
using N = Sym<uint,v>;

template<char...X> constexpr auto operator "" _Z(){
    return Z<C2N( (X-'0')... )>();
}

template<char...X> constexpr auto operator "" _N(){
    return N<C2N( (X-'0')... )>();
}



template<int...I>
struct Min_t{
    using type = Z<0>;
};

template<int I0,int...I>
struct Min_t<I0,I...> : Min_t<I0, Min_t<I...>::type::value >{};

template<int I0,int I1>
struct Min_t<I0,I1> {
  using type = Z<(I0<I1)?I0:I1>;
};


template<int...I>
using Min = typename Min_t<I...>::type;



}

#endif
