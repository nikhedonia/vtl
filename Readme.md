#VTL - variadic template library
##A C++14 Library to work with tuples and variadic templates in a functional manner


###examples:


```c++
#include<vtl.hpp>
using vtl;

constexpr Z<1> i=1_Z;
constexpr int j=i;//j=1;
constexpr Extractor<1> e=_<1>;
constexpr int k=e(1,2,3); // k=2;
constexpr List<Z<0>,Z<1>,Z<2>> L = range(0_Z,3_Z); // range(3_Z);
count(L)==3; 

constexpr auto t1 = make_tuple(1,2,3);
constexpr auto t2 = make_tuple(4,5,6);
constexpr auto t3 = make_tuple(7,8,9);
count(t1)==3;

auto print3=[](auto a,auto b, auto c)){
  cout<<a<<b<<c<<endl;
};

auto print1=[](auto arg){
  cout<<arg<<endl;
  return 1;
};

//all functions are constexpr if args are constexpr...

auto out=tupleCall( t1, print3 ); // out=1; cout<<1<<2<<3<<endl; 
tupleCall( t1 ,print3, NumList<2,1,0>()  ); // cout in reversed order


tupleForeach( t1 , print1 );
tupleForeach( t1 , print1 ,  NumList<2,1,0>()  ); // cout in reversed order

constexpr auto t4 = tupleZip( t1 , t2, t3 ); 
/*
t4 = make_tuple( 
          make_tuple(1,4,7),
          make_tuple(2,5,8),
          make_tuple(3,6,9)
);*/

constexpr auto t5 = tupleEnumerate( t1 ); 
/*
t5 = make_tuple( 
          make_tuple(0_Z,1),
          make_tuple(1_Z,2),
          make_tuple(2_Z,3)
);*/

constexpr auto t6 = tupleKeep( t1, NumList<1,0,1>() ); // NumList acts as a Bitmask 
/*t6 = make_tuple(1,3); */


```

More examples can be found in the test folder.

  
