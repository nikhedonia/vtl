#ifndef VTL_EINSTEIN_HPP
#define VTL_EINSTEIN_HPP

#include "tuple.hpp"

namespace vtl {

template<uint ID=0, bool Up=0, bool Down=0>
struct Index{
  static constexpr auto id(){ return ID; }
  static constexpr bool up(){ return Up; }
  static constexpr bool down(){ return Down; }
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

template<class I,class J>
struct IndexCompare{
  constexpr static auto value = ( I::id() == J::id() );
};

struct AsExtractor{

  constexpr AsExtractor(){}
  template<class T>
  constexpr auto operator()(T)const{
    return Extractor<T::value>();
  }

};

constexpr auto asExtractor = AsExtractor();

  
  /* makeOneToNExtractor
  T[M][N][O] =>
   I  J  K
   I=(x/N*O)%M 
   J=(x/O)%N
   K=(x)%O
  */
  template<class X,class...Xs>
  constexpr static auto makeOneToN(X&&x,Xs&&...xs){
    return _<0> / BinFold(*)(forward<Xs>(xs)...) % x;
  }

  template<class X>
  constexpr static auto makeOneToN(X&&x){
    return _<0> % x ;
  }

  template<class...Xs>
  struct OneToNHelper{
    template<class...E>
    constexpr static auto pack(E&&...e){
      return make_tuple(forward<E>(e)...);
    }  
  };
  
  template<class X,class...Xs>
  struct OneToNHelper<X,Xs...>{
    template<class...E>
    constexpr static auto pack(X&&x,Xs&&...xs,E&&...e){
      return OneToNHelper<Xs...>::pack( 
        forward<Xs>(xs)... ,
        forward<E>(e)..., 
        makeOneToN(forward<X>(x), forward<Xs>(xs)...) 
      );
    }  
  };

  template<class...Xs>
  constexpr static auto makeOneToNTuple(Xs&&...xs){
    return OneToNHelper<Xs...>::pack(forward<Xs>(xs)...);
  } 





#include<iostream>

  template<class F, class S, class E>
  struct Accumulator{

    constexpr Accumulator()
    {}


    template<class C,class D,class B,class...Ex, class...J, class...X>
    constexpr static auto eval(
      C&&caller,
      D&&Data,
      std::tuple<Ex...>,
      List<J...>, 
      B&&b, 
      X...x)
    {
      auto const c = BinFold(*)(get<J::value>(b)...);
      auto const OneToN = makeOneToNTuple(get<J::value>(b)...);

      Decay<decltype(caller(
          forward<D>(Data),
          Ex()(forward<X>(x)... , get<J::value>(OneToN)(0)... )...
     ))> result = 0;


      for(uint i = 0; i<c ; ++i){
        result+= caller(
          forward<D>(Data),
          Ex()(forward<X>(x)... , get<J::value>(OneToN)(i)... )...
        );
      }
      return result;
    }
    
    template<
      class C,
      class D,
      template<class...> class Bounds,
      class...B,
      class...X>
    constexpr auto operator()(C&&c,D&&Data, Bounds<B...> bounds, X&&...x)const{
      return eval(
        forward<C>(c),
        forward<D>(Data), 
        E(),
        rangeOf(bounds),
        bounds,
        forward<X>(x)... 
      );
    }

    
    template<class...X>
    constexpr auto operator()(X&&...x)const{
      return (*this)(funcCall, forward<X>(x)... );
    }




  };

  template<class F, class S, class E>
  constexpr static auto makeAccumulator(F,S,E){
    return Accumulator<F,S,E>();
  }




template<class...idx>
struct Einstein{

  static constexpr auto indexes = List<idx...>();

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
    constexpr auto free     = freeIdxBits();
    constexpr auto summable = summableIdxBits();
    constexpr auto all      = List<idx...>();
    
    //constexpr auto e = tupleMap(rangeAll,asExtractor);
    constexpr auto reordered = unique<IndexCompare>(listConcat( tupleKeep(all,free) , tupleKeep(all,summable) ));
    constexpr auto sorted = indexOfEach<IndexCompare>( all , reordered );
    constexpr auto e = tupleMap( sorted , asExtractor );

    return makeAccumulator(
      free,
      summable,
      e
    );
  }
};

template<class...i>
constexpr auto makeEinstein(i...){
  return Einstein<i...>::compile();
}



}

#endif
