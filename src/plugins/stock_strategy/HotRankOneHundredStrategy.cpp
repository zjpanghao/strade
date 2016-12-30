/*
 * HotRankOneHundredStrategy.cpp
 *
 *  Created on: 2016年11月21日
 *      Author: Maibenben
 */

#include "HotRankOneHundredStrategy.h"
#include "stock_factory.h"
#include "StockAccountManager.h"
#include "DateUtil.h"
#include "IndustryStockHotRank.h"

namespace stock_logic {

HotRankOneHundredStrategy::HotRankOneHundredStrategy(stock_logic::Subject* subject) {
  subject_ = subject;
  name_ = "HotRankOneHundredStrategy";
  rank_index_ = 0;
}

HotRankOneHundredStrategy::~HotRankOneHundredStrategy() {
  // YGTODO Auto-generated destructor stub
}

bool countVisitNumByDate(
    std::map<std::string, DataPerDay>& visit_datas,
    std::string& date,
    int& count) {
  std::map<std::string, DataPerDay>::iterator iter =
      visit_datas.begin();
  DataPerDay data_pre;
  DataPerDay data;
  for (; iter != visit_datas.end(); iter++) {
    std::string temp_date = iter->first;
    if (temp_date < date)
      data_pre = iter->second;
    else if (temp_date == date){
      data = iter->second;
      break;
    }
  }
  count = 0;
  for (int i = 0; i < 9; i++) {
    count += data.data_per_hour_[i].visit_per_hour_num_;
  }
  for (int i = 9; i < 23; i++) {
    count += data.data_per_hour_[i].visit_per_hour_num_;
  }
  return true;
}

bool countVisitByDate(
    std::map<std::string, DataPerDay>& visit_datas,
    std::string& date,
    int& count) {
  std::map<std::string, DataPerDay>::iterator iter =
      visit_datas.begin();
  DataPerDay data_pre;
  DataPerDay data;
  for (; iter != visit_datas.end(); iter++) {
    std::string temp_date = iter->first;
    if (temp_date < date)
      data_pre = iter->second;
    else if (temp_date == date){
      data = iter->second;
      break;
    }
  }
  count = 0;
  for (int i = 0; i < 9; i++) {
    count += data.data_per_hour_[i].visit_per_hour_num_;
  }
  for (int i = 9; i < 23; i++) {
    count += data.data_per_hour_[i].visit_per_hour_num_;
  }
  return true;
}

bool HotRankOneHundredStrategy::getGoodStocks(StockAccount* account) {
  LOG_MSG("HotRankOneHundredStrategy choose stock");
  std::string birthday = account->getBirthday();
  LOG_MSG2("birthday=%s", birthday.c_str());
  factory_ = (StockFactory*)subject_;
  std::list<IndustryStockRankUnit> rank_list;
  STOCKINFO_MAP& stock_map = factory_->GetStockInfoMap();
  STOCKINFO_MAP::iterator stocks_iter =
      stock_map.begin();
  for (; stocks_iter != stock_map.end(); stocks_iter++) {
    IndustryStockRankUnit unit;
    std::string stock_code =stocks_iter->first;
    StockBasicInfo& basic_info =
        factory_->GetBasicInfoByCode(stock_code);
    DataPerDay& data_per_day = basic_info.data_per_day_[birthday];
    /*for (int i = 0; i < 13; i++) {
      unit.visit_num_ += data_per_day.data_per_hour_[i].visit_per_hour_num_;
    }*/
    //unit.visit_num_ = basic_info.data_per_day_[birthday].visit_per_day_num_;
    countVisitByDate(basic_info.data_per_day_, birthday, unit.visit_num_);
    unit.stock_code_ = stock_code;
    unit.date_ = birthday;
    rank_list.push_back(unit);
    if (birthday > "2016-01-01" && birthday < "2016-02-01" && unit.visit_num_ > 0)
      LOG_MSG2("birthday=%s,unit.visit_num=%d,stock_code=%s",
               birthday.c_str(),
               unit.visit_num_,
               unit.stock_code_.c_str());
  }
  rank_list.sort(IndustryStockRankUnit::cmp);

  if (rank_index_ > rank_list.size() - 1)
    return false;
  std::list<IndustryStockRankUnit>::iterator iter =
      rank_list.begin();
  int count = 0;
  int temp_index = 0;
  int max_count = 100;
  if (rank_index_ == 10) {
    rank_index_ = 0;
    max_count = 10;
  }
  else if (rank_index_ == 50) {
    rank_index_ = 0;
    max_count = 50;
  }
  for (; iter != rank_list.end(); iter++) {
    /*LOG_MSG2("IndustryStockRankUnit:date=%s,stock=%s,visit_num=%d",
             birthday.c_str(),
             iter->stock_code_.c_str(),
             iter->visit_num_);*/
    if (temp_index < rank_index_) {
      temp_index++;
      continue;
    }
    if (count > max_count - 1) {
      continue;
    }
    std::string stock_code = iter->stock_code_;
    HistDataPerDay hist_data;
    if (factory_->GetHistDataByDate(stock_code, birthday, hist_data)) {

      double open_price = hist_data.open_;
      double close_price = hist_data.close_/(1 + hist_data.day_yield_);
      double open_price_change = 0;
      if (close_price > 1)
        open_price_change = (open_price - close_price)/close_price;
      if (open_price_change > 0.98)
        continue;
      if (stock_code == "hs300")
        continue;

      HoldingStock stock_info;
      this->setStockInfo(stock_info);
      stock_info.setStockCode(stock_code);
      stock_info.setBuyDate(birthday);
      stock_info.setBuyPrice(hist_data.open_);
      stock_info.setCurrentPrice(hist_data.open_);
      stock_info.setMaxPriceInHoldDays(hist_data.high_);
      stock_info.setMinPriceInHoldDays(hist_data.low_);
      stock_info.current_date_ = birthday;
      stock_info.update();
      account->addStock(stock_code, stock_info);
      count++;
    }
    else
      continue;
  }
  return true;
}

bool HotRankOneHundredStrategy::setStockInfo(HoldingStock& stock_info) {
  //stock_info.setHoldDays(7);
  stock_info.setStopRatio(stop_ratio_);
  stock_info.setLimitedRatio(sell_ratio_);
  return true;
}

void HotRankOneHundredStrategy::Update(int opcode, stock_logic::Subject* subject) {

}

} /* namespace stock_logic */
