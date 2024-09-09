//
// Created by Ray on 28.08.2024.
//


#include "stat_synchronize.hpp"

using namespace rist;

/*
 * В циклах проходимся по парам хостов
 */
uint32_t StatSync::synchronize(void){
    this->D_hg = 0;
    this->n_hg = 0;

    /* Пример: пара хостов захардкожена */
    uint32_t host1 = 0;
    uint32_t host2 = 1;
    for(uint32_t idx1 = 0; idx1 < this->LogFile.size(); idx1++){
        if(host1 == this->LogFile.at(idx1).get()->hostNumber){
            for(uint32_t idx2 = 0; idx2< this->LogFile.size(); idx2++){
                if(host2 == this->LogFile.at(idx2).get()->hostNumber) {
                    auto val = this->LogFile.at(idx1)->depatureEventTimestamp -
                               this->LogFile.at(idx1)->arrivalEventTimestamp -
                               (this->LogFile.at(idx2)->depatureEventTimestamp -
                                this->LogFile.at(idx2)->arrivalEventTimestamp);
                    this->D_hg += val;
                    if(auto search = this->V_hg.find(val);
                        search != this->V_hg.end()){
                        this->V_hg.insert(val);
                    }
                    this->n_hg++;
                }
            }
        }
    }
    // foreach h in H_F
        // foreach g in H_F, h =/= g
            this->A_hg += this->D_hg/this->n_hg;


}

void StatSync::addEventToLogFile(Event_t event){

}

uint32_t StatSync::pickRefHost(vector<shared_ptr<Event_t>>& LogFile,
                     Event_t event /*A*/, uint32_t V_hg, uint32_t n_hg){
    uint32_t m = UINT32_MAX;
}