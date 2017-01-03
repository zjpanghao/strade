/*
 * Strategy.h
 *
 *  Created on: 2016年8月30日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_STRATEGY_H_
#define GEORGE_MASTER_PLUGINS_STOCK_STRATEGY_H_

#include "Observer.h"
#include "HoldingStock.h"
#include "StockAccount.h"

#include <vector>

using namespace std;

namespace stock_logic {

class StockAccount;

class Strategy : public Observer {
 public:
  Strategy();
  virtual ~Strategy();

  virtual bool getGoodStocks(StockAccount* account);
  virtual bool setStockInfo(HoldingStock stock_info);
  virtual bool toSale(std::string hist_date, HoldingStock& stock, int traded_day);                         //是否卖出股票
  virtual void Update(int opcode, stock_logic::Subject* subject);

  int getMaxHoldDate();
  void setMaxHoldDate(int maxHoldDate);
  const std::string& getName() const;
  void setName(const std::string& name);
  double getSellRatio() const;
  void setSellRatio(double sellRatio);
  double getStopRatio() const;
  void setStopRatio(double stopRatio);
  const std::string& getType() const;
  void setType(const std::string& type);

  std::string strategy_name_;            //策略名称
  std::string type_;                     //策略类型
  int max_hold_date_;                    //最大持有日期
  double sell_ratio_;                    //止盈收益率
  double stop_ratio_;                    //止损收益率
  int max_traded_day_;                   //交易天数
  std::string strategy_start_date_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_STRATEGY_H_ */
