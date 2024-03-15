#include <iostream>

#include "logical_clock_with_direct_dependency\lcwdd.hpp"


int main(){
  ProcessID id = 2;
  EventIndex idx = 4;
  DependencyTrack(id, idx);

  std::cout<<"hello\n";
 
}