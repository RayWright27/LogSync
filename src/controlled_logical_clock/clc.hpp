#ifndef CLC_HPP
#define CLC_HPP

#include <cstdint>
#include <vector>
#include <iostream>
#include <cassert>
#include <memory>
#include <limits>
#include <cmath>

#include "../math/misc.hpp"

/*
    Задачи:
 1. Как будет происходить парсинг логов в местные массивы с процессами,
 событиями, переходами?
 2. Вычисление гаммы (нужно ли?) 
*/

namespace clc{

using namespace std;

#define TIMESTAMP_DOUBLE

enum event_t {internal, send, recieve};
#ifdef TIMESTAMP_DOUBLE
typedef double timestamp_t;
#endif
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
          event_location_t eventLocation, event_number eventNumber):
          eventType(event), timestamp(timestamp), 
          eventLocation(eventLocation), eventNumber(eventNumber){
            // пока дефолтное, это, либо 0.999
            this->gamma_i_j = 0.99f;
            this->timestampCLC = -1;
    };

    friend class Process;

    bool operator == (const Event &event){
        if(event.eventType == eventType &&
           event.eventLocation == eventLocation){
            return true;
        }
        else{
            return false;
        }
    }

    event_t getEventType(void){return this->eventType;};
    event_number getEventNumber(void){return this->eventNumber;};
    timestamp_t getTimestamp(void){return this->timestamp;};
    timestamp_t getTimestampCLC(void){
        assert(this->timestampCLC != -1 && 
                "Uninitialized CLC timestamp value was read!");
        return this->timestampCLC;
    };
    event_location_t getEventLocation(void){
        return this->eventLocation;};
    timestamp_t getGamma(void){return this->gamma_i_j;};
    void setTimestampCLC(timestamp_t val){this->timestampCLC = val;};
};

typedef struct EventCooperation {
    shared_ptr<Event> sendEvent;
    shared_ptr<Event> recieveEvent;

    EventCooperation(shared_ptr<Event> send, shared_ptr<Event> recieve):
        sendEvent(send), recieveEvent(recieve){
            assert(send->getEventType() == event_t::send && 
                    "First event should be of type send!");
            assert(recieve->getEventType() == event_t::recieve && 
                    "Second event should be of type recieve!");
    }

}EventCooperation_t;

class Process{
private:
    uint32_t eventsNum;
    process_id id;
    /*Minimal difference between two events on process i*/
    timestamp_t delta_i = 1e-1f;
    /*Вектор со всеми событиями процесса*/
    vector<shared_ptr<clc::Event>> eventVect;
    /*последнее событие, для которого был подсчитана CLC отметка
    (см. syncTimestamps)*/
    uint32_t lastEventEstimated = 0;
    /*подсчитаны ли все CLC отметки событий процесса*/
    bool clcComputed = false;

public:
    Process(process_id id, vector<std::shared_ptr<clc::Event>> eventVect);
    
    friend class CLCSynchronizer;

    timestamp_t getDelta_i(void){return this->delta_i;};
    process_id getProcessID(void){return this->id;};
    uint32_t getLastEventEstimated(void){return this->lastEventEstimated;};

    void setLastEventEstimated(uint32_t val){this->lastEventEstimated = val;};
    bool getCLCComputed(void){return this->clcComputed;};

    /*вовзращает true, если до currEvent нет события отправки, 
    нарушающего логику сообшений*/
    bool checkAmortizationInterval(shared_ptr<Event> currEvent);
};



class CLCSynchronizer{
private:
    /* Количество параллельных процессов/устройств */
    uint32_t processNum;
    /*Minimal message delay of messages from process k to process i,
      mu_k,i*/
    vector<vector<timestamp_t>> minMsgDelay;
    /*Массив всех процессов*/
    // TODO: необходима инициализация
    vector<Process> processVec;
    /*Вектор переходов между устройствами*/
    vector<EventCooperation_t> interProcessVector;

    /*Ищет событие отправки в таблице переходов между процессами/устройствами*/
    shared_ptr<Event> searchForSendEvent(shared_ptr<Event> currRcvEvent);

    /*Ищет событие приёма в таблице переходов между процессами/устройствами*/
    shared_ptr<Event> searchForRcvEvent(shared_ptr<Event> currSendEvent);

    /*Ищет событие по его id*/
    shared_ptr<Event> getEventByIDs(
        event_number eventNumber,
        event_location_t eventLocation){
            return this->processVec.at(eventLocation).eventVect.at(eventNumber);
        };
    /*Ищет процесс по его id*/
    Process getProcessByID(process_id processID);


public:
    CLCSynchronizer(
                    vector<EventCooperation_t> interProcessVector,
                    vector<Process> processVec);
    void clcComputeForwardAmortization(shared_ptr<Event> currEvent);
    void clcComputeBackwardAmortization(shared_ptr<Event> currEvent, uint32_t process_id);
    //TODO?:
    //page 5, para 3.1
    void evaluateGammas(void);

    /*главный метод, выполняет синхронизацию*/
    void syncTimestamps(void);
};

}

#endif //CLC_HPP