#ifndef VTL_TUPLE_HPP
#define VTL_TUPLE_HPP

#include "list.hpp"

namespace vtl {


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
    return tupleCall(forward<T>(tup), makeTuple, bits2Idx(c) );
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
#endif
