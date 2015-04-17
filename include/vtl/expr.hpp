#ifndef VTL_EXPR_HPP
#define VTL_EXPR_HPP



#include "fold.hpp"
#include "tuple.hpp"

namespace vtl {
inline namespace expressive{


template<template<class...>class T>
struct ExprTagHandler{
  template<class...X>
  using type = T<X...>;
};

template<class T,class U= typename T::ExprTag>
constexpr auto hasExprTag(T*){
  return 1;
}

template<class T>
constexpr auto hasExprTag(...){
  return 0;
}


template<
  class O,
  class L,
  class R,
  class result = typename std::conditional< 
      hasExprTag<Decay<L>>(0) , Decay<L> , Decay<R> >
    ::type::ExprTag::template type<O,L,R >
  >
constexpr auto makeExprFromTag(O&&o,L&&l,R&&r){
  return result(
    forward<O>(o),
    forward<L>(l),
    forward<R>(r) 
  );
}



template<class...>
struct Expr{
  using ExprTag = ExprTagHandler<expressive::Expr>;
  constexpr Expr(){}
  constexpr void operator()()const{}
};

template<class OP>
struct Expr<OP>{
  OP op;
  using ExprTag = ExprTagHandler<expressive::Expr>;
  constexpr Expr(OP&&op):op(forward<OP>(op)){}
  constexpr decltype(auto) operator()()const{
    return op;
  }

};

template<class OP,class...X>
struct Expr<OP,X...>{
  OP op;
  std::tuple<X...> Data;
  using ExprTag = ExprTagHandler<expressive::Expr>;
  constexpr Expr(OP&&op,X&&...x)
    :op(forward<OP>(op))
    ,Data(forward<X>(x)...)
  {}

  constexpr decltype(auto) operator()()const{
    return tupleCall(Data,op);
  }
};


template<class...X>
constexpr auto expr(X...x){ 
  return Expr<X...>(forward<X>(x)...); 
}



#define CREATE_EXPR_OP(OP)\
template<class L,class R>\
constexpr auto operator OP (L&&lhs,R&&rhs)\
-> decltype( makeExprFromTag( BinFold(OP) , forward<L>(lhs), forward<R>(rhs) ) ) {\
  return makeExprFromTag( BinFold(OP) , forward<L>(lhs), forward<R>(rhs) );\
}

CREATE_ALL_OPERATORS(CREATE_EXPR_OP)




}




}

#endif
