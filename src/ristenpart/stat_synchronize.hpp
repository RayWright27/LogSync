//
// Created by Ray on 19.08.2024.
//

#ifndef LOGSYNC_STAT_SYNCHRONIZE_HPP
#define LOGSYNC_STAT_SYNCHRONIZE_HPP

#include <vector>
#include <cstdint>
#include <memory>

using namespace std;

typedef struct LogFileEntry{
    uint32_t entryNumber;
    uint32_t depatureEventTimestamp;
    uint32_t arrivalEventTimestamp;
    /* host/process */
    uint32_t hostNumber;
}LogFileEntry_t;

typedef struct Host{
    vector<shared_ptr<LogFileEntry>> LogFile;
    /* number of entries in the log */
    uint32_t n_hg;
    uint32_t D_hg; //??
    uint32_t V_hg; //??
}Host_t;



#endif //LOGSYNC_STAT_SYNCHRONIZE_HPP
