/*
 * HistEventsObserver.cpp
 *
 *  Created on: 2016年9月29日
 *      Author: Maibenben
 */

#include <mysql.h>
#include "HistEventsObserver.h"
#include "DateUtil.h"
#include "stock_basic_info.h"
#include "stock_db.h"

namespace stock_logic {

void StocksPerDay::add_stock(std::string& stock_code, std::string& event_name) {
  //stocks_info_[stock_code] = event_name;
}

HistEventsObserver::HistEventsObserver(stock_logic::Subject* subject) {
  this->set_name("HistEventsObserver");
  factory_ = (StockFactory*)subject;
  mysql_engine_ = factory_->stock_db_->mysql_engine_;
}

HistEventsObserver::~HistEventsObserver() {
  // YGTODO Auto-generated destructor stub
}

void HistEventsObserver::Update(int opcode, stock_logic::Subject* subject) {
  LOG_MSG2("HistEventsObserver::Update opcode=%d",
             opcode);
  switch(opcode){

    case 0: {
      this->fetch_hist_events_info();
      break;
    }

    default:
    break;
  }
}

void HistEventsObserver::add_stock_by_day(std::string& date,
                      std::string& stock_code,
                      std::string& event_name) {
  this->event_stocks_[date].add_stock(stock_code, event_name);

}

bool HistEventsObserver::fetch_hist_events_info() {
  bool r = false;
  std::string sql = "call proc_FectchHistEventsInfo2();";
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallFectchEventsInfo);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  int count = listvalue->GetSize();
  StockFactory* factory = StockFactory::GetInstance();
  while (count > 0) {
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    this->ValueSerialization(dict_result_value);
    delete dict_result_value;
    dict_result_value = NULL;
  }
  return true;
}

void HistEventsObserver::CallFectchEventsInfo(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
    base_logic::ListValue* list = new base_logic::ListValue();
    base_storage::DBStorageEngine* engine =
        (base_storage::DBStorageEngine*) (param);
    MYSQL_ROW rows;
    int32 num = engine->RecordCount();
    if (num > 0) {
      while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
        base_logic::DictionaryValue* info_value =
            new base_logic::DictionaryValue();
        if (rows[0] != NULL)
          info_value->SetString(L"event_name", rows[0]);
        if (rows[2] != NULL)
          info_value->SetBigInteger(L"start_time", (int64)atoll(rows[2]));
        if (rows[3] != NULL)
          info_value->SetBigInteger(L"end_time", (int64) atoll(rows[3]));
        if (rows[4] != NULL)
          info_value->SetString(L"related_stocks", rows[4]);
        if (rows[5] != NULL)
          info_value->SetString(L"stock_weights", rows[5]);
        if (rows[6] != NULL)
          info_value->SetInteger(L"is_valid", (int) atoi(rows[6]));
        list->Append((base_logic::Value*) (info_value));
      }
    }
    dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void HistEventsObserver::ValueSerialization(base_logic::DictionaryValue* dict) {
  std::string event_name;
  int64 start_time = 0;
  int64 end_time = 0;
  std::string related_stocks_str;
  int valid = 0;
  dict->GetString(L"event_name", &event_name);
  dict->GetBigInteger(L"start_time", &start_time);
  std::string start_time_str;
  LOG_MSG2("event_name=%s,start_time=%lld", event_name.c_str(), start_time);
  DateUtil::instance()->int64timestampToDate(start_time, start_time_str);
  dict->GetBigInteger(L"end_time", &end_time);
  std::string end_time_str;
  DateUtil::instance()->int64timestampToDate(end_time, end_time_str);
  dict->GetString(L"related_stocks", &related_stocks_str);
  dict->GetInteger(L"is_valid", &valid);
  EventInfo& event_info = this->event_infos_[event_name];
  event_info.event_name_ = event_name;
  event_info.start_time_ = start_time;
  event_info.start_time_str_ = start_time_str;
  event_info.end_time_ = end_time;
  event_info.end_time_str_ = end_time_str;
  std::string split_chars = ",";
  StockUtil::Instance()->stock_split(related_stocks_str, split_chars, event_info.related_stocks_);
}

bool HistEventsObserver::getEventStocksByDate(std::string date, std::vector<std::string>& stock_codes) {
  std::map<std::string, EventInfo>::iterator iter =
      this->event_infos_.begin();
  for (; iter != this->event_infos_.end(); iter++) {
    EventInfo& event_info = iter->second;
    if (event_info.start_time_str_ > date || event_info.end_time_str_ < date) {
      continue;
    }
    for (int i = 0; i < event_info.related_stocks_.size(); i++) {
      stock_codes.push_back(event_info.related_stocks_[i]);
    }
  }
  return true;
}

bool HistEventsObserver::getEventStocksByDate(std::string date, std::set<std::string>& stock_codes) {
  std::vector<std::string> stocks_vec;
  this->getEventStocksByDate(date, stocks_vec);
  for (int i = 0; i < stocks_vec.size(); i++) {
    stock_codes.insert(stocks_vec[i]);
  }
  return true;
}

} /* namespace stock_logic */

