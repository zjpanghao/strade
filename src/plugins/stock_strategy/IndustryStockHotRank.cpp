/*
 * IndustryStockHotRank.cpp
 *
 *  Created on: 2016年9月4日
 *      Author: Maibenben
 */

#include "IndustryStockHotRank.h"
#include "stock_factory.h"

#include <list>

namespace stock_logic {

void IndustryStockRank::addList(std::string date, std::list<IndustryStockRankUnit>& rank_list) {
  rank_map_[date] = rank_list;
}

IndustryStockHotRankObserver::IndustryStockHotRankObserver() {
  attr_name_ = "IndustryStockHotRankObserver_rank";
  name_ = "IndustryStockHotRankObserver";
}

IndustryStockHotRankObserver::~IndustryStockHotRankObserver() {
  // YGTODO Auto-generated destructor stub
}

void IndustryStockHotRankObserver::Update(int opcode, stock_logic::Subject* subject) {
  factory_ = (StockFactory*)subject;

  switch(opcode){

  case 0: {
    updateIndustryHotRank();
    break;
  }

  default:
    break;
  }
}

void IndustryStockHotRankObserver::updateIndustryHotRank() {
  std::map<std::string, BasicIndustryInfo>& industry_map =
      factory_->GetIndustryMap();
  std::map<std::string, BasicIndustryInfo>::iterator iter =
      industry_map.begin();
  for (; iter != industry_map.end(); iter++) {
    BasicIndustryInfo& info = iter->second;
    this->UpdateHotRandPerIndustry(info);
  }
}

void IndustryStockHotRankObserver::UpdateHotRandPerIndustry(BasicIndustryInfo& info) {
  IndustryStockRank* rank_attr = NULL;
  rank_attr = (IndustryStockRank*)info.getAttr(attr_name_);
  if (NULL == rank_attr) {
    rank_attr = new IndustryStockRank();
    info.addAttr(attr_name_, rank_attr);
  }
  std::map<std::string, double>& stocks_map = info.stock_price_info_;
  std::map<std::string, HistDataPerDay>& hs300_hist_data =
      factory_->GetHistDataByCode(HSSANBAI);
  std::map<std::string, HistDataPerDay>::iterator hs300_iter =
      hs300_hist_data.begin();
  for (; hs300_iter != hs300_hist_data.end(); hs300_iter++) {
    std::string date = hs300_iter->first;
    std::list<IndustryStockRankUnit> rank_list;
    std::map<std::string, double>::iterator stocks_iter =
        stocks_map.begin();
    for (; stocks_iter != stocks_map.end(); stocks_iter++) {
      IndustryStockRankUnit unit;
      std::string stock_code =stocks_iter->first;
      StockBasicInfo& basic_info =
          factory_->GetBasicInfoByCode(stock_code);
      DataPerDay& data_per_day = basic_info.data_per_day_[date];
      /*for (int i = 0; i < 13; i++) {
        unit.visit_num_ += data_per_day.data_per_hour_[i].visit_per_hour_num_;
      }*/
      //unit.visit_num_ = basic_info.data_per_day_[date].visit_per_day_num_;
      this->countVisitByDate(basic_info.data_per_day_, date, unit.visit_num_);
      unit.stock_code_ = stock_code;
      unit.date_ = date;
      rank_list.push_back(unit);
      if (date > "2016-01-01" && date < "2016-02-01" && unit.visit_num_ > 0)
        LOG_MSG2("rank_attr=%p,date=%s,unit.visit_num=%d,stock_code=%s",
                 rank_attr,
                 date.c_str(),
                 unit.visit_num_,
                 unit.stock_code_.c_str());
    }
    rank_list.sort(IndustryStockRankUnit::cmp);
    rank_attr->addList(date, rank_list);
  }
}

bool IndustryStockHotRankObserver::countThirtyStandardDeviation(
    IndustryStockRankUnit& unit) {
  std::string stock_code = unit.stock_code_;
  std::string date = unit.date_;
  StockBasicInfo& basic_info =
      factory_->GetBasicInfoByCode(stock_code);
  int count = 30;
  std::map<std::string, DataPerDay>& dataPerDayMap =
      basic_info.data_per_day_;
  std::map<std::string, DataPerDay>::reverse_iterator riter =
      dataPerDayMap.rbegin();
  for (; riter != dataPerDayMap.rend(); riter++) {

  }
  return true;
}

bool IndustryStockHotRankObserver::getTwentyHotStocksOfIndustry(
    std::string& date,
    std::string& industry_name,
    std::list<IndustryStockRankUnit>& stocks) {
  BasicIndustryInfo& info =
      factory_->getBasicIndustryInfoByName(industry_name);
  IndustryStockRank* rank_attr =
      (IndustryStockRank*)info.getAttr(attr_name_);
  LOG_MSG2("getTwentyHotStocksOfIndustryrank_attr=%p",rank_attr);
  if (NULL == rank_attr)
    return false;
  else {
    stocks = rank_attr->rank_map_[date];
    return true;
  }
}

bool IndustryStockHotRankObserver::countVisitByDate(
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

} /* namespace stock_logic */
