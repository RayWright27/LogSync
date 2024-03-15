#ifndef LCWDD_H
#define LCWDD_H

#define N 4 // NUMBER OF PROCESSES

typedef int ProcessID ;
typedef int EventIndex;

void VisitEvent(ProcessID j, EventIndex y);

void DependencyTrack(ProcessID i, EventIndex x);

#endif //LCWDD_H