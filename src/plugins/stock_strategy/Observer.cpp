/*
 * observer.cpp
 *
 *  Created on: 2016年8月1日
 *      Author: Maibenben
 */

#include "Observer.h"
#include "stock_basic_info.h"

namespace stock_logic {

Observer::Observer(stock_logic::Subject* subject) {
  subject_ = subject;
  factory_ = (StockFactory*) subject;
  cache_ = factory_->stock_usr_mgr_->stock_user_cache_;
  stock_total_map_ = &(cache_->stock_total_info_);
}

Observer::Observer() {
  subject_ = NULL;
  init();
}

Observer::~Observer() {
  // YGTODO Auto-generated destructor stub
}

std::string Observer::name() {
  return name_;
}
void Observer::set_name(std::string name) {
  name_ = name;
}

void Observer::init() {

}

void Observer::Process(int socket, base_logic::DictionaryValue* params) {
  std::string result = Analysis(params);
  LOG_MSG2("result=%s", result.c_str());
  std::string::iterator iter = result.begin();
  for (; iter != result.end();) {
    if (*iter != '{')
      result.erase(iter++);
    else
      break;
  }
  LOG_MSG2("after cleared result=%s", result.c_str());
  base_logic::LogicComm::SendFull(socket, result.c_str(),
                                  result.length());
}

std::string Observer::Analysis(base_logic::DictionaryValue* params) {
  std::string result = "";
  base_logic::DictionaryValue* result_dic = Request(params);
  if (NULL != result_dic)
    StockUtil::Instance()->serialize(result_dic, result);
  delete result_dic;
  result_dic = NULL;
  return result;
}

base_logic::DictionaryValue* Observer::Request(base_logic::DictionaryValue* params) {
  return NULL;
}

void RegisterEvent(int opcode) {

}

} /* namespace stock_logic */
