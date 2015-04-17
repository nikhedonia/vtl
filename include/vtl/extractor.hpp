#ifndef VTL_EXTRACTOR_HPP
#define VTL_EXTRACTOR_HPP

#include "utils.hpp"
#include "macro.hpp"
#include "expr.hpp"

namespace vtl{

template<class T,class U= typename T::ExtractorTag>
constexpr auto hasExtractorTag(T*){
  return 1;
}

template<class T>
constexpr auto hasExtractorTag(...){
  return 0;
}


template<class E, class...X>
constexpr auto tryInject(E&&e,X&&...x)
->decltype( typename enable_if<hasExtractorTag<Decay<E>>(0)>::type() , e(forward<X>(x)...) ){
  return e(forward<X>(x)...);
}

template<class E, class...X>
constexpr auto tryInject(E&&e,X...)
-> decltype( typename enable_if<!hasExtractorTag<Decay<E>>(0)>::type() , forward<E>(e) ){
  return forward<E>(e);
}


template<
  template<class...> class L,
  class T,
  class...I, 
  class...X>
constexpr decltype(auto) inject(T&&Data,L<I...>,X&&...x){
  return std::tuple<decltype(tryInject(get<I::value>(forward<T>(Data)),forward<X>(x)...))...>( 
    tryInject(get<I::value>(forward<T>(Data)),forward<X>(x)...)... 
  );
}

template<class...X>
struct ExtractorExpr;

template<class OP,class...X>
struct ExtractorExpr<OP,X...>{
  OP op;
  std::tuple<X...> Data;
  using ExprTag = expressive::ExprTagHandler<vtl::ExtractorExpr>;
  using ExtractorTag = bool;
  constexpr ExtractorExpr(OP&&op,X&&...x)
    :op(forward<OP>(op))
    ,Data(forward<X>(x)...)
  {}

  template<class...Args>
  constexpr auto operator()(Args&&...args)const{
    return tupleCall( inject( Data, range(count(Data)) , forward<Args>(args)... ) , op );
  }


  template<class...Args>
  constexpr auto operator()(Args&&...args){
    return tupleCall( inject( Data, range(count(Data)) , forward<Args>(args)... ) , op );
  }

};

template<int i,class T=void>
struct Extractor{
  //static constexpr int argNum=i;

  using ExprTag = expressive::ExprTagHandler<vtl::ExtractorExpr>;
  using ExtractorTag = bool;


  constexpr Extractor(){}
  constexpr Extractor(Extractor const&){}


  template<int j=i,class x0,class...x>
  constexpr static auto eval(x0, x&&...X)
  -> typename enable_if< !!j ,
                         decltype(Extractor<j-1,T>()(forward<x>(X)...))>
  ::type {
    return Extractor<j-1,T>()(forward<x>(X)...);
  }

  template<int j=i,class x0,class...x>
  constexpr static auto eval(x0&&X0,x...)
  -> typename enable_if<!j && std::is_same<T,void>::value,       x0 >::type {
    return X0;
  }


  template<int j=i,class x0,class...x>
  constexpr static auto eval(x0&&X0,x...)
  -> typename enable_if< !j && !std::is_same<T,void>::value , T>::type {
    return X0;
  }

  template<int=i>
  constexpr static auto eval() {
    return Extractor<i,T>();
  }

  template<class...x>
  constexpr decltype(auto) operator()(x&&...X)const {
    return this->eval(forward<x>(X)...);
  }
/*
  constexpr explicit operator int()const{
    return argNum;
  }
*/
};

template<class F,class...X>
constexpr auto bind(F&&f,X&&...x){
  return ExtractorExpr<F,X...>(forward<F>(f),forward<X>(x)...);
}

template<int i,class T=void>
const constexpr static auto _= Extractor<i,T>();

}

#endif
