#include "logic/logic_comm.h"
#include "basic/template.h"
#include "stock_user_mgr.h"

#include "stock_proto_buf.h"
#include "thread/base_thread_lock.h"

namespace stock_logic {

StockUserManager* StockUserEngine::stock_user_mgr_ = NULL;
StockUserEngine* StockUserEngine::stock_user_engine_ = NULL;

StockUserManager::StockUserManager()
    : stock_db_(NULL) {
  stock_user_cache_ = new StockUserCache();
  Init();
  old_yield_data_deleted = false;
}

StockUserManager::~StockUserManager() {
  DeinitThreadrw(lock_);
}

void StockUserManager::Init() {
  InitThreadrw(&lock_);
}

void StockUserManager::Init(stock_logic::StockDB* vip_db) {
  stock_db_ = vip_db;
  stock_db_->FectchStockBasicInfo(stock_user_cache_->stock_total_info_);
  for (STOCKINFO_MAP::iterator it =
      stock_user_cache_->stock_total_info_.begin();
      it != stock_user_cache_->stock_total_info_.end(); it++) {
    stock_db_->FectchStockHistData(it->first,
                                   stock_user_cache_->stock_total_info_);
  }
  stock_db_->FectchIndustryInfo(stock_user_cache_->industry_info_);
  stock_db_->FectchEventsInfo(stock_user_cache_->industry_info_);
  for (std::map<std::string, BasicIndustryInfo>::iterator it = stock_user_cache_
      ->industry_info_.industry_info_map_.begin();
      it != stock_user_cache_->industry_info_.industry_info_map_.begin();
      it++) {
    std::string industry_name = it->first;
    BasicIndustryInfo& basic_industry_info = it->second;
    for (std::map<std::string, double>::iterator iter = basic_industry_info
        .stock_price_info_.begin();
        iter != basic_industry_info.stock_price_info_.end(); iter++) {
      stock_user_cache_->stock_total_info_[iter->first].add_stock_industry(
          it->first);
    }
  }
  //OnUpdateRealtimeStockInfo();
  UpdateRealtimeStockInfo();

  //int current_trade_time = 0;
  //UpdateIndustryPriceInfo(current_trade_time);
  UpdateIndustryVolume();
  //UpdateIndustryJson();
  UpdateIndustryMarketValue();
  UpdateStockKLine();
  UpdateAllRealtimeStockInfo();
  stock_db_->FectchStockShareInfo(stock_user_cache_->stock_total_info_);
  stock_db_->FectchStockHoldersInfo(stock_user_cache_->stock_total_info_);
  //UpdateRealtimeStockInfo();
}

void StockUserManager::UpdateStockHistData() {
  base_logic::WLockGd lk(lock_);
  for (STOCKINFO_MAP::iterator it =
      stock_user_cache_->stock_total_info_.begin();
      it != stock_user_cache_->stock_total_info_.end(); it++) {
    stock_db_->FectchStockHistData(it->first,
                                   stock_user_cache_->stock_total_info_);
    it->second.hist_data_info_.CountSupportPressurePos();
    it->second.hist_data_info_.CountQFQYield();
  }
  stock_user_cache_->set_stock_data_inited(true);
}

void StockUserManager::UpdateIndustryHistData() {
  base_logic::WLockGd lk(lock_);
  std::map<std::string, BasicIndustryInfo>& industry_map = stock_user_cache_
      ->industry_info_.industry_info_map_;
  std::map<std::string, BasicIndustryInfo>::iterator iter =
      industry_map.begin();
  for (; iter != industry_map.end(); iter++) {
    BasicIndustryInfo& basic_industry_info = iter->second;
    //if (1 != basic_industry_info.type())
    //	continue;
    basic_industry_info.update_hist_data();
  }
}

bool StockUserManager::UpdateStockDayKLineData() {
  base_logic::WLockGd lk(lock_);
  std::string latest_time = stock_user_cache_->max_hist_data_time();
  return stock_db_->FectchStockDayKLineData(
      latest_time, stock_user_cache_->stock_total_info_);
}

void StockUserManager::UpdateRealtimeStockInfo() {
  base_logic::WLockGd lk(lock_);
  LOG_MSG("UpdateRealtimeStockInfo");
  //if (StockUtil::Instance()->is_trading_time())
  stock_db_->UpdateRealtimeStockInfo(stock_user_cache_->stock_total_info_);
}

void StockUserManager::UpdateAllRealtimeStockInfo() {
  FuncTimeCount test_time("UpdateAllRealtimeStockInfo");
  base_logic::WLockGd lk(lock_);
  LOG_MSG("UpdateAllRealtimeStockInfo");
  stock_db_->UpdateALLRealtimeStockInfo(stock_user_cache_->stock_total_info_);
}

void StockUserManager::UpdateWeekMonthData() {
  base_logic::WLockGd lk(lock_);
  stock_db_->UpdateWeekMonthData(stock_user_cache_->stock_total_info_);
}

void StockUserManager::GetLimitData(std::string& market_limit_info_str,
                                    std::string format) {
  base_logic::WLockGd lk(lock_);
  /*if ("jsonp" == format)
   market_limit_info_str = stock_user_cache_->cached_json_info_.market_limit_jsonp();
   else*/
  market_limit_info_str =
      stock_user_cache_->cached_json_info_.market_limit_json();
}

void StockUserManager::GetHotDiagramIndustryData(
    std::list<HotDiagramPerIndustry>& hot_diagram_industry_data) {
  base_logic::WLockGd lk(lock_);

}

void StockUserManager::GetHotDiagramDataByIndustry(
    std::list<HotDiagramPerStock>& hot_diagram_data_by_industry) {
  base_logic::WLockGd lk(lock_);
}

void StockUserManager::UpdateLimitData() {
  LOG_MSG("UpdateLimitData");
  //base_logic::WLockGd lk(lock_);
  LOG_MSG("UpdateLimitData lock");
  stock_user_cache_->market_limit_info_.clear();
  //stock_user_cache_->cached_json_info_.clear_list();
  stock_db_->GetLimitData(stock_user_cache_->market_limit_info_);
  MARKET_LIMIT& market_limit_info = stock_user_cache_->market_limit_info_;
  stock_logic::net_reply::VIPNewsList* vip_list =
      new stock_logic::net_reply::VIPNewsList();
  MARKET_LIMIT::iterator iter = market_limit_info.begin();
  for (; iter != market_limit_info.end(); iter++) {
    stock_logic::net_reply::VIPLimitData* limit_data =
        new stock_logic::net_reply::VIPLimitData();
    limit_data->set_time(iter->first);
    limit_data->set_surged_stock_num(iter->second[0]);
    limit_data->set_decline_stock_num(iter->second[1]);
    base_logic::Value* limit_data_value = limit_data->get();
    vip_list->set_vip_news(limit_data_value);
    //stock_user_cache_->cached_json_info_.add_value_to_list(limit_data_value);
    delete limit_data;
    limit_data = NULL;
  }
  base_logic::DictionaryValue* vip_list_packet = vip_list->packet();
  stock_user_cache_->cached_json_info_.market_limit_json_ = "";
  StockUtil::Instance()->serialize(
      vip_list_packet, stock_user_cache_->cached_json_info_.market_limit_json_);
  delete vip_list_packet;
  vip_list_packet = NULL;
  delete vip_list;
  vip_list = NULL;
  LOG_MSG2("packet finished market_limit_info.size=%d", market_limit_info.size());
}

void StockUserManager::WriteLimitData(int& trade_time) {
  if (!StockUtil::Instance()->is_trading_time())
    return;
  int current_trade_time = 0;
  LOG_MSG2("time write limit data trade_time=%d", trade_time);
  base_logic::WLockGd lk(lock_);
  STOCKINFO_MAP& stock_total_info = stock_user_cache_->stock_total_info_;
  STOCKINFO_MAP::iterator iter = stock_total_info.begin();
  int surged_count = 0;
  int decline_count = 0;
  for (; iter != stock_total_info.end(); iter++) {
    std::string code = iter->first;
    StockBasicInfo& basic_info = iter->second.basic_info_;
    if (0 == current_trade_time) {
      current_trade_time = basic_info.current_trade_time();
      trade_time = (int) (current_trade_time / 60) * 60;
    }
    double current_trade = basic_info.current_trade();
    double current_open = basic_info.current_open();
    //double current_price_diff = current_trade - current_open;
    //double yield = current_price_diff/current_open;
    double yield = basic_info.change_percent();
    basic_info.add_yield_info(trade_time, current_trade, yield);
    if (code == "hs300") {
      LOG_MSG2("code=%s,trade_time_at_0_second=%d,current_trade=%f,yield=%f,current_open=%f",
          code.c_str(),trade_time, current_trade, yield, current_open);
    }
    basic_info.check_limit();
    if (basic_info.is_decline_limit()) {
      decline_count += 1;
    } else if (basic_info.is_surged_limit()) {
      surged_count += 1;
    }
  }
  if (current_trade_time == 0)
    return;
  stock_db_->WriteLimitData(trade_time, surged_count, decline_count);
}

void StockUserManager::UpdateStockVisitData(int& trade_time) {
  LOG_MSG("UpdateStockVisitData");
  base_logic::WLockGd lk(lock_);
  //if (!StockUtil::Instance()->is_trading_time())
  //    return;
  int max_visit_time = trade_time;
  int min_visit_time = stock_user_cache_->min_visit_data_time();
  stock_db_->FectchStockVisitData(min_visit_time,
                                  max_visit_time,
                                  stock_user_cache_->stock_total_info_);
}

void StockUserManager::UpdateEventsYield(int current_trade_time) {
  base_logic::WLockGd lk(lock_);
}

void StockUserManager::DeleteOldEventsData() {
  base_logic::WLockGd lk(lock_);
  STOCKINFO_MAP& stock_total_info = stock_user_cache_->stock_total_info_;
  STOCKINFO_MAP::iterator iter = stock_total_info.begin();
  for (; iter != stock_total_info.end(); iter++) {
    StockBasicInfo& basic_info = iter->second.basic_info_;
    StockUtil::Instance()->delete_old_yield_info(basic_info.yield_infos_);
  }
  INDUSTRYINFO_MAP& industry_info = stock_user_cache_->industry_info_;
  std::map<std::string, BasicIndustryInfo>& basic_industry_infos = industry_info
      .industry_info_map_;
  std::map<std::string, BasicIndustryInfo>::iterator basic_industry_iter =
      basic_industry_infos.begin();
  for (; basic_industry_iter != basic_industry_infos.end();
      basic_industry_iter++) {
    StockUtil::Instance()->delete_old_yield_info(
        basic_industry_iter->second.industry_yield_infos_);
  }

}

void StockUserManager::UpdateStockHotDiagram() {
  base_logic::WLockGd lk(lock_);
  /*CachedJsonInfo& cached_json = vip_user_cache_->cached_json_info_;
   cached_json.clear();
   std::map<std::string, STOCKS_PER_INDUSTRY_JSON>& stocks_map = cached_json.stocks_per_industry_json_;
   std::map<std::string, std::string>& industry_map = cached_json.industry_json_;
   vip_user_cache_->industry_info_.get_day_hottest_industry(industry_map);
   std::list<BasicIndustryInfo> industry_list;
   for ()

   vip_logic::net_reply::VIPNewsList* vip_list = new vip_logic::net_reply::VIPNewsList();
   MARKET_LIMIT::iterator iter = vip_user_cache_->cached_json_info_.stocks_per_industry_json_.begin();
   for (; iter != vip_user_cache_->cached_json_info_.stocks_per_industry_json_.end(); iter++) {
   STOCKS_PER_INDUSTRY_JSON& stocks_json_map = iter->second;
   STOCKS_PER_INDUSTRY_JSON::iterator it = stocks_json_map.begin();
   for ()
   vip_logic::net_reply::STOIndustryData* stock_data = new  vip_logic::net_reply::STOIndustryData();
   stock_data->set_code(iter->)
   limit_data->set_time(iter->first);
   limit_data->set_surged_stock_num(iter->second[0]);
   limit_data->set_decline_stock_num(iter->second[1]);
   vip_list->set_vip_news(limit_data->get());
   }

   std::string stocks_hot_diagram_json;
   StockUtil::Instance()->serialize(vip_list->packet(), stocks_hot_diagram_json);*/
}

void StockUserManager::UpdateOfflineVisitData() {
  base_logic::WLockGd lk(lock_);
  std::string max_date = stock_user_cache_->max_offline_visit_data_date();
  STOCKINFO_MAP& stock_total_info = stock_user_cache_->stock_total_info_;
  stock_db_->FecthOffLineVisitData(max_date,
                                   stock_total_info);
}

std::string StockUserManager::GetIndustryHotDiagram(std::string type,
                                                    std::string format) {
  base_logic::RLockGd lk(lock_);
  //std::string industry_json = vip_user_cache_->cached_json_info_.industry_json(type);
  std::string industry_json = stock_user_cache_->industry_info_
      .get_industry_hot_diagram_by_type(type, format);
  return industry_json;
}

std::string StockUserManager::GetEventHotDiagram() {
  base_logic::RLockGd lk(lock_);
  std::string event_json = "test";
  event_json = stock_user_cache_->industry_info_.get_event_hot_diagram();
  return event_json;
}

bool StockUserManager::GetYieldJsonByName(std::string& cycle_type,
                                          std::string& start_date,
                                          std::string& end_date,
                                          std::string& industry_name,
                                          std::string& yield_json,
                                          std::string& name) {
  INDUSTRYINFO_MAP& industry_info = stock_user_cache_->industry_info_;
  if (industry_info.check_event(industry_name)) {
    BasicIndustryInfo& basic_info = industry_info.get_industry_by_name(
        industry_name);
    if ("current" == cycle_type) {
      std::map<int, YieldInfoUnit>& hs300_yield_data = stock_user_cache_
          ->get_hs300_yield_data();
      return basic_info.get_chart_json(yield_json,
                                       hs300_yield_data,
                                       name);
    } else {
      std::map<std::string, HistDataPerDay>& hs300_hist_data = stock_user_cache_
          ->get_hs300_hist_data();
      return basic_info.get_hist_data_json(hs300_hist_data,
                                           start_date,
                                           end_date,
                                           yield_json);
    }
  } else
    return false;
}

std::string StockUserManager::GetStocksHotDiagram(std::string type,
                                                  std::string industry_name) {
  base_logic::RLockGd lk(lock_);
  //std::string stocks_json = vip_user_cache_->cached_json_info_.get_stocks_json(type, industry_name);
  BasicIndustryInfo& industry_info = stock_user_cache_->industry_info_
      .get_industry_by_name(industry_name);
  std::string stocks_json = industry_info.get_hot_diagram_by_type(type);
  return stocks_json;
}

void StockUserManager::UpdateIndustryYieldInfo(int& current_trade_time) {
  LOG_MSG2("UpdateIndustryYieldInfo,current_trade_time=%d",
      current_trade_time);
  base_logic::WLockGd lk(lock_);
  std::string first_day_of_week =
      StockUtil::Instance()->get_first_day_of_week();
  std::string first_day_of_month =
      StockUtil::Instance()->get_first_day_of_month();
  std::map<std::string, BasicIndustryInfo>& industry_map = stock_user_cache_
      ->industry_info_.industry_info_map_;
  std::map<std::string, BasicIndustryInfo>::iterator iter =
      industry_map.begin();
  for (; iter != industry_map.end(); iter++) {
    std::string industry_name = iter->first;
    double industry_changepoint = 0;
    BasicIndustryInfo& basic_industry_info = iter->second;
    std::map<std::string, double>::iterator it = basic_industry_info
        .stock_price_info_.begin();
    for (; it != basic_industry_info.stock_price_info_.end(); it++) {
      std::string code = it->first;
      StockTotalInfo& stock_total_info =
          stock_user_cache_->stock_total_info_[code];
      StockBasicInfo& stock_basic_info = stock_total_info.basic_info_;
      //if(first_day_of_week > stock_basic_info.week_trade_day())
      //	continue;
      double stock_market_value = stock_basic_info.market_value();
      double stock_yield = stock_basic_info.get_yield_by_time(
          current_trade_time);
      industry_changepoint += stock_yield * stock_market_value
          / basic_industry_info.industry_market_value();
    }
    basic_industry_info.set_industry_changepoint(industry_changepoint);
    if (0 != current_trade_time)
      basic_industry_info.add_industry_yield_info(current_trade_time,
                                                  industry_changepoint);
  }
}

void StockUserManager::UpdateIndustryPriceInfo(int& current_trade_time) {
  LOG_MSG("UpdateIndustryPriceInfo");
  base_logic::WLockGd lk(lock_);
  std::string first_day_of_week =
      StockUtil::Instance()->get_first_day_of_week();
  std::string first_day_of_month =
      StockUtil::Instance()->get_first_day_of_month();
  std::map<std::string, BasicIndustryInfo>& industry_map = stock_user_cache_
      ->industry_info_.industry_info_map_;
  std::map<std::string, BasicIndustryInfo>::iterator iter =
      industry_map.begin();
  for (; iter != industry_map.end(); iter++) {
    std::string industry_name = iter->first;
    double industry_changepoint = 0;
    double week_industry_changepoint = 0;
    double month_industry_changepoint = 0;
    BasicIndustryInfo& basic_industry_info = iter->second;
    std::map<std::string, double>::iterator it = basic_industry_info
        .stock_price_info_.begin();
    for (; it != basic_industry_info.stock_price_info_.end(); it++) {
      std::string code = it->first;
      StockTotalInfo& stock_total_info =
          stock_user_cache_->stock_total_info_[code];
      StockBasicInfo& stock_basic_info = stock_total_info.basic_info_;
      //if(first_day_of_week > stock_basic_info.week_trade_day())
      //	continue;
      double stock_market_value = stock_basic_info.market_value();
      industry_changepoint += stock_basic_info.change_percent()
          * stock_market_value / basic_industry_info.industry_market_value();
      week_industry_changepoint += stock_basic_info.week_change_percent()
          * stock_market_value / basic_industry_info.industry_market_value();
      month_industry_changepoint += stock_basic_info.month_change_percent()
          * stock_market_value / basic_industry_info.industry_market_value();
    }
    basic_industry_info.set_industry_changepoint(industry_changepoint);
    basic_industry_info.set_industry_week_changepoint(
        week_industry_changepoint);
    basic_industry_info.set_industry_month_changepoint(
        month_industry_changepoint);
    if (0 != current_trade_time)
      basic_industry_info.add_industry_yield_info(current_trade_time,
                                                  industry_changepoint);
  }
}

void StockUserManager::UpdateIndustryVisitData(int& current_trade_time) {
  LOG_MSG2("UpdateIndustryVisitDatacurrent_trade_time=%d", current_trade_time);
  current_trade_time = (current_trade_time / 60) * 60;
  //base_logic::WLockGd lk(lock_);
  std::map<std::string, BasicIndustryInfo>& industry_map = stock_user_cache_
        ->industry_info_.industry_info_map_;
    std::map<std::string, BasicIndustryInfo>::iterator iter =
        industry_map.begin();
    for (; iter != industry_map.end(); iter++) {
      int industry_visit = 0;
      bool industry_has_visit = false;
      std::string industry_name = iter->first;
      BasicIndustryInfo& basic_industry_info = iter->second;
      std::map<std::string, double>::iterator it = basic_industry_info
          .stock_price_info_.begin();
      for (; it != basic_industry_info.stock_price_info_.end(); it++) {
        std::string code = it->first;
        StockTotalInfo& stock_total_info =
            stock_user_cache_->stock_total_info_[code];
        StockBasicInfo& stock_basic_info = stock_total_info.basic_info_;
        int stock_visit_count = 0;
        if(!stock_basic_info.get_visit_by_time(current_trade_time, stock_visit_count))
          continue;
        else {
          industry_visit += stock_visit_count;
        }
        if (code == "601288" || code == "000002") {
          LOG_MSG2("code=%s,current_trade_time=%d,stock_visit_count=%d",
                   code.c_str(),
                   current_trade_time,
                   stock_visit_count);
        }
      }
      basic_industry_info.set_industry_visit_data(current_trade_time, industry_visit);
    }
}

void StockUserManager::UpdateIndustryVolume() {
  LOG_MSG("UpdateIndustryVolume");
  base_logic::WLockGd lk(lock_);
  std::string first_day_of_week =
      StockUtil::Instance()->get_first_day_of_week();
  std::string first_day_of_month =
      StockUtil::Instance()->get_first_day_of_month();
  std::map<std::string, BasicIndustryInfo>& industry_map = stock_user_cache_
      ->industry_info_.industry_info_map_;
  std::map<std::string, BasicIndustryInfo>::iterator iter =
      industry_map.begin();
  for (; iter != industry_map.end(); iter++) {
    std::string industry_name = iter->first;
    double industry_volume = 0;
    double week_industry_volume = 0;
    double month_industry_volume = 0;
    BasicIndustryInfo& basic_industry_info = iter->second;
    std::map<std::string, double>::iterator it = basic_industry_info
        .stock_price_info_.begin();
    for (; it != basic_industry_info.stock_price_info_.end(); it++) {
      std::string code = it->first;
      StockTotalInfo& stock_total_info =
          stock_user_cache_->stock_total_info_[code];
      StockBasicInfo& stock_basic_info = stock_total_info.basic_info_;
      //if(first_day_of_week > stock_basic_info.week_trade_day())
      //	continue;
      double stock_market_value = stock_basic_info.market_value();
      industry_volume += stock_basic_info.volume();
      week_industry_volume += stock_basic_info.week_volume();
      month_industry_volume += stock_basic_info.month_volume();
    }
    basic_industry_info.set_industry_volume(industry_volume);
    basic_industry_info.set_week_industry_volume(week_industry_volume);
    basic_industry_info.set_month_industry_volume(month_industry_volume);
  }
}

void StockUserManager::UpdateIndustryJson() {
  base_logic::WLockGd lk(lock_);
  stock_user_cache_->industry_info_.update_hottest_industry();
}

void StockUserManager::UpdateStockKLine() {
  LOG_MSG("UpdateStockKLine");
  base_logic::WLockGd lk(lock_);
  STOCKINFO_MAP& stocks_map = stock_user_cache_->stock_total_info_;
  STOCKINFO_MAP::iterator stocks_iter = stocks_map.begin();
  for (; stocks_iter != stocks_map.end(); stocks_iter++) {
    if ("hs300" == stocks_iter->first) {
      LOG_MSG2("code=%s", stocks_iter->first.c_str());
    }
    stocks_iter->second.update_kline_json();
  }
}

void StockUserManager::UpdateEventsData() {
  LOG_MSG("UpdateEventsData");
  base_logic::WLockGd lk(lock_);
  stock_db_->FectchEventsInfo(stock_user_cache_->industry_info_);
}

void StockUserManager::UpdateYieldDataFromDB() {
  LOG_MSG("UpdateYieldDataFromDB");
  base_logic::WLockGd lk(lock_);
  int start_time = (int) time(NULL);
  stock_db_->FetchYieldData(stock_user_cache_->stock_total_info_);
  int end_time = (int) time(NULL);
  LOG_MSG2("UpdateYieldDataFromDBstart_time=%d,end_time=%d,time_used=%d",
      start_time, end_time, start_time - end_time);
}

void StockUserManager::DeleteOldYieldData() {
  LOG_MSG("DeleteOldYieldData");
  base_logic::WLockGd lk(lock_);
  if (!StockUtil::Instance()->is_trading_time()) {
    old_yield_data_deleted = false;
    return;
  }
  if (old_yield_data_deleted)
    return;
  int current_time = (int) time(NULL);
  int end_time = current_time - 10 * 3600;
  stock_db_->DeleteOldYieldInfo(end_time);
  stock_user_cache_->clear_yield_info(end_time);
  old_yield_data_deleted = true;
}

void StockUserManager::LoadCustomEvent() {
  LOG_MSG("LoadCustomEvent");
  base_logic::WLockGd lk(lock_);
  INDUSTRYINFO_MAP& industry_info = stock_user_cache_->industry_info_;
  stock_db_->LoadCustomEvent(industry_info);
}

void StockUserManager::UpdateYieldDataToDB() {
  LOG_MSG("UpdateYieldDataToDB");
  if (StockUtil::Instance()->is_trading_time())
    return;
  if (StockUtil::Instance()->get_current_hour() > 1)
    return;
  base_logic::RLockGd lk(lock_);
  STOCKINFO_MAP& stocks_map = stock_user_cache_->stock_total_info_;
  STOCKINFO_MAP::iterator stocks_iter = stocks_map.begin();
  for (; stocks_iter != stocks_map.end(); stocks_iter++) {
    std::string code = stocks_iter->first;
    stock_logic::StockTotalInfo& total_info = stocks_iter->second;
    std::map<int, YieldInfoUnit>& yield_map = total_info.basic_info_
        .get_yield_infos();
    stock_db_->BatchUpdateYieldInfo(code, yield_map);
    /*std::map<int, YieldInfoUnit>::iterator yield_iter = yield_map.begin();
     for (; yield_iter != yield_map.end(); yield_iter++) {
     int trade_time = yield_iter->first;
     double yield = yield_iter->second.yield_;
     stock_db_->UpdateYieldInfo(code, trade_time, yield);
     }*/
  }
  std::map<std::string, BasicIndustryInfo>& industry_info = stock_user_cache_
      ->industry_info_.industry_info_map_;
  std::map<std::string, BasicIndustryInfo>::iterator industry_iter =
      industry_info.begin();
  for (; industry_iter != industry_info.end(); industry_iter++) {
    BasicIndustryInfo& basic_info = industry_iter->second;
    //if (1 != basic_info.type())
    //	continue;
    std::string industry_name = industry_iter->first;
    std::map<int, YieldInfoUnit>& industry_yields = basic_info
        .industry_yield_infos_;
    std::map<int, YieldInfoUnit>::iterator industry_yield_iter = industry_yields
        .begin();
    for (; industry_yield_iter != industry_yields.end();
        industry_yield_iter++) {
      stock_db_->UpdateYieldInfo(industry_name,
                                 industry_yield_iter->second.trade_time_,
                                 industry_yield_iter->second.yield_);
    }
  }
}

std::string StockUserManager::GetStockKLineByCode(std::string stock_code,
                                                  std::string format,
                                                  std::string& cycle_type,
                                                  std::string& start_date,
                                                  std::string& end_date,
                                                  std::string& name) {
  LOG_MSG("GetStockKLineByCode");
  base_logic::RLockGd lk(lock_);
  LOG_MSG("GetStockKLineByCode");
  std::string r_json = "";
  STOCKINFO_MAP& stocks_map = stock_user_cache_->stock_total_info_;
  STOCKINFO_MAP::iterator iter = stocks_map.find(stock_code);
  if (stocks_map.end() != iter) {
    stock_logic::StockTotalInfo& total_info = iter->second;
    if ("jsonp" == format)
      return iter->second.getKLineJsonp();
    else if ("current" == cycle_type) {
      std::map<int, YieldInfoUnit>& hs300_yield_data = stocks_map[HSSANBAI]
          .basic_info_.yield_infos_;
      return iter->second.getCharmJson(hs300_yield_data, name);
    } else if ("bydate" == cycle_type) {
      std::map<std::string, HistDataPerDay>& hs300_hist_data = stock_user_cache_
          ->get_hs300_hist_data();
      StockUtil::Instance()->get_json_of_hist_data(
          hs300_hist_data, start_date,
          end_date,
          total_info.hist_data_info_.stock_hist_data_, r_json);
      return r_json;
    } else
      return iter->second.getLineJson();
  } else
    return "null";
}

void StockUserManager::UpdateIndustryMarketValue() {
  base_logic::WLockGd lk(lock_);
  std::map<std::string, BasicIndustryInfo>& industry_map = stock_user_cache_
      ->industry_info_.industry_info_map_;
  std::map<std::string, BasicIndustryInfo>::iterator iter =
      industry_map.begin();
  for (; iter != industry_map.end(); iter++) {
    BasicIndustryInfo& basic_industry_info = iter->second;
    double industry_market_value = 0.0;
    std::map<std::string, double>::iterator it = basic_industry_info
        .stock_price_info_.begin();
    for (; it != basic_industry_info.stock_price_info_.end(); it++) {
      std::string code = it->first;
      StockTotalInfo& stock_total_info =
          stock_user_cache_->stock_total_info_[code];
      StockBasicInfo& stock_basic_info = stock_total_info.basic_info_;
      double stock_market_value = stock_basic_info.market_value();
      /*if ("酿酒食品" == iter->first) {
       LOG_MSG2("industry_name=%s,code=%s,market_value=%f,industry_market_value=%f",
       iter->first.c_str(),code.c_str(),stock_market_value,industry_market_value);
       }*/
      industry_market_value += stock_market_value;
    }
    basic_industry_info.set_industry_market_value(industry_market_value);
  }
}

std::string& StockUserCache::max_offline_visit_data_date() {
  return max_offline_visit_data_date_;
}

void StockUserCache::set_max_offline_visit_data_date(std::string date) {
  max_offline_visit_data_date_ = date;
}

void StockUserCache::update_max_offline_visit_data_Date(std::string date) {
  if (max_offline_visit_data_date_ < date)
    max_offline_visit_data_date_ = date;
}

}

