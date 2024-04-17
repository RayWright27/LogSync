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
  std::shared_ptr<clc::Event> e_0_0(new clc::Event(clc::internal, 0, 0, 0));
  std::shared_ptr<clc::Event> e_0_1(new clc::Event(clc::send, 1.5, 0, 1));
  std::shared_ptr<clc::Event> e_0_2(new clc::Event(clc::internal, 2.12, 0, 2));
  std::shared_ptr<clc::Event> e_0_3(new clc::Event(clc::recieve, 3.57, 0, 3));
  std::shared_ptr<clc::Event> e_0_4(new clc::Event(clc::internal, 4.11587, 0, 4));
  std::shared_ptr<clc::Event> e_1_0(new clc::Event(clc::internal, 0, 1, 0));
  std::shared_ptr<clc::Event> e_1_1(new clc::Event(clc::recieve, 1.65, 1, 1));
  std::shared_ptr<clc::Event> e_1_2(new clc::Event(clc::send, 2.42, 1, 2));
  std::shared_ptr<clc::Event> e_1_3(new clc::Event(clc::internal, 3.995, 1, 3));
  std::shared_ptr<clc::Event> e_2_0(new clc::Event(clc::send, 0, 2, 0));
  std::shared_ptr<clc::Event> e_2_1(new clc::Event(clc::internal, 0.4857, 2, 1));
  std::shared_ptr<clc::Event> e_2_2(new clc::Event(clc::recieve, 3.68, 2, 2));
  std::shared_ptr<clc::Event> e_2_3(new clc::Event(clc::internal, 4.3654, 2, 3));
  /*инициализируем вектор процессов (перенос инфы из логов)*/
  std::vector<clc::Process> processVec;
  processVec.push_back(clc::Process(0, 
    std::vector<std::shared_ptr<clc::Event>>{e_0_0, e_0_1, e_0_2, e_0_3, e_0_4}));
  processVec.push_back(clc::Process(1, 
    std::vector<std::shared_ptr<clc::Event>>{e_1_0, e_1_1, e_1_2, e_1_3}));
  processVec.push_back(clc::Process(2, 
    std::vector<std::shared_ptr<clc::Event>>{e_2_0, e_2_1, e_2_2, e_2_3}));

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