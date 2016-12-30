/*
 * StockAccount.h
 *
 *  Created on: 2016年8月30日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_STOCKACCOUNT_H_
#define GEORGE_MASTER_PLUGINS_STOCK_STOCKACCOUNT_H_

#include "Observer.h"
#include "HoldingStock.h"
#include "Strategy.h"

namespace stock_logic {

class Strategy;

class StockAccount : public Observer {
 public:
  StockAccount();
  StockAccount(std::string& birthday, Strategy* strategy, std::string& industry_name);
  virtual ~StockAccount();
  double getCurrentCash() const;
  void setCurrentCash(double currentCash);
  const std::map<std::string, HoldingStock>& getHoldingStocks() const;
  void setHoldingStocks(
      const std::map<std::string, HoldingStock>& holdingStocks);
  Strategy* getStrategy();
  void setStrategy(Strategy* strategy);
  double getTotalAsset() const;
  void setTotalAsset(double totalAsset);
  const std::string& getBirthday() const;
  void setBirthday(const std::string& birthday);
  double getCurrentProfit() const;
  void setCurrentProfit(double currentProfit);
  double getCurrentStockAsset() const;
  void setCurrentStockAsset(double currentStockAsset);
  const std::string& getDeathDate() const;
  void setDeathDate(const std::string& deathDate);
  bool isIsDeath() const;
  void setIsDeath(bool isDeath);
  double getTradePosition() const;
  void setTradePosition(double tradePosition);
  const std::string& getCurrentTradeDate() const;
  void setCurrentTradeDate(const std::string& currentTradeDate);
  const std::string& getIndustryName() const;
  void setIndustryName(const std::string& industryName);
  std::string getAccountName();
  void setAccountName(std::string account_name);

  virtual void Update(int opcode, stock_logic::Subject* subject);

  bool startStrategy();                                     //开始策略，把现金换算成股票
  void endStrategy();                                       //结算策略，把所有股票换算成现金
  void changeStrategy();                                    //转换策略
  virtual void run();
  void cycle_run();
  void printSelf();
  int updateTradePosition();                                 //计算仓位
  void updateAssetInfo();                                    //计算资产状况
  virtual void trade();
  bool isShortPosition();
  void addStock(std::string& stock_code, HoldingStock& stock_info);
  void buyStocks();
  std::string serialize();
  void serializeMember(std::string& serialize_str, double member, bool hasComma);
  void serializeMember(std::string& serialize_str, int member, bool hasComma);
  void serializeMember(std::string& serialize_str, std::string member, bool hasComma);
  void serializeStocks(std::string& serialize_str, bool hasComma);
  void setDeathDateBySuspention();
  void setCurrentDateBySuspention();
  std::string snapshot();

  double total_asset_;                                      //总资产
  double principal_;                                        //本金
  double current_cash_;                                     //当前持有现金
  double current_stock_asset_;                              //当前持有市值
  double trade_position_;                                   //当前仓位
  double current_profit_;                                   //持仓浮动盈亏
  double current_profit_ratio_;                             //持仓收益率
  std::string birthday_;
  std::string industry_name_;
  std::string death_date_;
  bool is_death_;
  std::map<std::string, HoldingStock> holding_stocks_;      //当前持有股票
  Strategy *strategy_;
  std::string current_trade_date_;
  std::string account_name_;
  int max_hold_day_;
  double stop_ratio_;
  double limited_ratio_;
  std::string current_date_;
  int traded_day_;                                          //已持有交易日
  int hold_day_;                                            //已持有自然日
  int rank_index_;                                          //HotRankOneHundredStrategy
  int stock_count_;                                         //
  int stock_hold_cycle_;
  std::multimap<std::string, HoldingStock> holding_stocks_record_;
  std::string serialize_str_;
  std::string transfer_day_;                                //调仓日
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_STOCKACCOUNT_H_ */
