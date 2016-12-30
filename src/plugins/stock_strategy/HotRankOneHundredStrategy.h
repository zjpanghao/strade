/*
 * HotRankOneHundredStrategy.h
 *
 *  Created on: 2016年11月21日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_HOTRANKONEHUNDREDSTRATEGY_H_
#define GEORGE_MASTER_PLUGINS_STOCK_HOTRANKONEHUNDREDSTRATEGY_H_

#include "Strategy.h"

namespace stock_logic {

class HotRankOneHundredStrategy : public Strategy {
 public:
  HotRankOneHundredStrategy(stock_logic::Subject* subject);
  virtual ~HotRankOneHundredStrategy();

  bool getGoodStocks(StockAccount* account);                                      //选股
  bool setStockInfo(HoldingStock& stock_info);
  void Update(int opcode, stock_logic::Subject* subject);

  int rank_index_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_HOTRANKONEHUNDREDSTRATEGY_H_ */
