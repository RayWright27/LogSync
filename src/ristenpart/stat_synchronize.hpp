//
// Created by Ray on 19.08.2024.
//

#ifndef LOGSYNC_STAT_SYNCHRONIZE_HPP
#define LOGSYNC_STAT_SYNCHRONIZE_HPP

#include <vector>
#include <cstdint>
#include <memory>
#include <set>

using namespace std;

// TODO: ОТКУДА БЕРЁТСЯ ИНФОРМАЦИЯ О СВЯЗИ ХОСТОВ??
namespace rist{
typedef struct Event{
    uint32_t entryNumber;
    /* host/process */
    uint32_t hostNumber;
    uint32_t depatureEventTimestamp; //p
    uint32_t arrivalEventTimestamp; //a
}Event_t;

class StatSync {
private:
    vector<shared_ptr<Event_t>> LogFile;
    /* number of entries in the log */
    uint32_t n_hg = 0;
    uint32_t D_hg = 0;
    // V_hg - множество!
    std::set<uint32_t> V_hg;
    uint32_t A_hg = 0;

    uint32_t pickRefHost(vector<shared_ptr<Event_t>> &LogFile,
                         Event_t event /*A*/, uint32_t V_hg, uint32_t n_hg);

public:
    StatSync(){};
    void addEventToLogFile(Event_t event);

    uint32_t synchronize(void);
};

}




#endif //LOGSYNC_STAT_SYNCHRONIZE_HPP
