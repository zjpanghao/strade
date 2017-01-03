/*
 * HotRankFiftyHundredStrategy.cpp
 *
 *  Created on: 2016年11月30日
 *      Author: Maibenben
 */

#include "HotRankFiftyHundredStrategy.h"
#include "stock_factory.h"
#include "StockAccountManager.h"
#include "DateUtil.h"
#include "IndustryStockHotRank.h"

namespace stock_logic {

HotRankFiftyHundredStrategy::HotRankFiftyHundredStrategy(stock_logic::Subject* subject) {
  subject_ = subject;
  name_ = "HotRankFiftyHundredStrategy";
  rank_index_ = 0;
}

HotRankFiftyHundredStrategy::~HotRankFiftyHundredStrategy() {
  // YGTODO Auto-generated destructor stub
}

bool fiftyCountVisitNumByDate(
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

bool fiftyCountVisitByDate(
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

bool HotRankFiftyHundredStrategy::getGoodStocks(StockAccount* account) {
  LOG_MSG("HotRankFiftyHundredStrategy choose stock");
  std::string birthday = this->strategy_start_date_;
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
    fiftyCountVisitByDate(basic_info.data_per_day_, birthday, unit.visit_num_);
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
  if (rank_index_ == 100) {
    rank_index_ = 0;
    max_count = 100;
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
      break;
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

bool HotRankFiftyHundredStrategy::toSale(std::string hist_date, HoldingStock& stock, int traded_day) {
  HistDataPerDay hist_data;
  const std::string& stock_code = stock.getStockCode();
  StockFactory* factory = StockFactory::GetInstance();
  LOG_MSG2("hist_date=%s,stock_code=%s", hist_date.c_str(),stock_code.c_str());
  if (factory->GetHistDataByDate(stock_code, hist_date, hist_data)) {
    double high = hist_data.high_;
    double low = hist_data.low_;
    double close = hist_data.close_;
    double limit_price = stock.getLimitedPrice();
    double stop_price = stock.getStopPrice();
    double buy_price = stock.getBuyPrice();
    const std::string& buy_date = stock.getBuyDate();
    LOG_MSG2("stock_code=%s,high=%f,low=%f,limit_price=%f,stop_price=%f,buy_date=%s,buy_price=%f",
             stock_code.c_str(),
             high,
             low,
             limit_price,
             stop_price,
             buy_date.c_str(),
             buy_price);
    int hold_day = DateUtil::instance()->DaysBetween2Date(hist_date, buy_date);
    LOG_MSG2("hold_day=%d,max_hold_date_=%d,max_traded_day=%d,traded_day=%d",
             hold_day,
             max_hold_date_,
             max_traded_day_,
             traded_day);
    HistDataPerDay hs300_hist_data;
    factory->GetHistDataByDate("hs300", hist_date, hs300_hist_data);
    double p_hs300_change = hs300_hist_data.day_yield_;
    LOG_MSG2("p_hs300_change=%f", p_hs300_change);
    if (traded_day > max_traded_day_) {
    //if (hold_day > max_hold_date_) {
      //stock.setCurrentPrice(stock.getMaxPriceInHoldDays());
      //stock.setCurrentPrice(stock.getMinPriceInHoldDays());
      stock.saleAsSuspension();
      //stock.sale(hist_date, low);
      LOG_MSG2("sale stocks, hold_day=%d,max_hold_date_=%d", hold_day, max_hold_date_);
      return true;
    }
    else {
      double pre_close = stock.current_price_;
      assert(pre_close > 1 && low > 1);
      double temp_stop_price = pre_close * (1 - 0.05);
      double temp_stop_to_buy_price = stock.buy_price_ * (1 - 0.05);
      if (temp_stop_price > low) {
        stock.sale(hist_date, temp_stop_price);
        LOG_MSG2("low to temp_stop_price=%f, sale stocks", temp_stop_price);
      }

      /*double temp_sale_price = pre_close * (1 + p_hs300_change + 0.01);
      if (high > temp_sale_price) {
        stock.sale(hist_date, temp_sale_price);
        LOG_MSG2("high to temp_sale_price=%f, sale stocks", temp_sale_price);
      }*/
      /*if (temp_stop_to_buy_price > low) {
        stock.sale(hist_date, temp_stop_to_buy_price);
        LOG_MSG2("low to temp_stop_to_buy_price=%f, sale stocks", temp_stop_to_buy_price);
      }*/
      stock.setCurrentPrice(close);
      stock.current_date_ = hist_date;
      if (stock.max_price_in_hold_days_ < high)
        stock.setMaxPriceInHoldDays(high);
      if (stock.min_price_in_hold_days_ > low && low > 1)
        stock.setMinPriceInHoldDays(low);
      //stock.setCurrentDate(hist_date);
    }

    /*if (high > stock.getLimitedPrice() && high > 1) {
      LOG_MSG("up to limited price");
      stock.sale(hist_date, limit_price);
    }
    else if (low < stop_price && low > 1) {
      LOG_MSG("low to stop price");
      stock.sale(hist_date, stop_price);
    }*/
    return true;
  }
  else
    return false;
}

bool HotRankFiftyHundredStrategy::setStockInfo(HoldingStock& stock_info) {
  //stock_info.setHoldDays(7);
  stock_info.setStopRatio(stop_ratio_);
  stock_info.setLimitedRatio(sell_ratio_);
  return true;
}

void HotRankFiftyHundredStrategy::Update(int opcode, stock_logic::Subject* subject) {

}

} /* namespace stock_logic */
