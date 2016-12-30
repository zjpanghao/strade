/*
 * CycleStockAccount.h
 *
 *  Created on: 2016年11月30日
 *      Author: Maibenben
 *  根据周期买股，本金滚动
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_CYCLESTOCKACCOUNT_H_
#define GEORGE_MASTER_PLUGINS_STOCK_CYCLESTOCKACCOUNT_H_

#include "StockAccount.h"

namespace stock_logic {

class CycleStockAccount : public StockAccount {
 public:
  CycleStockAccount(std::string& birthday, Strategy* strategy, std::string& industry_name):
  StockAccount(birthday, strategy, industry_name){
    stock_hold_cycle_ = 7;
  }
  virtual ~CycleStockAccount();
  int getStockHoldCycle();
  void setStockHoldCycle(int stockHoldCycle);

  void run();


};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_CYCLESTOCKACCOUNT_H_ */
