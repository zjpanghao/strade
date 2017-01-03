/*
 * HoldingStock.h
 *
 *  Created on: 2016年8月30日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_HOLDINGSTOCK_H_
#define GEORGE_MASTER_PLUGINS_STOCK_HOLDINGSTOCK_H_

#include <string>

using namespace std;

namespace stock_logic {

/*
 * 股票卖出条件：
 * 1、股票达到止损价格
 * 2、股票达到止盈价格
 * 3、股票已持有天数超过计划持有天数
 * 暂时不考虑股票停牌，跌停或涨停情况。
 */
class HoldingStock {
 public:

  HoldingStock();
  ~HoldingStock();
  const std::string& getBuyDate() const;
  void setBuyDate(const std::string& buyDate);
  double getBuyPrice() const;
  void setBuyPrice(double buyPrice);
  double getCash() const;
  void setCash(double cash);
  double getCurrentPrice() const;
  void setCurrentPrice(double currentPrice);
  int getHoldDays() const;
  void setHoldDays(int holdDays);
  double getLimitedPrice() const;
  void setLimitedPrice(double limitedPrice);
  int getPlanHoldDays() const;
  void setPlanHoldDays(int planHoldDays);
  double getProfit() const;
  void setProfit(double profit);
  double getProfitRatio() const;
  void setProfitRatio(double profitRatio);
  const std::string& getSaleDate() const;
  void setSaleDate(const std::string& saleDate);
  double getSellPrice() const;
  void setSellPrice(double sellPrice);
  double getStockNum() const;
  void setStockNum(double stockNum);
  double getStopPrice() const;
  void setStopPrice(double stopPrice);
  double getTradingPosition() const;
  void setTradingPosition(double tradingPosition);
  double getCost() const;
  void setCost(double cost);
  const std::string& getStockCode() const;
  void setStockCode(const std::string& stockCode);
  bool isIsHold() const;
  void setIsHold(bool isHold);
  double getLimitedRatio() const;
  void setLimitedRatio(double limitedRatio);
  double getStopRatio() const;
  void setStopRatio(double stopRatio);
  double getPrincipal() const;
  void setPrincipal(double principal);
  const std::string& getTradeDay() const;
  void setTradeDay(const std::string& tradeDay);

  bool sale(std::string& sale_date, double sale_price, double sale_ratio = 1);
  bool saleAsSuspension(double sale_ratio = 1);
  bool buy(std::string& buy_date, double buy_price, double stock_num, double trade_position);
  bool buy(double money, double trade_position);
  int countHoldDays();
  int countProfit();
  void settle();
  bool isShortPosition();
  bool isFullPosition();
  void printSelf();
  void updateStopRatio(double price_ratio);
  void updateLimitedRatio(double price_ratio);
  void update();

  double getMaxPriceInHoldDays() const;
  void setMaxPriceInHoldDays(double maxPriceInHoldDays);
  double getMinPriceInHoldDays() const;
  void setMinPriceInHoldDays(double minPriceInHoldDays);

  std::string stock_code_;
  double buy_price_;                           //买入价格
  double sell_price_;                          //卖出价格
  double stop_price_;                          //止损价格
  double stop_ratio_;                          //止损收益率
  double limited_price_;                       //止盈价格
  double limited_ratio_;                       //止盈收益率
  double current_price_;                       //当前价格
  std::string current_date_;                   //当前交易日
  double trading_position_;                    //仓位
  double cash_;                                //股票总价值
  double profit_;                              //盈利资金
  double cost_;                                //成本价
  double principal_;                           //本金
  double profit_ratio_;                        //收益率
  double stock_num_;                           //股票数量
  int hold_days_;                              //已持有天数
  int plan_hold_days_;                         //计划持有天数
  std::string buy_date_;                       //买入日期
  std::string sale_date_;                      //卖出日期
  bool is_hold_;                               //是否持有
  std::string trade_day_;                      //交易日
  double max_price_in_hold_days_;              //持有日内最高价
  double min_price_in_hold_days_;              //持有日内最低价
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_HOLDINGSTOCK_H_ */
