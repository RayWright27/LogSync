#include <iostream>

#include "logical_clock_with_direct_dependency\lcwdd.hpp"
#include "controlled_logical_clock\clc.hpp"

/*
Сохранять не только переходы в таблицах, но и логические отметки времени?
Передавать 1 логическую отметку между процессами
*/
int main(){
  lcwdd::ProcessID id = 2;
  lcwdd::EventIndex idx = 4;
  lcwdd::DependencyTrack(id, idx);

  lcwdd::ClearOutputArr();
  id = 3;
  idx = 1;
  lcwdd::DependencyTrack(id, idx);

  std::cout<<"hello\n";
 
}