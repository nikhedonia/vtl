#include<igloo/igloo_alt.h>
#include<vtl/vtl.hpp>
#include<type_traits>
#include<tuple>
#include<vector>
#include<typeinfo>
#include<chrono>

using namespace igloo;
using namespace vtl;
using namespace std;


constexpr float addOneFloat(int x){ 
  return x+1.0f; 
}



Describe(vtlTestcase)
{
  Describe(ConstexprLambda){
    It(should_allowConstexprLambdas){

      constexpr auto y = vtlLambda( (int i)const{ return i; } );
      constexpr auto z=N<y(1)>();
      Assert::That((uint)z,Equals(1) );

    };
  };



  Describe(ExtractorTests){
    It(should_have_working_extractors){
      
      Assert::That( _<0>(1,2,3,4) , Equals(1) );
      Assert::That( _<3>(1,2)(3,4) , Equals(4) );
      Assert::That( _<0,int>(1.2f,2,3,4) , Equals(1) );
      Assert::That( _<3,int>(1,2)(3)(4.1) , Equals(4) );


      auto z = 2;
      Assert::That( z , Equals(2) );
      Assert::That(vtl::bind(_<0>,_<1>+1)(1,2), Equals(3) );

      _<1>(1,_<0>(z),1)=32;
      Assert::That( z , Equals(32) );
      vtl::bind([](auto& x,auto y){x=14+y;} , z,_<0> )(12);
      Assert::That( z , Equals(26) );
      (_<1> =_<1>+_<0>+1)(z,z);
      Assert::That( z, Equals(53) );

    };
  };




  Describe(IntegralConstantTests){

    It(should_be_arithmetic_and_constexpr){
      constexpr auto Num = Z<50>() + Z<50>();
      Assert::That( (bool)!!Num , Equals(1) );
      Assert::That( Num.value , Equals(100) );
      Assert::That( (int)Num , Equals(100) );
    };

  };



  Describe(TypeListTests){

    It(should_be_countable){
      constexpr auto L0 = List<>();
      constexpr auto L3 = List<int,float,bool>();
      constexpr int C0 = count(L0);
      constexpr int C3 = count(L3);
      
      Assert::That( C0, Equals(0));
      Assert::That( C3, Equals(3));
      Assert::That( (int)Count<decltype(L3)>(), Equals(3) );
    };

    It(should_have_correct_sized_range){
      constexpr auto L0 = List<>();
      constexpr auto L3 = List<int,float,bool>();
      constexpr auto R0 = range(count(L0));
      constexpr auto R3 = range(count(L3));
      
      Assert::That( (int)count(R0), Equals(0));
      Assert::That( (int)count(R3), Equals(3));
    };

    It(should_have_accessable_elements){
      using L3 = NumList<1,2,3>;
      
      Assert::That( Get<L3,0>::value, Equals(1));     
      Assert::That( Get<L3,1>::value, Equals(2));
      Assert::That( Get<L3,2>::value, Equals(3));
    };

    It(should_be_concatable){
      using L0 = List<>;
      using L1a = List<Z<2>>;
      using L1b = List<Z<3>>;
      using L2 = NumList<2,3>;

      Assert::That( is_same<Concat<L0,L0>,L0>::value );  
      Assert::That( is_same<Concat<L1a,L1b>,L2>::value );  
      Assert::That( is_same<Concat<L0,L2>,L2>::value );
      Assert::That( is_same<Concat<L0,L1a,L0,L1b,L0>,L2>::value );  
    };


    It(should_be_searchable){
      using needle1 = int;
      using needle2 = float;
      using needle3 = unsigned;
      using haysack1 = List<int,float,bool,float,long,char>;

      Assert::That(is_same< IndexOf<needle1,haysack1> , NumList<0>>::value);
      Assert::That(is_same< IndexOf<needle2,haysack1> , NumList<1,3>>::value);
      Assert::That(is_same< IndexOf<needle3,haysack1> , NumList<>>::value);
      Assert::That(is_same< IndexOf<needle1,List<>> , NumList<>>::value);
    };


    It(should_be_copyable){
      using Source = List<int,float,bool,float,long,char>;

      using L1 = List<>;
      using L2 = List<char>;
      using L3 = List<int,float,int>;

      Assert::That(is_same< Copy<Source,NumList<>> , L1 >::value);
      Assert::That(is_same< Copy<Source,NumList<5>> , L2 >::value);
      Assert::That(is_same< Copy<Source,NumList<11>> , L2 >::value);
      Assert::That(is_same< Copy<Source,NumList<0,1,0>> , L3 >::value);
      Assert::That(is_same< Copy<List<>,NumList<1>> , List<> >::value);
    };

    It(should_be_enumerable){

      using L1 = NumList<0,1,2>;
      using L2 = List<int,float,int>;
      using L3 = List< List<Z<0>,int> , List<Z<1>,float> , List<Z<2>,int> >;

      Assert::That(is_same< Zip<L1,L2>,L3>::value);
      Assert::That(is_same< Enumerate<L2> , L3 >::value);
    };

    It(should_pass_set_operation){
      using L1 = NumList<0,1,1,2,3,2,6>;
      using L2 = NumList<0,1,2,3,4,5,7>;

      Assert::That(is_same<Distinct<L1>,NumList<0,3,6>>::value);
      Assert::That(is_same<Unique<L1>,NumList<0,1,2,3,6>>::value);
      Assert::That(is_same<Complement<L1,L2>,NumList<4,5,7>>::value);
      Assert::That(is_same<Union<L1,L2>,NumList<0,1,2,3,6,4,5,7>>::value);
      Assert::That(is_same<Meet<L1,L2>,NumList<0,1,2,3>>::value);
      Assert::That(is_same<Diff<L2,L1>,NumList<4,5,7>>::value);

      Assert::That(is_same<DistinctIdx<L1>,NumList<0,4,6>>::value);
      Assert::That(is_same<UniqueIdx<L1>,NumList<0,1,3,4,6>>::value);
      Assert::That(is_same<ComplementIdx<L1,L2>,NumList<4,5,6>>::value);
      Assert::That(is_same<UnionIdx<L1,L2>,NumList<0,1,3,4,6,11,12,13>>::value);


    };


  };  


  Describe(TupleTests){

    It(should_call_correctly){
      constexpr auto t1 = make_tuple(1,2,3);
      
      tupleCall(t1,[](auto a,auto b,auto c){
        Assert::That( a, Equals(1) );
        Assert::That( b, Equals(2) );
        Assert::That( c, Equals(3) );
      });

      tupleCall(t1,[](auto c,auto a){
        Assert::That( c, Equals(3) );
        Assert::That( a, Equals(1) );
      },NumList<2,0>() );

      tupleCall(t1,[](){},NumList<>() );



    };

    It(should_loop_correctly){
      constexpr auto t1 = make_tuple(1,2,3,4,5,6);
      
      int i=1;
      tupleForeach(t1,[&i](auto j){
        Assert::That( j, Equals(i) );
        ++i;
      });

      i=6;
      tupleForeach(t1,[&i](auto j){
        Assert::That( j, Equals(i) );
        --i;
      }, NumList<5,4,3,2,1>() );

    };

    It(should_pass_by_reference){
      auto t1 = make_tuple(1,2,3,4,5,6);
      tupleForeach(t1, [](auto&i){ i+=1; } );

      int i=2;
      tupleForeach( t1 , [&i](auto j){
        Assert::That( j, Equals(i) );
        ++i;
      });

    };

    It(should_map_correctly){
      
      constexpr auto t1 = make_tuple(1,2,3,4,5,6);    
      constexpr auto t2 = tupleMap(t1, addOneFloat );

      float i=2.0;
      tupleForeach(t2,[&i](auto j){
        Assert::That( j, Equals(i) );
        Assert::That( is_same<decltype(j),float>::value );
        i+=1.0;
      });

    };

    It(should_zip_correctly){
      
      constexpr auto t1 = make_tuple(1,2,3,0);    
      constexpr auto t2 = make_tuple(4,5,6); 
      constexpr auto t3 = make_tuple(7,8,9,0); 
      constexpr auto t4 = tupleZip(t1,t2,t3);

      int Arr[3][3] = { {1,4,7} , {2,5,8} , {3,6,9} };
      int i=0;   
      
      tupleForeach(t4,[&i,Arr](auto tup){
        int j=0;
        tupleForeach(tup,[&i,&j,Arr](auto v){
          Assert::That( Arr[i][j] , Equals(v) ); 
          ++j;
        });
        ++i;
      });

    };

    It(should_flatten_correctly){
      
      constexpr auto t1 = make_tuple(1,2,3,0);    
      constexpr auto t2 = make_tuple(4,5,6); 
      constexpr auto t3 = make_tuple(7,8,9,0); 
      constexpr auto t4 = tupleZip(t1,t2,t3);

      int Arr[3][3] = { {1,4,7} , {2,5,8} , {3,6,9} };
      int i=0;   
      
      tupleForeach(t4,flat([&i,Arr](auto a,auto b,auto c){
        Assert::That( Arr[i][0] , Equals(a) ); 
        Assert::That( Arr[i][1] , Equals(b) );
        Assert::That( Arr[i][2] , Equals(c) ); 
        ++i;
      }));

    };

    

    It(should_pass_some_examples){
      
      auto tup = make_tuple(true,3,4,1.0f);
      using T = decltype(tup);

      auto tiedInts = tupleCall(tup, makeTie, IndexOf<int,T,std::is_same>() ); //get references on the ints only
      //tupleCall(tup,flat(...),IndexOf<int,T>() )); should provide the same result...
      tupleCall(tiedInts,[](auto& a,auto& b){
        a=b=42;
      });

      Assert::That( get<1>(tup), Equals(42) );
    };
  };


  Describe(EinsteinTests){
    It(should_find_free_indexes){
      constexpr auto I=Index<0>();
      constexpr auto J=Index<1>();
      constexpr auto K=Index<2>();
      constexpr auto free1    = getFreeIndexes( +I , +I , +I ) ; 
      constexpr auto free2    = getFreeIndexes( +I , +J , -I ) ; 

      Assert::That( is_same< NumList<1,1,1> , Decay<decltype(free1)>>::value );
      Assert::That( is_same< NumList<0,1,0> , Decay<decltype(free2)>>::value );

      constexpr auto EinsteinLoop = makeEinstein( +I , +K , -J, +J, -I,-K, Index<5>() ) ;
      
      auto Data=[](auto i=-1,auto j=-1,auto k=-1,auto l=-1,auto m=-1,auto o=-1,auto p=-1)->double{ 
        //cout<<i<<j<<k<<l<<m<<o<<p<<endl;
        return i+j+k+l+m+o+1; 
      };

      //EinsteinLoop(Data,make_tuple(3,3,3),9);

/*
      double res2=0;
      auto start2 = std::chrono::system_clock::now();
      for(int ii=0;ii<40000000;++ii){
      //for(int i=0;i<1;++i){
        for(int j=0;j<30;++j){
          for(int k=0;k<5;++k){
            for(int l=0;l<4;++l){
              res2+= Data(j,k,l,l,j,k,9);
            }
          }        
        }
      }
      auto end2= std::chrono::system_clock::now();
      
      double res1=0;
      auto start = std::chrono::system_clock::now();
      cout<<endl;
      for(int ii=0;ii<40000000;++ii){
        res1+= EinsteinLoop(Data,make_tuple(30,5,4),9); //why the hell is this 20% faster????
      }


      auto end= std::chrono::system_clock::now();
      cout<<"time"<<std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count()<<endl;
      



      
      cout<<"time"<<std::chrono::duration_cast<std::chrono::milliseconds>(end2-start2).count()<<endl;
      cout<<endl<<res2<<"\n"<<res1<<endl;
*/
    };
  };



};

int main(int argc, char const* argv[])
{
  return TestRunner::RunAllTests(argc, argv);
}
