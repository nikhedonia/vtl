CXXLIST     :=g++ /usr/lib/gcc-snapshot/bin/g++ clang++-3.6
CC          :=clang++-3.6
CXX         :=clang++-3.6
ANA         :=clang++-3.6
TARGET      :=Debug



LIBPATHS    := 
INCPATHS    :=-Iinclude
CXXFLAGS    :=-std=c++14 -Wall -Wpedantic
Debug       :=$(CXXFLAGS) -pg
Release     :=$(CXXFLAGS) -O3

CPP_FILTER  :=%.cpp %.cxx %.c %.cc
HPP_FILTER  :=%.hpp %.hxx %.h %.hh
