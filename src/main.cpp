#include <iostream>

#include "logical_clock_with_direct_dependency\lcwdd.hpp"
#include "controlled_logical_clock\clc.hpp"

//---------------------------------------------------
/*
Сохранять не только переходы в таблицах, но и логические отметки времени?
Передавать 1 логическую отметку между процессами
*/
void testLCWDD(void){
  lcwdd::ProcessID id = 2;
  lcwdd::EventIndex idx = 4;
  lcwdd::DependencyTrack(id, idx);

  lcwdd::ClearOutputArr();
  id = 3;
  idx = 1;
  lcwdd::DependencyTrack(id, idx);

}

//---------------------------------------------------
void testCLC(void){
  /*объявляем ивенты*/  
  clc::Event e_0_0(clc::internal, 0, 0, 0);
  clc::Event e_0_1(clc::send, 1.5, 0, 1);
  clc::Event e_0_2(clc::internal, 2.12, 0, 2);
  clc::Event e_0_3(clc::recieve, 3.57, 0, 3);
  clc::Event e_0_4(clc::internal, 4.11587, 0, 4);
  clc::Event e_1_0(clc::internal, 0, 1, 0);
  clc::Event e_1_1(clc::recieve, 1.65, 1, 1);
  clc::Event e_1_2(clc::send, 2.42, 1, 2);
  clc::Event e_1_3(clc::internal, 3.995, 1, 3);
  clc::Event e_2_0(clc::send, 0, 2, 0);
  clc::Event e_2_1(clc::internal, 0.4857, 2, 1);
  clc::Event e_2_2(clc::recieve, 3.68, 2, 2);
  clc::Event e_2_3(clc::internal, 4.3654, 2, 3);

  /*инициализируем вектор процессов (перенос инфы из логов)*/
  std::vector<clc::Process> processVec;
  processVec.push_back(clc::Process(0, 
    std::vector<clc::Event>{e_0_0, e_0_1, e_0_2, e_0_3, e_0_4}));
  processVec.push_back(clc::Process(1, 
    std::vector<clc::Event>{e_1_0, e_1_1, e_1_2, e_1_3}));
  processVec.push_back(clc::Process(2, 
    std::vector<clc::Event>{e_2_0, e_2_1, e_2_2, e_2_3}));

  /*инициализируем список переходов между процессорами*/
  // TODO: переписать eventsCoopMap так, чтобы там были
  // ссылки на ивенты (чтоб подтягивать актуальные CLC timestamps)
  // инициализацию событиями мб делать внутри  CLCSynchronizer
  std::vector<clc::EventCooperation_t> eventsCoopMap; 
  eventsCoopMap.push_back(clc::EventCooperation_t {
    e_0_1, e_1_1});
  eventsCoopMap.push_back(clc::EventCooperation_t {
    e_1_2, e_2_2});
  eventsCoopMap.push_back(clc::EventCooperation_t {
    e_2_0, e_0_3});

  /*создаём объект алгоритма*/                                                
  clc::CLCSynchronizer Sync(eventsCoopMap, processVec);
  Sync.syncTimestamps();
}
//---------------------------------------------------

int main(){
  testLCWDD();

  testCLC();

  std::cout<<"tests finished\n";
 
}