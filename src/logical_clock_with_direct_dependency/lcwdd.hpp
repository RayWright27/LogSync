#ifndef LCWDD_HPP
#define LCWDD_HPP

#include <cstring>

namespace lcwdd{

#define NUM_OF_PROCESSES 4 // NUMBER OF PROCESSES

typedef int ProcessID ;
typedef int EventIndex;

void VisitEvent(ProcessID j, EventIndex y);

void DependencyTrack(ProcessID i, EventIndex x);

void ClearOutputArr(void);

}
#endif //LCWDD_HPP