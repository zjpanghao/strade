/*
 * HotRankFiftyHundredStrategy.h
 *
 *  Created on: 2016年11月30日
 *      Author: Maibenben
 *      1月5日到今天 每天热度前50和前100 条件：止损5% 涨停不买 跌停不卖 打开买卖
 *      停盘不买卖 复牌可交易 交易费率单笔千分之三
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_HOTRANKFIFTYHUNDREDSTRATEGY_H_
#define GEORGE_MASTER_PLUGINS_STOCK_HOTRANKFIFTYHUNDREDSTRATEGY_H_

#include "Strategy.h"

namespace stock_logic {

class HotRankFiftyHundredStrategy : public Strategy {
 public:
  HotRankFiftyHundredStrategy(stock_logic::Subject* subject);
  virtual ~HotRankFiftyHundredStrategy();
  bool getGoodStocks(StockAccount* account);                                      //选股
  bool setStockInfo(HoldingStock& stock_info);
  void Update(int opcode, stock_logic::Subject* subject);
  bool toSale(std::string hist_date, HoldingStock& stock, int traded_day);                         //是否卖出股票

  int rank_index_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_HOTRANKFIFTYHUNDREDSTRATEGY_H_ */
