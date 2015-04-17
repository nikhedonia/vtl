#ifndef VTL_FOLD_HPP
#define VTL_FOLD_HPP

#include "utils.hpp"
#include "macro.hpp"

namespace vtl{

template<class op>
struct Fold {
  op OP;

  constexpr Fold(){}
  constexpr Fold(Fold const&){}
  constexpr Fold(op OP): OP(OP){}

  template<class X>
  constexpr decltype(auto) operator()(X&&x)const{
    return forward<X>(x);
  }

  template<class X0,class...X>
  constexpr decltype(auto) operator()(X0&&x0,X&&...x)const{
    return    OP( forward<X0>(x0) , (*this)(forward<X>(x)...) );
  }
};

template<uint op>
struct BinaryOp
{};

#define CREATE_BinaryOP_struct(OP)\
template<>\
struct BinaryOp<CHAR(OP)>\
{\
  constexpr BinaryOp(){}\
  template<class X0, class X1>\
  constexpr auto operator()(X0&&x0,X1&&x1)const\
  -> decltype(x0 OP x1)\
  {\
    return x0 OP x1;\
  }\
};

CREATE_ALL_OPERATORS(CREATE_BinaryOP_struct)
CREATE_ASSIGMENT_OPERATORS(CREATE_BinaryOP_struct)



template<uint op>
constexpr auto BinaryFold = Fold<BinaryOp<op>>();

#define BinFold(OP) BinaryFold<CHAR(OP)>


}

#endif
