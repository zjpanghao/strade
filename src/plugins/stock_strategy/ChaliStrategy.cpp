/*
 * ChaliStrategy.cpp
 *
 *  Created on: 2016年8月31日
 *      Author: Maibenben
 */

#include "ChaliStrategy.h"
#include "stock_factory.h"
#include "IndustryStockHotRank.h"
#include "StockAccountManager.h"
#include "DateUtil.h"
#include "HistEventsObserver.h"

namespace stock_logic {

ChaliStrategy::ChaliStrategy(stock_logic::Subject* subject) {
  subject_ = subject;
  name_ = "ChaliStrategy";
  //setMaxHoldDate();
}

ChaliStrategy::~ChaliStrategy() {
  // YGTODO Auto-generated destructor stub
}

bool ChaliStrategy::getGoodStocks(std::string hist_date,
                                  std::vector<std::string>& stocks) {
  return true;
}

bool ChaliStrategy::getGoodStocks(StockAccount* account) {
  LOG_MSG("ChaliStrategy choose stock");
  std::string birthday = account->getBirthday();
  std::string industry_name = account->getIndustryName();
  LOG_MSG2("birthday=%s", birthday.c_str());
  factory_ = (StockFactory*)subject_;
  std::string observer_name = "IndustryStockHotRankObserver";
  IndustryStockHotRankObserver* rank_observer =
      (IndustryStockHotRankObserver*)factory_->GetObserverByName(observer_name);

  std::string hist_event_observer_name = "HistEventsObserver";
  HistEventsObserver* hist_event_observer =
      (HistEventsObserver*)factory_->GetObserverByName(hist_event_observer_name);
  std::set<std::string> event_stocks;
  hist_event_observer->getEventStocksByDate(birthday, event_stocks);
  LOG_MSG2("event_stocks count=%d", event_stocks.size());

  std::list<IndustryStockRankUnit> stocks;
  rank_observer->getTwentyHotStocksOfIndustry(birthday, industry_name, stocks);
  std::list<IndustryStockRankUnit>::iterator iter =
      stocks.begin();
  int count = 0;
  for (; iter != stocks.end(); iter++) {
    LOG_MSG2("IndustryStockRankUnit:date=%s,stock=%s,visit_num=%d",
             birthday.c_str(),
             iter->stock_code_.c_str(),
             iter->visit_num_);
    if (count > 19) {
      continue;
    }
    std::string stock_code = iter->stock_code_;
    HistDataPerDay hist_data;
    if (factory_->GetHistDataByDate(stock_code, birthday, hist_data)) {
      double pre_one_day_price_change =
          this->getPriceChangeInPreDays(stock_code, birthday, 1);
      //if (hist_data.day_yield_ > 0.03)
      if (pre_one_day_price_change > 0.03)
        continue;
      double pre_price_change = this->getPriceChangeInPreDays(stock_code, birthday, 3);
      if (pre_price_change > 0.1)
        continue;
      double open_price = hist_data.open_;
      double close_price = hist_data.close_/(1 + hist_data.day_yield_);
      double open_price_change = 0;
      if (close_price > 1)
        open_price_change = (open_price - close_price)/close_price;
      if (open_price_change > 0.98)
        continue;

      if (event_stocks.end() == event_stocks.find(stock_code))
        continue;
      //if (this->checkStockRepeat(stock_code, industry_name, birthday))
      //  continue;
      HoldingStock stock_info;
      this->setStockInfo(stock_info);
      stock_info.setStockCode(stock_code);
      stock_info.setBuyDate(birthday);
      //stock_info.setBuyPrice(hist_data.high_);
      //stock_info.setCurrentPrice(hist_data.high_);
      stock_info.setBuyPrice(hist_data.open_);
      stock_info.setCurrentPrice(hist_data.open_);
      stock_info.setMaxPriceInHoldDays(hist_data.high_);
      stock_info.setMinPriceInHoldDays(hist_data.low_);
      stock_info.current_date_ = birthday;
      //stock_info.setCurrentDate(birthday);
      stock_info.update();
      account->addStock(stock_code, stock_info);
      count++;
    }
    else
      continue;
  }
  return true;
}

bool ChaliStrategy::setStockInfo(HoldingStock& stock_info) {
  //stock_info.setHoldDays(7);
  stock_info.setStopRatio(stop_ratio_);
  stock_info.setLimitedRatio(sell_ratio_);
  return true;
}

void ChaliStrategy::Update(int opcode, stock_logic::Subject* subject) {

}

double ChaliStrategy::getPriceChangeInPreDays(std::string& stock_code, std::string& start_day, int day_num) {
  double price_change = 0;
  if (day_num < 1)
    return price_change;
  day_num++;
  std::map<std::string, HistDataPerDay>& hs300_hist_data =
      factory_->GetHistDataByCode(HSSANBAI);
  std::map<std::string, HistDataPerDay>::reverse_iterator r_iter =
      hs300_hist_data.rbegin();
  double start_price = 0;
  double end_price = 0;
  std::string start_date;
  std::string end_date;
  bool find_start_date = false;
  int current_day_num = 0;
  for (; r_iter != hs300_hist_data.rend(); r_iter++) {
    std::string date = r_iter->first;
    HistDataPerDay hist_data_per_day;
    if (start_day < date)
      continue;
    if (start_day > date) {
      if (factory_->GetHistDataByDate(stock_code, date, hist_data_per_day)) {
        if (find_start_date == false) {
          find_start_date = true;
          end_price = hist_data_per_day.close_;
          end_date = date;
          price_change = hist_data_per_day.day_yield_;
          //start_price = hist_data_per_day.close_;
          //start_date = date;
        } else {
          start_price = hist_data_per_day.close_;
          start_date = date;
        }
      }
      else
        continue;
      current_day_num += 1;
      if (current_day_num == day_num)
        break;
    }
  }
  if (start_price > 1)
      price_change = (end_price - start_price) / start_price;
  LOG_MSG2("stock_code=%s,start_day=%s,price_change=%f,start_price=%f,end_price=%f,start_date=%s,end_date=%s",
           stock_code.c_str(),
           start_day.c_str(),
           price_change,
           start_price,
           end_price,
           start_date.c_str(),
           end_date.c_str());
  return price_change;
}

bool ChaliStrategy::checkStockRepeat(std::string& stock_code, std::string& industry_name, std::string& birthday) {
  StockFactory* factory = (StockFactory*)subject_;
  std::string manager_name = "StockAccountManager";
  StockAccountManager* manager = (StockAccountManager*)factory->GetObserverByName(manager_name);
  if (NULL == manager) {
    LOG_MSG("StockAccountManager not found!");
    return false;
  }
  std::map<std::string, StockAccount>& stock_accounts = manager->stock_accounts_;
  std::map<std::string, StockAccount>::iterator iter =
      stock_accounts.begin();
  for (; iter != stock_accounts.end(); iter++) {
    StockAccount& account = iter->second;
    const std::string& account_industry_name = account.getIndustryName();
    const std::string& account_stock_birthday = account.getBirthday();
    if (account_industry_name != industry_name)
      continue;
    int days = DateUtil::instance()->DaysBetween2Date(birthday, account_stock_birthday);
    if (days > 7 || days < 1)
      continue;
    std::map<std::string, HoldingStock>& holding_stocks =
        account.holding_stocks_;
    std::map<std::string, HoldingStock>::iterator stock_iter =
        holding_stocks.begin();
    for (; stock_iter != holding_stocks.end(); stock_iter++) {
      std::string holding_stock_code = stock_iter->first;
      if (holding_stock_code == stock_code)
        return true;
    }
  }
  return false;
}

} /* namespace stock_logic */
