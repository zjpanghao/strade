/*
 * CycleStockAccount.cpp
 *
 *  Created on: 2016年11月30日
 *      Author: Maibenben
 */

#include "CycleStockAccount.h"

namespace stock_logic {

CycleStockAccount::~CycleStockAccount() {
  // YGTODO Auto-generated destructor stub
}

void CycleStockAccount::run() {
  if (is_death_ == true)
    return;
  traded_day_ = 0;
  std::map<std::string, HistDataPerDay>& hs300_hist_data =
      factory_->GetHistDataByCode(HSSANBAI);
  std::map<std::string, HistDataPerDay>::iterator iter =
      hs300_hist_data.begin();
  for (; iter != hs300_hist_data.end(); iter++) {
    std::string date = iter->first;
    if (date <= birthday_)
      continue;
    traded_day_++;
    setCurrentTradeDate(date);
    LOG_MSG2("account_name=%s,trade_date=%s,",
             account_name_.c_str(),
             date.c_str());
    trade();
    this->setCurrentDateBySuspention();
    if (isShortPosition()) {
      this->setIsDeath(true);
      //this->setDeathDate(date);YGTODO
      this->setDeathDateBySuspention();
      this->printSelf();
      strategy_->strategy_start_date_ = date;
      this->startStrategy();
      traded_day_ = 0;
    }
  }
}

int CycleStockAccount::getStockHoldCycle() {
  return stock_hold_cycle_;
}

void CycleStockAccount::setStockHoldCycle(int stockHoldCycle) {
  stock_hold_cycle_ = stockHoldCycle;
}

} /* namespace stock_logic */
