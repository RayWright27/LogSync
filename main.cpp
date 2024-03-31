#include <iostream>

#define N 4

typedef uint32_t ProcessID ;
typedef uint32_t EventIndex;

EventIndex DTV[N];
uint32_t D[N] = {1, 4, 3, 1};

void VisitEvent(ProcessID j, EventIndex y){
  for(uint32_t k = 0; k < N; k++){
    if(k != j){
      if(D[k] > DTV[k]){
        DTV[k] = D[k];
        VisitEvent(k, D[k]);
      }
    }
  }
}

void DependencyTrack(ProcessID i, EventIndex x){
  for(uint32_t k = 0; k < N; k++){
    if(k != i){
      DTV[k] = 0;
    }else{
      DTV[k] = x;
    }
  }

  VisitEvent(i, x);
}

int main(){
  ProcessID id = 1;
  EventIndex idx = 4;
  DependencyTrack(id, idx);

  std::cout<<"hello\n";
 
}