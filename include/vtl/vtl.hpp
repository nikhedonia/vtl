#ifndef VTL_HPP
#define VTL_HPP

#include<type_traits>
#include<tuple>

namespace vtl {


template<class T>
constexpr auto extractInlineStruct(T const F){
    using S= decltype(F());
    return S();
}


#define vtlInlineStruct(STRUCT)\
    extractInlineStruct([](){\
        struct self\
        STRUCT;\
        return self();\
    })

#define vtlLambda(FUNC)\
    vtlInlineStruct({\
        constexpr self(){}\
        constexpr self(self const&){}\
        constexpr self operator|(int) { return *this; }\
        constexpr auto operator() FUNC\
    })

template<class T>
constexpr T Obj(...);

using uint = unsigned int;

constexpr long MCC( const char* S, const uint P=0, long Result=0)
{
    return (S[P])?MCC(S,P+1, 256*Result+S[P]):Result;
}

template<class b, class exp>
constexpr int Pow(b B, exp Exp) { return (Exp)?B*Pow(B,Exp-1):1; }

template<class...>
constexpr int C2N(){ return 0;}

template<class arg, class...args>
constexpr int C2N(arg Arg, args...Args) { return C2N<args...>(Args...) + Arg*Pow(10,sizeof...(args)) ;}

template<class...x> void nop(x...){}

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

using std::enable_if;
using std::forward;
using std::get;
using std::make_tuple;

template<class T>
using Decay = typename std::decay<T>::type;

template<class op>
struct Fold
{
  op OP;

  constexpr Fold(){}
  constexpr Fold(Fold const&){}
  constexpr Fold(op OP): OP(OP){}

  template<class X>
  constexpr auto operator()(X&&x)const
  -> decltype( forward<X>(x) ){
    return forward<X>(x);
  }

  template<class X0,class...X>
  constexpr auto operator()(X0&&x0,X&&...x)const
  -> decltype(OP( forward<X0>(x0) , (*this)(forward<X>(x)...) )){
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


/*
template<uint op>
constexpr auto BinaryFold = Fold<BinaryOp<op>>();

#define BinFold(OP) BinaryFold<CHAR(OP)>
*/

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

template<int i>
struct Extractor{
  static constexpr int argNum=i;
  constexpr Extractor(){}
  constexpr Extractor(Extractor const&){}



  template<int j=i,class x0,class...x>
  constexpr static auto eval(x0, x&&...X)
  -> typename enable_if< j ,
                         decltype(Extractor<j-1>::template eval<j-1>(forward<x>(X)...))>
  ::type {
    return Extractor<j-1>::eval(forward<x>(X)...);
  }

  template<int j=i,class x0,class...x>
  constexpr static auto eval(x0&&X0,x...)
  -> typename enable_if< !j ,
                         decltype(forward<x0>(X0)) >
  ::type {
    return forward<x0>(X0);
  }



  template<int=i>
  constexpr static auto eval() {
    return Extractor<i>();
  }

  template<class...x>
  constexpr auto operator()(x...X)const
  -> decltype(this->eval(forward<x>(X)...)) {
    return this->eval(forward<x>(X)...);
  }

  constexpr explicit operator int()const{
    return argNum;
  }

};

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



template<int i>
using _=Extractor<i>;


template<class L,uint i>
using Get = typename ListExtractor<i>::template type<L>;

template<uint i,class L>
constexpr auto get(L){ return Get<L,i>(); }


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



template<class L,class F>
constexpr auto listFold(L,F){
    return listFold(L(),F(), range(Z<0>(),count<L>()) );
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


template<class C, class Result=List<>, uint I = ((uint)count(C())?:1) -1>
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


template<class L,class idx>
struct Copy_t{
    using type = List<>;
};

template<template<class...> class L,template<class...> class R,class...I,class l0,class...l>
struct Copy_t<L<l0,l...>,R<I...>>{
    using type = List< Get<L<l0,l...>,I::value%Count<L<l0,l...>>()>... >;
};



template<class L,class R>
using Copy = typename Copy_t<Decay<L>, Decay<R>> ::type;



struct makeTuple_t{
    constexpr makeTuple_t(){}
    template<class...args>
    constexpr auto operator()(args&&...Args)const{
        return make_tuple(std::forward<args>(Args)...);
    }
};

struct makeTie_t{
    constexpr makeTie_t(){}
    template<class...args>
    constexpr auto operator()(args&&...Args)const{
        return std::tie(std::forward<args>(Args)...);
    }
};

struct makeList_t{
    constexpr makeList_t(){}
    template<class...args>
    constexpr auto operator()(args...)const{
        return List<args...>();
    }
};

constexpr auto makeTuple = makeTuple_t();
constexpr auto makeTie   = makeTie_t();
constexpr auto makeList  = makeList_t();


template<class F,class...X>
constexpr bool callHelper(F f, X&&...x){
    f(forward<X>(x)...);
    return 1;
}

template<class...F>
constexpr void callAll(F&&...f){
    nop( f()... );
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

template< template<class,class>class P>
struct checkTail{

    template<class L,class R>
    struct test{
        using type = Z<0>;
    };

    template<template<class...>class L,template<class...>class R,class V1,class T1,class V2,class T2>
    struct test< L<V1,T1> , R<V2,T2> >{
        using type = Z< P<T1,T2>::value && (V1()>=V2())>;
    };

    template<class T,class U>
    using type= typename test<Decay<T>,Decay<U>>::type;
};





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
using UniqueIdx= DistinctIdx< Enumerate<L>, checkTail<P>::template type >;

template<class L,template<class,class>class P=std::is_same>
using Unique= Copy<L,UniqueIdx<L,P> >;

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
using Diff = NotIn<R,L,P>;

template<class L,class R,template<class,class> class P=std::is_same>
using DiffIdx = NotInIdx<R,L,P>;

template<class L,class R,template<class,class> class P=std::is_same>
using SymDiff = Complement< Meet<L,R,P>  , Union<L,R,P> ,P>;

template<class L,class R,template<class,class> class P=std::is_same>
using SymDiffIdx = ComplementIdx< Meet<L,R,P>  , Union<L,R,P> ,P>;







template<class T,class F,template<class...>class range, class...I>
constexpr auto tupleCall(T&&tup,F&&f, range<I...> ){
    return f( get<I::value>(tup)... );
}

template<class T,class F>
constexpr auto tupleCall(T&&tup,F&&f){
    return tupleCall( std::forward<T>(tup) , std::forward<F>(f) , range(count(tup) ) );
}

template<class T,class C>
constexpr auto tupleKeep(T&&tup, C c){
    return tupleApply(forward<T>(tup), makeTuple, bitmask2Idx(c) );
}

template<class T,class F,template<class...>class range, class...I>
constexpr auto tupleMap(T&&tup,F&&f, range<I...> ){
    return make_tuple(f( get<I::value>(tup) )...);
}

template<class T,class F>
constexpr auto tupleMap(T&&tup,F&&f){
    return tupleMap( std::forward<T>(tup) , std::forward<F>(f) , range(count(tup)) );
}



template<class T,class F,template<class...>class range, class...I>
constexpr void tupleForeach(T&&tup,F&&f, range<I...> ){
    nop( callHelper(f,get<I::value>(tup) )...) ;
}

template<class T,class F>
constexpr void tupleForeach(T&&tup,F&&f){
    tupleForeach( std::forward<T>(tup) , std::forward<F>(f) , range(count(tup)) );
}


template<class T,template<class...>class range, class...I>
constexpr auto tupleEnumerate(T&&tup, range<I...> ){
    return make_tuple( make_tuple(I(), get<I::value>(tup))... );
}

template<class T>
constexpr auto tupleEnumerate(T&&tup){
    return tupleEnumerate( std::forward<T>(tup) , range(count(tup) ) );
}

template<class I,class...T>
constexpr auto tupleZipOne( I, T&&...tup ){
    return make_tuple( get<(uint)I()>( forward<T>(tup) )... );
}

template<class...T,class...I,class...J>
constexpr auto tupleZipAll(List<I...>,  T&&...tup ){
    return make_tuple( tupleZipOne( I(), tup... )... );
}

template<class...T>
constexpr auto tupleZip(T&&...tup){
    return tupleZipAll( range(Min<(uint)Count<T>()...>()) , tup...  );
}



template<class F>
struct tupleAdapter{
    F const f;
    constexpr tupleAdapter(F f):f(f){}

    template<class T>
    constexpr auto operator()(T&&tup)const{
        return tupleCall( forward<T>(tup) , f );
    }
};

template<class F>
constexpr auto flat(F f){
    return tupleAdapter<F>(f);
}



}

#endif // VTL_HPP
