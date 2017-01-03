/*
 * StockAccountManager.h
 *
 *  Created on: 2016年8月30日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_STOCKACCOUNTMANAGER_H_
#define GEORGE_MASTER_PLUGINS_STOCK_STOCKACCOUNTMANAGER_H_

#include "Observer.h"
#include "StockAccount.h"

namespace stock_logic {

class StockAccountManager : public Observer {
 public:
  StockAccountManager(stock_logic::Subject* subject);
  virtual ~StockAccountManager();

  void delStockAccount(std::string& account_name);

  bool createAccount(std::string& account_name, std::string& birthday, Strategy* strategy, std::string& industry_name);
  bool createCycleAccount(std::string& account_name, std::string& birthday, Strategy* strategy, std::string& industry_name);

  virtual void Update(int opcode, stock_logic::Subject* subject);

  void runChaliStrategy(int opcode, stock_logic::Subject* subject);
  void runHotRankOneHundredStrategy(int opcode, stock_logic::Subject* subject);
  void runHotRankFiftyHundredStrategy(int opcode, stock_logic::Subject* subject);

  StockAccount* getAccountByName(std::string& account_name);

  void serialize();
  void serializeByIndustry(std::string& result);
  void serializeByBuyDate();

  std::map<std::string, StockAccount> stock_accounts_;
  std::vector<int> hold_days_;
  std::vector<double> limited_ratios_;
  std::vector<double> stop_ratios_;
  std::vector<int> traded_days_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_STOCKACCOUNTMANAGER_H_ */
