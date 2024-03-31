#include "clc.hpp"

using namespace clc;

Process::Process(process_id id, uint32_t eventsNum): 
    id(id),
    adjTable(eventsNum, std::vector<timestamp_t>(eventsNum)),
    eventVect(eventsNum) {

};

CLCSynchronizer::CLCSynchronizer(unsigned numOfProcesses):
    numOfProcesses(numOfProcesses),
    minMsgDelay(numOfProcesses, std::vector<timestamp_t>(numOfProcesses)){
        /*инициализация minMsgDelay*/
        // пока что дефолтными значениями
        for(std::vector<std::vector<timestamp_t>>::iterator row 
            = minMsgDelay.begin();
            row != minMsgDelay.end(); 
            row++){
                for(std::vector<timestamp_t>::iterator col = row->begin(); 
                    col != row->end(); col++){
                    *col = 0;
                }
            }
};

void CLCSynchronizer::clcComputeForwardAmortization(Event &currEvent){
    /*equation 4(a)*/
    if(currEvent.getEventType() == recieve){
        if(currEvent.getEventNumber() != 0){
            std::max(
                std::max(
                searchAdjTableForSendEvent(currEvent).getTimestampCLC() + 
                minMsgDelay
                    .at(searchAdjTableForSendEvent(currEvent).getEventLocation())
                    .at(currEvent.getEventLocation()),

                getEventByIDs(currEvent.getEventNumber() - 1, 
                    currEvent.getEventLocation()).getTimestampCLC() + 
                    getProcessByID(currEvent.getEventLocation()).getDelta_i()               
                
                ), 

                std::max(
                    getEventByIDs(currEvent.getEventNumber() - 1, 
                    currEvent.getEventLocation()).getTimestampCLC() + 
                currEvent.getGamma()*(
                    currEvent.getTimestamp() - 
                    (getEventByIDs(currEvent.getEventNumber() - 1, 
                    currEvent.getEventLocation()).getTimestamp())),

                currEvent.getTimestamp()
                )
            );
        }
        // есди это первое событие, то некоторые элементы отбрасываются
        else{
            std::max(
                searchAdjTableForSendEvent(currEvent).getTimestampCLC() + 
                minMsgDelay
                    .at(searchAdjTableForSendEvent(currEvent).getEventLocation())
                    .at(currEvent.getEventLocation()),
                currEvent.getTimestamp()
            );
        }

    }
    /*equation 4(b)*/
    else if(currEvent.getEventType() == send or 
    currEvent.getEventType() == interlnal){
        if(currEvent.getEventNumber() != 0){
            std::max(
                std::max(
                    getEventByIDs(currEvent.getEventNumber() - 1, 
                        currEvent.getEventLocation()).getTimestampCLC() + 
                        getProcessByID(currEvent.getEventLocation()).getDelta_i(),

                    
                    getEventByIDs(currEvent.getEventNumber() - 1, 
                        currEvent.getEventLocation()).getTimestampCLC() + 
                    currEvent.getGamma()*(
                        currEvent.getTimestamp() - 
                        (getEventByIDs(currEvent.getEventNumber() - 1, 
                        currEvent.getEventLocation()).getTimestamp()))
                ),

                currEvent.getTimestamp()
            );
        }else{
            currEvent.getTimestamp();
        }
    }else{
        std::cout << "ERROR: invalid event type of current event!\n";
    }
};