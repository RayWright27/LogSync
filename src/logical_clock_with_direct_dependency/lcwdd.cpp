#include "lcwdd.hpp"
/*
* Векторная отметка времени для события Ei с индексом x 
* процесса Pi
*/
lcwdd::EventIndex DTV[NUM_OF_PROCESSES] = {0};

/*
* Direct dependency vector - в каждом элементе Di[k] 
* хранится локальное время процесса Pk на момент отправки
* Pk->Pi
* В любой момент знач-е Di[j] соответсвует порядк. 
* № последн. события в процессе Pj, от которого текущ.
* состояние процесса Pi зависит напрямую!
*/
// У НАС УЧАСТВУЕТ НЕСКОЛЬКО ВЕКТОРОВ D ДЛЯ КАЖДОГО ПРОЦЕССА
// В ВЫЗОВЕ VISIT EVENT С СООТВЕТСТВУЮЩИМИ ProcessID И EventIndex
int D[NUM_OF_PROCESSES][NUM_OF_PROCESSES] = {{1, 0, 0, 0},
               {1, 4, 3, 1},
               {0, 0, 3, 7},
               {0, 0, 0, 7}};

void lcwdd::VisitEvent(ProcessID j, EventIndex y){
  /*
  каждый раз, когда VisitEvent находит в D 
  знач-е > порядк. №, хранящегося в DTV -> в эл-т 
  пишется новое знач-е 
  */
  for(int k = 0; k < NUM_OF_PROCESSES; k++){
    if(k != j - 1){
      if(D[j - 1][k] > DTV[k]){
        DTV[k] = D[j - 1][k];
        /* посещаем рекурсивно чтобы достать события из 
        конуса прошлого события Ei с индексом x
        */
        VisitEvent(k + 1, D[j - 1][k]);
      }
    }
  }
}

void lcwdd::DependencyTrack(ProcessID i, EventIndex x){
  /*инициализация i-го элемента, соответствующего событию
  с порядковым номером x*/
  for(int k = 0; k < NUM_OF_PROCESSES; k++){
    if(k != i){
      DTV[k - 1] = 0;
    }else{
      DTV[k - 1] = x;
    }
  }

  VisitEvent(i, x);
}

void lcwdd::ClearOutputArr(void){
  memset(&DTV[0], 0, sizeof(DTV));
}