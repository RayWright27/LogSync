#ifndef LCWDD_HPP
#define LCWDD_HPP

#include <cstring>
#include <vector>
#include <memory>
#include <iostream>

using namespace std;

namespace lcwdd{

//#define NUM_OF_PROCESSES 4 // NUMBER OF PROCESSES

typedef int ProcessID ;
typedef int EventIndex;

class LCWDDSynchronizer{
private:
    uint32_t numOfProcesses = 0;
    /*
    * Результат - векторная отметка времени для события Ei с индексом x
    * процесса Pi
    */
    vector<uint32_t> DTV;
    /*
    * Direct dependency vector - в каждом элементе Di[k]
    * хранится локальное время процесса Pk на момент отправки
    * последнего сообщения из Pk в Pi
    * В любой момент знач-е Di[j] соответсвует порядк.
    * № последн. события в процессе Pj, от которого текущ.
    * состояние процесса Pi зависит напрямую!
    */
    // У НАС УЧАСТВУЕТ НЕСКОЛЬКО ВЕКТОРОВ D ДЛЯ КАЖДОГО ПРОЦЕССА
    // В ВЫЗОВЕ VISIT EVENT С СООТВЕТСТВУЮЩИМИ ProcessID И EventIndex
    vector<vector<uint32_t>> D;

    void VisitEvent(ProcessID j, EventIndex y);

public:
    LCWDDSynchronizer(uint32_t numOfProcesses);
    void addDirectDependencyVectorEntry(vector<uint32_t> entry);
    void DependencyTrack(ProcessID i, EventIndex x);
    vector<uint32_t> getResult (void) {return this->DTV;};
    void ClearOutputArr(void);
};




}
#endif //LCWDD_HPP