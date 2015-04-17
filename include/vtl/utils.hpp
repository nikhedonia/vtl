#ifndef VTL_UTILS_HPP
#define VTL_UTILS_HPP

#include <type_traits>

namespace vtl{
  using uint = unsigned int;

  using std::enable_if;
  using std::forward;
  using std::get;
  using std::make_tuple;
  using std::conditional;

  template<class T>
  using Decay = typename std::decay<T>::type;

  template<class T>
  constexpr T Obj(...);
  template<class...x> void nop(x...){}

  template<class F,class...X>
  constexpr bool callHelper(F f, X&&...x){
      f(forward<X>(x)...);
      return 1;
  }

  template<class...F>
  constexpr void callAll(F&&...f){
      nop( f()... );
  }


template<class...> 
struct Overload;


template<class...F>
struct Overload 
  : Overload<F>... {

  constexpr Overload(F&&...f) :  
    Overload<F>(std::forward<F>(f))...
  {}
};

template<class F> 
struct Overload<F> {
  constexpr Overload(F&&f) : f(std::forward<F>(f)) { }

  template <typename... Args>
  constexpr auto operator()(Args&&... args) const 
  -> decltype(std::declval<F>()(std::forward<Args>(args)...)) {
    return f(std::forward<Args>(args)...);
  }

private:
  F f;
};


template <typename... F>
constexpr auto overload(F&&...f){
  return Overload<F...>{ 
    std::forward<F>(f)... 
  };
}

/*
template<class T,uint...n>
struct arrayInfo{
  using dims = NumList<n...>;
};


template<class T,uint m, uint...n>
struct arrayInfo<T[m],n...>
  :arrayInfo<decltype(Obj<T>()[0]),n...,m>
{};
*/

template<class T>
constexpr decltype(auto) indexCall(T&&Data){
  return Data;
}

template<class T,class X,class...Xs>
constexpr decltype(auto) indexCall(T&&Data,X&&x,Xs&&...xs){
  return indexCall(Data[x],forward<Xs>(xs)...);
}


}

#endif
