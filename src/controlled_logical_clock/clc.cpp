#include "clc.hpp"

using namespace clc;

Process::Process(process_id id,
                 vector<std::shared_ptr<clc::Event>> eventVect): 
    id(id),
    //adjTable(eventsNum, vector<timestamp_t>(eventsNum)),
    eventVect(eventVect){
        this->eventsNum = eventVect.size();
};

Process CLCSynchronizer::getProcessByID(process_id processID){
    for(uint32_t idx = 0; idx < processVec.size(); idx++){
        if(processVec.at(idx).getProcessID() == processID){
            return processVec.at(idx);
        }
    }
    assert(1 && "Error! No process with such ID!\n");
};

CLCSynchronizer::CLCSynchronizer(
                    vector<EventCooperation_t> interProcessVector,
                    vector<Process> processVec):
    processVec(processVec),
    processNum(processVec.size()),
    interProcessVector(interProcessVector),
    minMsgDelay(processNum, vector<timestamp_t>(processNum)){
        /*инициализация minMsgDelay*/
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
    for(uint32_t idx = 0; idx < interProcessVector.size(); idx++){
        if(currRcvEvent == interProcessVector.at(idx).recieveEvent){
            return interProcessVector.at(idx).sendEvent;
        }
    }
}

void CLCSynchronizer::clcComputeForwardAmortization(shared_ptr<Event> currEvent){
    /*equation 4(a)*/
    if(currEvent->getEventType() == recieve){
        if(currEvent->getEventNumber() != 0){
            currEvent->setTimestampCLC(
                max(
                    max(
                    searchForSendEvent(currEvent)->getTimestampCLC() + 
                    minMsgDelay
                        .at(searchForSendEvent(currEvent)->getEventLocation())
                        .at(currEvent->getEventLocation()),

                    getEventByIDs(currEvent->getEventNumber() - 1, 
                        currEvent->getEventLocation())->getTimestampCLC() + 
                        getProcessByID(currEvent->getEventLocation()).getDelta_i()               
                    
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
            currEvent->setTimestampCLC(
                max(
                    max(
                        getEventByIDs(currEvent->getEventNumber() - 1, 
                            currEvent->getEventLocation())->getTimestampCLC() + 
                            getProcessByID(currEvent->getEventLocation()).getDelta_i(),

                        
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

void CLCSynchronizer::syncTimestamps(void){
    for(uint32_t pass_num = 0; pass_num < 2; pass_num++){
        for(uint32_t proccess_id = 0; proccess_id < this->processVec.size(); proccess_id++){
            for(int32_t ev_id = this->processVec.at(proccess_id).getLastEventEstimated(); 
                ev_id < this->processVec.at(proccess_id).eventVect.size(); ev_id++){
                /*если это recieve скипаем сохраняя id последнего обработанного эл-та в процессе 
                т.к. мб неизвестена CLC отметка времени соответствующего ему send*/
                if((this->processVec.at(proccess_id).eventVect.at(ev_id)->getEventType() == recieve) &&
                    (pass_num == 0)){
                    this->processVec.at(proccess_id).setLastEventEstimated(ev_id);
                    if(proccess_id < this->processVec.size() - 1){
                        proccess_id++;
                        ev_id = -1;
                    }
                    /*если это последний процесс в векторе - переходим на повторный проход*/
                    else{
                        break;
                    }
                }else{
                    clcComputeForwardAmortization(
                        this->processVec.at(proccess_id).eventVect.at(ev_id));
                }
            }
        }
    }
};