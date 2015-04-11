CXXLIST     :=g++ /usr/lib/gcc-snapshot/bin/g++ clang++-3.5
CC          :=clang++-3.5
CXX         :=clang++-3.5
ANA         :=clang++-3.5
TARGET      :=Debug



LIBPATHS    := 
INCPATHS    :=-Iinclude
CXXFLAGS    :=-std=c++14 -Wall -Wpedantic
Debug       :=$(CXXFLAGS) -pg
Release     :=$(CXXFLAGS) -O3

CPP_FILTER  :=%.cpp %.cxx %.c %.cc
HPP_FILTER  :=%.hpp %.hxx %.h %.hh
