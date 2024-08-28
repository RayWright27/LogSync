#include "lcwdd.hpp"

using namespace lcwdd;

LCWDDSynchronizer::LCWDDSynchronizer(uint32_t numOfProcesses):
        numOfProcesses(numOfProcesses){
    if(this->numOfProcesses == 0){
        std::cout << "number of processes can't be zero";
        return;
    }
    for(uint32_t idx = 0; idx < this->numOfProcesses; idx++){
            this->DTV.push_back(0);
        }
}

void LCWDDSynchronizer::addDirectDependencyVectorEntry(vector<uint32_t> entry){
    if(entry.size() != this->numOfProcesses){
        std::cout << "Incorrect number of processes!";
    }
    this->D.push_back(entry);
}

void LCWDDSynchronizer::VisitEvent(ProcessID j, EventIndex y){
  /*
  каждый раз, когда VisitEvent находит в D 
  знач-е > порядк. №, хранящегося в DTV -> в эл-т 
  пишется новое знач-е 
  */
  for(int k = 0; k < this->numOfProcesses; k++){
    if(k != j - 1){
      if(this->D.at(j-1).at(k) > this->DTV.at(k)){
          this->DTV.at(k) = this->D.at(j-1).at(k);
        /* посещаем рекурсивно чтобы достать события из 
        конуса прошлого события Ei с индексом x
        */
        VisitEvent(k + 1, this->D[j - 1][k]);
      }
    }
  }
}

void LCWDDSynchronizer::DependencyTrack(ProcessID i, EventIndex x){
  /*инициализация i-го элемента, соответствующего событию
  с порядковым номером x*/
  for(int k = 0; k < this->numOfProcesses; k++){
    if(k != i){
        if(k - 1 < 0)
            continue;
      this->DTV.at(k - 1) = 0;
    }else{
      this->DTV.at(k - 1) = x;
    }
  }

  VisitEvent(i, x);
}

void lcwdd::LCWDDSynchronizer::ClearOutputArr(void){
    for(auto idx = 0; idx < DTV.size(); idx++) {
        std::cout << idx << "\n";
        this->DTV.at(idx) = 0;
    }
}