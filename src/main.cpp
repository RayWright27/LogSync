#include <iostream>
#include <chrono>
#include "windows.h"
#include "psapi.h"
#pragma comment( lib, "psapi.lib" )
#include "logical_clock_with_direct_dependency\lcwdd.hpp"
#include "controlled_logical_clock\clc.hpp"
#include "iterative_linear_least_squares\ills.hpp"

//---------------------------------------------------
/*
Сохранять не только переходы в таблицах, но и логические отметки времени?
Передавать 1 логическую отметку между процессами
*/
void testLCWDD(void){
  lcwdd::ProcessID id = 2;
  lcwdd::EventIndex idx = 4;

  auto start = std::chrono::high_resolution_clock::now();
  //16 событий в 1 цикле
  for(int i = 0; i < 1; i++){
    lcwdd::DependencyTrack(id, idx);
    lcwdd::ClearOutputArr();
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
  std::cout << "LCWDD duration = " << duration.count() << std::endl;

  lcwdd::ClearOutputArr();
  id = 3;
  idx = 1;
  lcwdd::DependencyTrack(id, idx);

}

//---------------------------------------------------
void testCLC(void){
  /*объявляем ивенты*/  
  clc::addEvent(clc::internal, 0, 0, 0);
  clc::addEvent(clc::send, /*1.5*/ 2.11, 0, 1);
  clc::addEvent(clc::internal, 2.12, 0, 2);
  clc::addEvent(clc::recieve, 3.57, 0, 3);
  clc::addEvent(clc::internal, 4.11587, 0, 4);

  clc::addEvent(clc::internal, 0, 1, 0);
  clc::addEvent(clc::internal, 1, 1, 1);
  clc::addEvent(clc::recieve, 1.65, 1, /*1*/2);
  clc::addEvent(clc::send, 2.42, 1, /*2*/3);
  clc::addEvent(clc::internal, 3.995, 1, /*3*/ 4);

  clc::addEvent(clc::send, 0, 2, 0);
  clc::addEvent(clc::internal, 0.4857, 2, 1);
  clc::addEvent(clc::recieve, 3.68, 2, 2);
  clc::addEvent(clc::internal, 4.3654, 2, 3);

  //std::shared_ptr<clc::Event> e_0_0(new clc::Event(clc::internal, 0, 0, 0));
  //std::shared_ptr<clc::Event> e_0_1(new clc::Event(clc::send, /*1.5*/ 2.11, 0, 1));
  //std::shared_ptr<clc::Event> e_0_2(new clc::Event(clc::internal, 2.12, 0, 2));
  //std::shared_ptr<clc::Event> e_0_3(new clc::Event(clc::recieve, 3.57, 0, 3));
  //std::shared_ptr<clc::Event> e_0_4(new clc::Event(clc::internal, 4.11587, 0, 4));

  //std::shared_ptr<clc::Event> e_1_0(new clc::Event(clc::internal, 0, 1, 0));
  //std::shared_ptr<clc::Event> e_1_05(new clc::Event(clc::internal, 1, 1, 1));
  //std::shared_ptr<clc::Event> e_1_1(new clc::Event(clc::recieve, 1.65, 1, /*1*/2));
  //std::shared_ptr<clc::Event> e_1_2(new clc::Event(clc::send, 2.42, 1, /*2*/3));
  //std::shared_ptr<clc::Event> e_1_3(new clc::Event(clc::internal, 3.995, 1, /*3*/ 4));

  //std::shared_ptr<clc::Event> e_2_0(new clc::Event(clc::send, 0, 2, 0));
  //std::shared_ptr<clc::Event> e_2_1(new clc::Event(clc::internal, 0.4857, 2, 1));
  //std::shared_ptr<clc::Event> e_2_2(new clc::Event(clc::recieve, 3.68, 2, 2));
  //std::shared_ptr<clc::Event> e_2_3(new clc::Event(clc::internal, 4.3654, 2, 3));
//
  /*инициализируем вектор процессов (перенос инфы из логов)*/
  //std::vector<clc::Process> processVecMain;
  //processVecMain.push_back(clc::Process(0, 
  //  std::vector<std::shared_ptr<clc::Event>>{e_0_0, e_0_1, e_0_2, e_0_3, e_0_4}));
  //processVecMain.push_back(clc::Process(1, 
  //  std::vector<std::shared_ptr<clc::Event>>{e_1_0, e_1_05, e_1_1, e_1_2, e_1_3}));
  //processVecMain.push_back(clc::Process(2, 
  //  std::vector<std::shared_ptr<clc::Event>>{e_2_0, e_2_1, e_2_2, e_2_3}));
  /*инициализируем список переходов между процессорами*/
  //std::vector<clc::EventCooperation_t> eventsCoopMap; 
  //eventsCoopMap.push_back(clc::EventCooperation_t {
  //  e_0_1, e_1_1});
  //eventsCoopMap.push_back(clc::EventCooperation_t {
  //  e_1_2, e_2_2});
  //eventsCoopMap.push_back(clc::EventCooperation_t {
  //  e_2_0, e_0_3});
  clc::addEventConnection(0, 1, 1, 2);
  clc::addEventConnection(1, 3, 2, 2);
  clc::addEventConnection(2, 0, 0, 3);

  /*создаём объект алгоритма*/                                                
  //clc::CLCSynchronizer Sync(eventsCoopMap, processVecMain);
  auto Sync = std::make_unique<clc::CLCSynchronizer>();
  Sync->syncTimestamps();
}
//---------------------------------------------------

void testILLS(void){
    /* initialising test values*/
    uint32_t cols = 2;
    uint32_t num_timestamps = 100; //rows


    std::vector<float> sender_timestamps(num_timestamps); // l_i, x_axis
    std::vector<float> reciever_timestamps(num_timestamps); // r_i, y_axis
    std::vector<uint32_t> idx_to_delete;

    for(uint32_t i = 0; i < num_timestamps; i++){
        if(i > 0)
            sender_timestamps.at(i) = sender_timestamps.at(i - 1) + 
            i + pow(-1, rand());
        else
             sender_timestamps.at(i) = rand()/(RAND_MAX/2);

        reciever_timestamps.at(i) = sender_timestamps.at(i) + 
            (float)(rand())/(float)(RAND_MAX/2) + 
            (i + 1)*(float)(rand())/(float)(RAND_MAX) ;
    }

    float result = 0;
    
    
    result = 
    illsEstimateSkew(sender_timestamps, reciever_timestamps, num_timestamps,
                     cols);
    
    

    std::cout << "resulting skew = " << result <<"\n";
};

//---------------------------------------------------

int main(){

  
  
  //auto start = std::chrono::high_resolution_clock::now();
  //for(int i = 0; i < 1; i++){
    testILLS();
  //}
  //auto stop = std::chrono::high_resolution_clock::now();
  //auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - 
  //                             start);
  //std::cout << "ILLS duration [ns] = " << duration.count() << std::endl;

  testLCWDD();

  //auto start = std::chrono::high_resolution_clock::now();
  // 14 событий в 1 цикле
  //for(int i = 0; i < 10000; i++){
  
  testCLC();
  
  /*PROCESS_MEMORY_COUNTERS_EX pmc;
  GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
  SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;*/
 
  //auto stop = std::chrono::high_resolution_clock::now();
  //auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
  //std::cout << "CLC duration = " << duration.count() << std::endl;

  std::vector<uint32_t> vec;
  vec.resize(5);
  vec.insert(vec.begin() + 4, 9);

 

  std::cout<<"tests finished\n";
 
}