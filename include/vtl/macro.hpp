#ifndef VTL_MACRO_HPP
#define VTL_MACRO_HPP

#include "macro.hpp"

namespace vtl{

template<class F>
struct LambdaWrapper
{
  template<class... Ts>
  decltype(auto) operator()(Ts&&... xs) const {
    return reinterpret_cast<const F&>(*this)(std::forward<Ts>(xs)...);
  }
};

struct wrapper_factor{
  template<class F>
  constexpr LambdaWrapper<F> operator += (F*){
    return {};
  }
};

struct addr_add{
};

template<class T>
  typename std::remove_reference<T>::type*
  operator+(addr_add, T &&t) {
    return &t;
  }

template<class T>
constexpr auto unwrapHidden(T){
  return decltype(T()())();
}

#define staticLambda(L) (wrapper_factor() += true? nullptr : (addr_add() + L)   )

#define vtlLambdaStruct(S) unwrapHidden(staticLambda([](){\
  struct hidden{\
    S \
  };\
  return hidden(); \
}))

#define vtlLambda(L) vtlLambdaStruct(\
  constexpr hidden(){}\
  constexpr decltype(auto) operator() L \
)


constexpr long MCC( const char* S, const unsigned P=0, long Result=0)
{
    return (S[P])?MCC(S,P+1, 256*Result+S[P]):Result;
}

template<class b, class exp>
constexpr int Pow(b B, exp Exp) { return (Exp)?B*Pow(B,Exp-1):1; }

template<class...>
constexpr int C2N(){ return 0;}

template<class arg, class...args>
constexpr int C2N(arg Arg, args...Args) { return C2N<args...>(Args...) + Arg*Pow(10,sizeof...(args)) ;}


#define STRING(S) #S
#define CONCAT(lhs,rhs) lhs#rhs
#define CHAR(CH) MCC(STRING(CH))


#define CREATE_ARITHMETIC_OPERATORS( MACRO )\
MACRO(+) MACRO(-) MACRO(*) MACRO(/) MACRO(%)

#define CREATE_BINARY_OPERATORS( MACRO )\
MACRO(|) MACRO(&) MACRO(^)

#define CREATE_COMPARABLE_OPERATORS( MACRO )\
MACRO(&&) MACRO(||) MACRO(==) MACRO(!=)\
MACRO(<) MACRO(>) MACRO(<=) MACRO(>=)

#define CREATE_ASSIGMENT_OPERATORS( MACRO )\
MACRO(=) \
MACRO(+=) MACRO(-=) MACRO(*=) MACRO(/=) \
MACRO(%=) MACRO(|=) MACRO(&=) MACRO(^=)

#define CREATE_ALL_OPERATORS( MACRO )\
CREATE_ARITHMETIC_OPERATORS( MACRO )\
CREATE_BINARY_OPERATORS( MACRO )\
CREATE_COMPARABLE_OPERATORS( MACRO )

}

#endif
