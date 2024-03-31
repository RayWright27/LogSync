#ifndef CLC_HPP
#define CLC_HPP

#include <cstdint>
#include <vector>
#include <iostream>

/*
    Задачи:
 1. Как будет происходить парсинг логов в местные массивы с процессами,
 событиями, переходами?
*/

namespace clc{

enum event_t {interlnal, send, recieve};
typedef uint64_t timestamp_t;
typedef uint32_t process_id;
typedef uint32_t event_number;
typedef process_id event_location_t;


class Event{
private:
    event_t eventType;
    timestamp_t timestamp;
    /*таймстамп, вычисленный CLC алгоритмом, LC'_i*/
    timestamp_t timestampCLC;
    /*процесс, i*/
    event_location_t eventLocation;
    /*порядковый номер события, j*/
    event_number eventNumber;
    /*Переменная контроля*/
    timestamp_t gamma_i_j;


public:
    Event(event_t event, timestamp_t timestamp, 
          event_location_t eventLocation):
          eventType(event), timestamp(timestamp), 
          eventLocation(eventLocation){};

    event_t getEventType(void){return this->eventType;};
    event_number getEventNumber(void){return this->eventNumber;};
    timestamp_t getTimestamp(void){return this->timestamp;};
    timestamp_t getTimestampCLC(void){return this->timestampCLC;};
    event_location_t getEventLocation(void){
        return this->eventLocation;};
    timestamp_t getGamma(void){return this->gamma_i_j;};
};


class Process{
private:
    uint32_t eventsNum;
    process_id id;
    /*Minimal difference between two events on process i*/
    timestamp_t delta_i;
    /*Таблица (матрица) смежности графа событий (Adjacency table/matrix)*/
    std::vector<std::vector<timestamp_t>> adjTable;
    /*Вектор со всеми событиями процесса*/
    std::vector<Event> eventVect;

public:
    Process(process_id id, uint32_t eventsNum);
    
    timestamp_t getDelta_i(void){return this->delta_i;};
};

class CLCSynchronizer{
private:
    unsigned numOfProcesses;
    /*Minimal message delay of messages from process k to process i,
      mu_k,i*/
    std::vector<std::vector<timestamp_t>> minMsgDelay;
    /*Массив всех процессов*/
    // TODO: необходима инициализация
    std::vector<Process> processVec;
    /*Таблица переходов между устройствами*/
    //TODO: формат?
    // multidim_vect interProcessAdjTable; ??

    /*Ищет событие отправки в таблице переходов между устройствами*/
    Event searchAdjTableForSendEvent(Event &currRcvEvent);

    /*Ищет событие по его id*/
    Event getEventByIDs(event_location_t eventLocation,
        event_number eventNumber);
    /*Ищет процесс по его id*/
    Process getProcessByID(process_id processID);

public:
    CLCSynchronizer(unsigned numOfProcesses);
    void clcComputeForwardAmortization(Event &currEvent);
    //TODO:
    //page 5, para 3.1
    void evaluateGammas(void);
};

}

#endif //CLC_HPP