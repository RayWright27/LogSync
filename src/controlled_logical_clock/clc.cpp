#include "clc.hpp"

using namespace clc;

Process::Process(process_id id,
                 deque<std::shared_ptr<clc::Event>> eventVect): 
    id(id),
    eventVect(eventVect){
        this->eventsNum = eventVect.size();
};

Process::Process(const Process& process){
    this->eventsNum = process.eventsNum;
    this->id = process.id;
    this->delta_i = process.delta_i;
    /*copy(process.eventVect.begin(), process.eventVect.end(),
        this->eventVect.begin());*/
    for(uint32_t i = 0; i < process.eventVect.size(); i++){
        this->eventVect.push_back(process.eventVect.at(i));
    }
    this->lastEventEstimated = process.lastEventEstimated;
    this->clcComputed = process.clcComputed;
}

shared_ptr<Process> CLCSynchronizer::getProcessByID(process_id processID){
    for(uint32_t idx = 0; idx < processVector.size(); idx++){
        auto id = processVector.at(idx)->getProcessID();
        if(id == processID){
            return processVector.at(idx);
        }
    }
    assert(1 && "Error! No process with such ID!\n");
};

CLCSynchronizer::CLCSynchronizer(
                    /*vector<EventCooperation_t> interProcessVector,
                    vector<Process> processVec*/)
    //processVec(processVec),
    //processNum(processVec.size()),
    //interProcessVector(interProcessVector),
{
        /*инициализация minMsgDelay*/
    this->processNum = processVector.size() + 1;
    //processNum, vector<timestamp_t>(processNum));
    this->minMsgDelay.resize(processNum);
    for(uint32_t i = 0; i < this->minMsgDelay.size(); i++){
        this->minMsgDelay.at(i) = vector<timestamp_t>(processNum);
    }
    // пока что дефолтными значениями
    for(vector<vector<timestamp_t>>::iterator row 
        = minMsgDelay.begin();
        row != minMsgDelay.end(); 
        row++){
            for(vector<timestamp_t>::iterator col = row->begin(); 
                col != row->end(); col++){
                *col = 0.01;
            }
        }
        
};

shared_ptr<Event> CLCSynchronizer::searchForSendEvent(shared_ptr<Event> currRcvEvent){
    for(uint32_t idx = 0; idx < eventsCoopMap.size(); idx++){
        if(currRcvEvent == eventsCoopMap.at(idx).recieveEvent){
        std::cout<<"____\n";
        std::cout<<"searchForSendEvent()\n currRcvEvent = " << currRcvEvent;
        std::cout<<"____\n";

            return eventsCoopMap.at(idx).sendEvent;
        }
    }
        std::cout<<"____\n";
        std::cout<<"searchForSendEvent()\n currRcvEvent = " << currRcvEvent;
        std::cout<<"____\n";
};

shared_ptr<Event> CLCSynchronizer::searchForRcvEvent(shared_ptr<Event> currSendEvent){
    for(uint32_t idx = 0; idx < eventsCoopMap.size(); idx++) {
        if (currSendEvent == eventsCoopMap.at(idx).sendEvent) {
            return eventsCoopMap.at(idx).recieveEvent;
        }
    }
};


void CLCSynchronizer::clcComputeForwardAmortization(shared_ptr<Event> currEvent){
    /*equation 4(a)*/
    if(currEvent->getEventType() == recieve){
        if(currEvent->getEventNumber() != 0){
            auto tmp = getProcessByID(currEvent->getEventLocation());
            auto p = tmp->getDelta_i();
            currEvent->setTimestampCLC(
                max(
                    max(
                    searchForSendEvent(currEvent)->getTimestampCLC() + 
                    minMsgDelay
                        .at(searchForSendEvent(currEvent)->getEventLocation())
                        .at(currEvent->getEventLocation()),

                    getEventByIDs(currEvent->getEventNumber() - 1, 
                        currEvent->getEventLocation())->getTimestampCLC() + 
                        p
                    
                    ), 

                    max(
                        getEventByIDs(currEvent->getEventNumber() - 1, 
                        currEvent->getEventLocation())->getTimestampCLC() + 
                    currEvent->getGamma()*(
                        currEvent->getTimestamp() - 
                        (getEventByIDs(currEvent->getEventNumber() - 1, 
                        currEvent->getEventLocation())->getTimestamp())),

                    currEvent->getTimestamp()
                    )
                )
            );
        }
        // есди это первое событие, то некоторые элементы отбрасываются
        else{
            currEvent->setTimestampCLC(
                max(
                    searchForSendEvent(currEvent)->getTimestampCLC() + 
                    minMsgDelay
                        .at(searchForSendEvent(currEvent)->getEventLocation())
                        .at(currEvent->getEventLocation()),
                    currEvent->getTimestamp()
                )
            );
        }

    }
    /*equation 4(b)*/
    else if(currEvent->getEventType() == send or 
    currEvent->getEventType() == internal){
        if(currEvent->getEventNumber() != 0){
            auto tmp1 = currEvent->getEventLocation();
            auto tmp = getProcessByID(tmp1);
            auto p = tmp->getDelta_i();
            currEvent->setTimestampCLC(
                max(
                    max(
                        getEventByIDs(currEvent->getEventNumber() - 1, 
                            currEvent->getEventLocation())->getTimestampCLC() + 
                            p,

                        
                        getEventByIDs(currEvent->getEventNumber() - 1, 
                            currEvent->getEventLocation())->getTimestampCLC() + 
                        currEvent->getGamma()*(
                            currEvent->getTimestamp() - 
                            (getEventByIDs(currEvent->getEventNumber() - 1, 
                            currEvent->getEventLocation())->getTimestamp()))
                    ),

                    currEvent->getTimestamp()
                )
            );
        }else{
            currEvent->setTimestampCLC(currEvent->getTimestamp());
        }         
    }else{
        cout << "ERROR: invalid event type of current event!\n";
    }
};

bool Process::checkAmortizationInterval(shared_ptr<Event> currEvent){
    
    for(uint32_t idx = currEvent->getEventNumber(); idx > 0; idx--){
        if((this->eventVect.at(idx)->eventType == internal) 
            // проверяем соответствие timestamp_t стандартному типу и сравниваем
            // относительно соответсвующего эпсилон
            #ifdef TIMESTAMP_DOUBLE
            || (
            abs(static_cast<double>(this->eventVect.at(idx)->getTimestamp() - 
                this->eventVect.at(idx)->getTimestampCLC()))
            <= numeric_limits<double>::epsilon())
            #else
            assert(1 && "Timestamp datatype undefined!")
            #endif
            ){
                continue;
        }else{
            return false;
        }
    }
    return true;
};

void CLCSynchronizer::clcComputeBackwardAmortization(shared_ptr<Event> currEvent, 
    uint32_t process_id){
    auto currProcess = processVector.at(process_id);    
    if(currProcess->checkAmortizationInterval(currEvent)){
        /*..используем линейную интерполяцию, начиная с события, предшествующего
        currEvent'у*/
        for(uint32_t idx = currEvent->getEventNumber() - 1; idx > 0; idx--){
            double CLCval = lintrp(currProcess->eventVect.at(idx)->getTimestamp(), 
                currProcess->eventVect.at(0)->getTimestamp(),
                currEvent->getTimestamp(), 
                currProcess->eventVect.at(0)->getTimestampCLC(),
                currEvent->getTimestampCLC());
            timestamp_t a = 0;
            if(currProcess->eventVect.at(idx)->getEventType() != internal)
                a = searchForRcvEvent(currProcess->eventVect.at(idx))->getTimestampCLC();
            else
                a = 0.0f;
            auto b = minMsgDelay.at(currProcess->getProcessID()).
                at(currProcess->eventVect.at(idx)->getEventLocation());
            double LCminusMu = a -
                minMsgDelay.at(currProcess->getProcessID()).
                at(currProcess->eventVect.at(idx)->getEventLocation());    
            if(currProcess->eventVect.at(idx)->getEventType() == send){
                if(CLCval > LCminusMu){
                    /*.. если хоть один сюда попал - выходит кусочно-линейная интерполяция*/
                    currProcess->eventVect.at(idx)->setTimestampCLC(LCminusMu);                    
                }else{
                    currProcess->eventVect.at(idx)->setTimestampCLC(CLCval);                    
                }
            }else{
                currProcess->eventVect.at(idx)->setTimestampCLC(CLCval); 
            }
            return;
        }
    }
    /*..или не используем ничего..?*/    
};

void CLCSynchronizer::syncTimestamps(void){
    for(uint32_t pass_num = 0; pass_num < 2; pass_num++){
        for(uint32_t proccess_id = 0; proccess_id < processVector.size(); proccess_id++){
            for(int32_t ev_id = processVector.at(proccess_id)->getLastEventEstimated(); 
                ev_id < processVector.at(proccess_id)->eventVect.size(); ev_id++){
                /*если это recieve скипаем сохраняя id последнего обработанного эл-та в процессе 
                т.к. мб неизвестена CLC отметка времени соответствующего ему send*/
                if((processVector.at(proccess_id)->eventVect.at(ev_id)->getEventType() == recieve) &&
                    (pass_num == 0)){
                    processVector.at(proccess_id)->setLastEventEstimated(ev_id);
                    if(proccess_id < processVector.size() - 1){
                        proccess_id++;
                        ev_id = -1;
                    }
                    /*если это последний процесс в векторе - переходим на повторный проход*/
                    else{
                        break;
                    }
                }else{
                    clcComputeForwardAmortization(processVector.at(proccess_id)->eventVect.at(ev_id));

                    if(processVector.at(proccess_id)->eventVect.at(ev_id)->getEventType() == recieve){
                        clcComputeBackwardAmortization(processVector.at(proccess_id)->eventVect.at(ev_id),
                            proccess_id);
                    }
                }
            }
        }
    }
};

void clc::addEvent(event_t evType, timestamp_t timestamp, 
    process_id evLocation, event_number evNum){
    event_number id;

        std::shared_ptr<Event> event = 
            std::make_shared<Event>(evType, timestamp, evLocation, evNum);
        //shared_ptr<Event> event(new Event(evType, timestamp, evLocation, evNum));
        
        /* если вектор событий процесса ещё не содержит процесс с данным 
           event_location_t - создаём и сам процесс и само событие внутри него */
        if((evLocation >= processVector.size() /*&& evLocation>0*/) /*or 
            evLocation == 0*/){
            std::deque<std::shared_ptr<Event>>eventVect;

            shared_ptr<Process> process = std::make_shared<Process>(
                static_cast<event_number>(/*evNum*/evLocation), eventVect);

            process->getEventVect().resize(evNum);

            process->getEventVect().insert(
                process->getEventVect().begin() + evNum, event);
            
            processVector.resize(evLocation);
            processVector.insert(processVector.begin() + evLocation, 
                process);
        }
        else{
            if(processVector[evLocation] == NULL){
                std::deque<std::shared_ptr<Event>>eventVect;
                id = static_cast<event_number>(/*evNum*/evLocation);
                Process process(id, eventVect);
                process.getEventVect().resize(evNum);

                process.getEventVect().insert(
                    process.getEventVect().begin() + evNum, event);
                
                processVector.at(evLocation) = make_shared<Process>(process);
            }
            /* иначе событие записываем в вектор событий соотв. процесса */
            else{
                auto& evVect = processVector.at(evLocation)->getEventVect();
                if(evNum > evVect.size()){
                    evVect.resize(evNum);
                    evVect.insert(evVect.begin() + evNum, event);
                }else{
                    evVect.insert(evVect.begin() + evNum, event);
                }
            }
        }
             
    
};

void clc::addEventConnection(process_id sendEvLocation, event_number sendEvNum, 
    process_id recvEvLocation, event_number recvEvNum){
        auto sendEv = 
            processVector.at(sendEvLocation)->getEventVect().at(sendEvNum);
        auto recvEv = 
            processVector.at(recvEvLocation)->getEventVect().at(recvEvNum);

        EventCooperation_t evPair{sendEv, recvEv};

        eventsCoopMap.push_back(evPair);
}
