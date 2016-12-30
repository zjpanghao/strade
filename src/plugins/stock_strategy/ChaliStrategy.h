/*
 * ChaliStrategy.h
 *
 *  Created on: 2016年8月31日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_CHALISTRATEGY_H_
#define GEORGE_MASTER_PLUGINS_STOCK_CHALISTRATEGY_H_

#include "Strategy.h"

namespace stock_logic {

class ChaliStrategy : public Strategy {
 public:
  ChaliStrategy(stock_logic::Subject* subject);
  virtual ~ChaliStrategy();

  bool getGoodStocks(std::string hist_date, std::vector<std::string>& stocks);    //根据策略选股
  //bool toSale(std::string hist_date, HoldingStock stock);                       //是否卖出股票
  bool getGoodStocks(StockAccount* account);                                      //选股
  bool setStockInfo(HoldingStock& stock_info);
  double getPriceChangeInPreDays(std::string& stock_code, std::string& start_day, int day_num);
  bool checkStockRepeat(std::string& stock_code, std::string& industry_name, std::string& birthday);
  void Update(int opcode, stock_logic::Subject* subject);

};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_CHALISTRATEGY_H_ */
