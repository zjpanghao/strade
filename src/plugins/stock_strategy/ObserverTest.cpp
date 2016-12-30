/*
 * ObserverTest.cpp
 *
 *  Created on: 2016年8月4日
 *      Author: Maibenben
 */

#include "ObserverTest.h"

namespace stock_logic {

ObserverTest::ObserverTest(stock_logic::Subject* subject) {
  // YGTODO Auto-generated constructor stub
  name_ = "ObserverTest";
  subject_ = subject;
}

ObserverTest::~ObserverTest() {
  // YGTODO Auto-generated destructor stub
}

void ObserverTest::Update(int opcode, stock_logic::Subject*) {

}

base_logic::DictionaryValue* ObserverTest::Request(base_logic::DictionaryValue* params) {
  std::string start_date = ",2016-07-01";
    bool r = true;
    r = params->GetString(L"start_date", &start_date);
    if (true == r) {
      LOG_MSG2("get start_date=%s", start_date.c_str());
    }
    if (start_date.size() > 1)
      start_date.erase(start_date.begin());
    LOG_MSG2("start_date=%s", start_date.c_str());
    base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
    dict->SetString(L"start_date", start_date);

    return dict;
}

} /* namespace stock_logic */
