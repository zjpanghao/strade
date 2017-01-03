/*
 * OffLineVisitStockObserver.cpp
 *
 *  Created on: 2016年8月5日
 *      Author: Maibenben
 */

#include "OffLineVisitStockObserver.h"
#include "stock_basic_info.h"
#include "stock_logic.h"
#include "stock_factory.h"
#include "stock_user_mgr.h"

namespace stock_logic {

OffLineVisitStockObserver::OffLineVisitStockObserver(stock_logic::Subject* subject) {
  set_name("OffLineVisitStockObserver");
  subject_ = subject;
}

OffLineVisitStockObserver::~OffLineVisitStockObserver() {
  // YGTODO Auto-generated destructor stub
}

base_logic::DictionaryValue* OffLineVisitStockObserver::Request(base_logic::DictionaryValue* params) {
  std::string start_date = ",2016-07-01";
  bool r = true;
  r = params->GetString(L"start_date", &start_date);
  if (true == r) {
    LOG_MSG2("get start_date=%s", start_date.c_str());
  }
  if (start_date.size() > 1)
    start_date.erase(start_date.begin());
  LOG_MSG2("start_date=%s", start_date.c_str());

  base_logic::DictionaryValue* total_dict = new base_logic::DictionaryValue();
  std::map<std::string, VisitInfo>::iterator iter = visit_info_.begin();
  LOG_MSG2("visit_info_.size()=%d", visit_info_.size());
  for (; iter != visit_info_.end(); iter++) {
    std::string date = iter->first;
    VisitInfo& info = iter->second;
    if (date < start_date)
      continue;
    base_logic::DictionaryValue* info_dict = new base_logic::DictionaryValue();
    std::list<StockVisitInfo> stock_visit_info_list;
    std::map<std::string, StockVisitInfo>::iterator stock_visit_info_iter =
        info.stock_visit_info_.begin();
    for (; stock_visit_info_iter != info.stock_visit_info_.end(); stock_visit_info_iter++) {
      stock_visit_info_list.push_back(stock_visit_info_iter->second);
    }
    stock_visit_info_list.sort(StockVisitInfo::cmp);

    std::list<StockVisitInfo>::iterator list_iter = stock_visit_info_list.begin();
    int count = 0;
    for (; list_iter != stock_visit_info_list.end(); list_iter++) {
      base_logic::DictionaryValue* stock_dict = new base_logic::DictionaryValue();
      stock_dict->SetInteger(L"visit_num_", list_iter->visit_num_);
      stock_dict->SetReal(L"change_percent_", list_iter->change_percent_);
      stock_dict->SetReal(L"close", list_iter->close_);
      std::string stock_code = list_iter->stock_code_;
      stock_dict->SetString(L"stock_code", stock_code.c_str());
      info_dict->Set(std::wstring(stock_code.begin(), stock_code.end()), stock_dict);
      count++;
      if (count > 10)
        break;
      LOG_MSG2("stock_code=%s,visit_num=%d,change_percent=%f,date=%s", stock_code.c_str(),
               list_iter->visit_num_,
               list_iter->change_percent_,
               date.c_str());
    }
    total_dict->Set(std::wstring(date.begin(), date.end()), info_dict);
  }
  return total_dict;
}

void OffLineVisitStockObserver::Update(int opcode, stock_logic::Subject* subject) {
  LOG_MSG2("OffLineVisitStockObserver::Update opcode=%d",
           opcode);
  switch(opcode){

    case 0: {
      UpdateStockOfflineVisitData(subject);
      UpdateStockHistData(subject);
      break;
    }
    case UPDATE_STOCK_OFFLINE_VISIT_DATA: {
      UpdateStockOfflineVisitData(subject);
      break;
    }
    default:
    break;
  }
}

void OffLineVisitStockObserver::UpdateStockOfflineVisitData(stock_logic::Subject* subject) {
  StockFactory* factory  = (StockFactory*)subject;
  StockUserCache* cache = factory->stock_usr_mgr_->stock_user_cache_;
  STOCKINFO_MAP& stock_total_map = cache->stock_total_info_;
  STOCKINFO_MAP::iterator stock_total_map_iter = stock_total_map.begin();
  for (; stock_total_map_iter != stock_total_map.end(); stock_total_map_iter++) {
    std::string stock_code = stock_total_map_iter->first;
    std::map<std::string, DataPerDay>& data_per_day =
        stock_total_map_iter->second.basic_info_.data_per_day_;
    CountVisitDataPerDay(stock_code, data_per_day);
  }
}

void OffLineVisitStockObserver::UpdateStockHistData(stock_logic::Subject* subject) {
  LOG_MSG("OffLineVisitStockObserver::UpdateStockHistData");
  StockFactory* factory  = (StockFactory*)subject;
  StockUserCache* cache = factory->stock_usr_mgr_->stock_user_cache_;
  STOCKINFO_MAP& stock_total_map = cache->stock_total_info_;
  STOCKINFO_MAP::iterator stock_total_map_iter = stock_total_map.begin();
  for (; stock_total_map_iter != stock_total_map.end(); stock_total_map_iter++) {
      std::string stock_code = stock_total_map_iter->first;
      std::map<std::string, DataPerDay>& data_per_day =
              stock_total_map_iter->second.basic_info_.data_per_day_;
      std::map<std::string, HistDataPerDay>& stock_hist_data =
          stock_total_map_iter->second.hist_data_info_.stock_hist_data_;
      std::map<std::string, HistDataPerDay>::iterator stock_hist_data_iter =
          stock_hist_data.begin();
      for (; stock_hist_data_iter != stock_hist_data.end(); stock_hist_data_iter++) {
        std::string date = stock_hist_data_iter->first;
        double stock_close = stock_hist_data_iter->second.close_;
        double stock_change_percent = stock_hist_data_iter->second.day_yield_;
        data_per_day[date].changepercent_ = stock_change_percent;
        data_per_day[date].close_ = stock_close;
        visit_info_[date].stock_visit_info_[stock_code].change_percent_ = stock_change_percent;
        visit_info_[date].stock_visit_info_[stock_code].close_ = stock_close;
        if (stock_code == "000001") {
                  LOG_MSG2("date=%s,stock_close=%f,stock_change_percent=%f",
                           date.c_str(),
                           stock_close,
                           stock_change_percent);
        }
      }
  }
}

void OffLineVisitStockObserver::CountVisitDataPerDay(std::string stock_code,
                                                    std::map<std::string, DataPerDay>& data_per_day) {
  int count = 0;
  if (data_per_day.empty()) {
    LOG_MSG2("stock_code=%s no visit data", stock_code.c_str());
    return;
  }
  std::map<std::string, DataPerDay>::iterator iter =
      data_per_day.begin();
  for (; iter != data_per_day.end(); iter++) {
    std::string date = iter->first;
    count = 0;
    for (int i = 0; i < 9; i++) {
      count += iter->second.data_per_hour_[i].visit_per_hour_num_;
    }
    visit_info_[date].add_visit_info(stock_code, count);
  }
}

void VisitInfo::add_visit_info(std::string stock_code,
                               int visit_num) {
  StockVisitInfo& info = stock_visit_info_[stock_code];
  info.stock_code_ = stock_code;
  info.visit_num_ = visit_num;
}

} /* namespace stock_logic */


