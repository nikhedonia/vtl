#ifndef VTL_HPP
#define VTL_HPP

#include "utils.hpp"
#include "macro.hpp"
#include "expr.hpp"
#include "sym.hpp"
#include "extractor.hpp"
#include "list.hpp"
#include "tuple.hpp"
#include "set.hpp"


namespace vtl {






/*
template<uint ID=0, bool Up=0, bool Down=0>
struct Index{
  static constexpr auto id(){ return N<ID>(); }
  static constexpr bool up(){ return N<Up>(); }
  static constexpr bool down(){ return N<Down>(); }
  constexpr Index(){}
  
  constexpr auto operator+()const{ return Index<ID,1,0>(); }
  constexpr auto operator-()const{ return Index<ID,0,1>(); }
  constexpr auto operator*()const{ return Index<ID,1,1>(); }
  constexpr auto operator&()const{ return Index<ID,0,0>(); } 

};

template<class LHS,class RHS>
constexpr bool isSummable(LHS lhs,RHS rhs){
  return (lhs.id() == rhs.id() ) && ( (lhs.up() && rhs.down() ) || (lhs.down() && rhs.up() ) );
}

template<class I0,class...I>
constexpr bool isFreeIndex(I0 i0,I...i){
  return BinFold(&&)( !isSummable(i0,i)... );
}

template<template<class...>class L,class I0,class...I>
constexpr bool isFreeIndex(I0 i0,L<I...> LI){
  return BinFold(&&)( !isSummable(i0,I())... );
}

template<template<class...>class L,class...I,class...J>
constexpr auto getFreeIndexes(L<J...> LI,I...i){
  return NumList< isFreeIndex( _<J::value>(i...), copy( List<I...>(), remove(LI, L<J>())) )...>();
}

template<class...I>
constexpr auto getFreeIndexes(I...i){
  return getFreeIndexes<List>(Range<0,sizeof...(i)>(), i...);
}

template<uint ID>
constexpr auto _I=Index<ID>();

struct Negate_t{
  constexpr Negate_t(){}
  template<class T>
  constexpr auto operator()(T X)const{
    return !X;
  }
};

constexpr Negate_t Negate;

template<class...idx>
struct EinsteinCompiler{
  constexpr static auto freeIdxBits(){
    return getFreeIndexes(idx()...);
  }

  constexpr static auto summableIdxBits(){
    return tupleMap(freeIdxBits() , Negate );
  }

  constexpr static auto freeIdx(){
    return tupleKeep(List<idx...>(), freeIdxBits() );
  }

  constexpr static auto summableIdx(){
    return tupleKeep(List<idx...>(), summableIdxBits() );
  }

  constexpr static auto compile(){
    constexpr auto uint c=count(unique( tupleMap( freeIdx() , [](auto i){
      return i.id();
    })));

    return Range<0,c>

  }
*/




}

#endif // VTL_HPP
