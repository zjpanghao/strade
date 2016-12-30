/*
 * StockAccount.cpp
 *
 *  Created on: 2016年8月30日
 *      Author: Maibenben
 */

#include "StockAccount.h"

namespace stock_logic {

StockAccount::StockAccount() {
  current_profit_ = 0;
  principal_ = 0;
  total_asset_ = 0;
  current_stock_asset_ = 0;
  current_cash_ = 0;
  is_death_ = false;
  trade_position_ = 0;
  max_hold_day_ = 7;
  stop_ratio_ = -0.05;
  limited_ratio_ = 0.05;
  traded_day_ = 0;
  current_profit_ratio_ = 0;
  strategy_ = NULL;
  stock_count_ = 0;
}

StockAccount::StockAccount(std::string& birthday, Strategy* strategy, std::string& industry_name) {
  stock_hold_cycle_ = 7;
  stock_count_ = 0;
  birthday_ = birthday;
  transfer_day_ = birthday;
  industry_name_ = industry_name;
  strategy_ = strategy;
  factory_ = StockFactory::GetInstance();
  current_profit_ = 0;
  principal_ = total_asset_ = 1000000;
  current_stock_asset_ = 0;
  current_cash_ = 1000000;
  is_death_ = false;
  trade_position_ = 0;
  max_hold_day_ = strategy->max_hold_date_;
  stop_ratio_ = strategy->stop_ratio_;
  limited_ratio_ = strategy->sell_ratio_;
  startStrategy();
  //run();
}

StockAccount::~StockAccount() {
//  if (NULL != strategy_) {
//    delete strategy_;
//    strategy_ = NULL;
//  }
}

double StockAccount::getCurrentCash() const {
  return current_cash_;
}

void StockAccount::setCurrentCash(double currentCash) {
  current_cash_ = currentCash;
}

const std::map<std::string, HoldingStock>& StockAccount::getHoldingStocks() const {
  return holding_stocks_;
}

void StockAccount::setHoldingStocks(
    const std::map<std::string, HoldingStock>& holdingStocks) {
  holding_stocks_ = holdingStocks;
}

Strategy* StockAccount::getStrategy(){
  return strategy_;
}

void StockAccount::setStrategy(Strategy* strategy) {
  strategy_ = strategy;
}

double StockAccount::getTotalAsset() const {
  return total_asset_;
}

void StockAccount::setTotalAsset(double totalAsset) {
  total_asset_ = totalAsset;
}

void StockAccount::Update(int opcode, stock_logic::Subject* subject) {

}

bool StockAccount::startStrategy() {
  if (NULL == strategy_)
    return false;
  else {
    //strategy_->getGoodStocks(birthday_, holding_stocks_);
    holding_stocks_.clear();
    strategy_->getGoodStocks(this);
    if (holding_stocks_.size() > 0) {
      current_cash_ = 0;
      trade_position_ = 1;
      stock_count_ = holding_stocks_.size();
    }
    buyStocks();
    return true;
  }
}

void StockAccount::endStrategy() {

}

void StockAccount::changeStrategy() {

}

int StockAccount::updateTradePosition() {
  std::map<std::string, HoldingStock>::iterator iter =
      holding_stocks_.begin();
  trade_position_ = 0;
  for (; iter != holding_stocks_.end(); iter++) {
    trade_position_ += iter->second.getTradingPosition();
    LOG_MSG2("account_name=%s,stock=%s,trade_position=%f",
             account_name_.c_str(),
             iter->first.c_str(),
             iter->second.getTradingPosition());
  }
  return trade_position_;
}

void StockAccount::updateAssetInfo() {
  current_stock_asset_ = 0;
  total_asset_ = current_cash_;
  std::map<std::string, HoldingStock>::iterator iter =
      holding_stocks_.begin();
  for (; iter != holding_stocks_.end(); iter++) {
    HoldingStock& stock = iter->second;
    double stock_asset = stock.getCash();
    if (stock.isFullPosition())
      current_stock_asset_ += stock_asset;
    total_asset_ += stock_asset;
  }
  current_profit_ = total_asset_ - principal_;
  current_profit_ratio_ = current_profit_ / principal_;
}

void StockAccount::trade() {
  std::map<std::string, HoldingStock>::iterator iter =
      holding_stocks_.begin();
  for (; iter != holding_stocks_.end(); iter++) {
    HoldingStock& stock = iter->second;
    if (stock.isIsHold())
      strategy_->toSale(current_trade_date_, iter->second, traded_day_);
  }
  updateAssetInfo();
  updateTradePosition();
}

bool StockAccount::isShortPosition() {
  if (trade_position_ < 0.01)
    return true;
  else
    return false;
}

void StockAccount::addStock(std::string& stock_code, HoldingStock& stock_info) {
  holding_stocks_[stock_code] = stock_info;
}

void StockAccount::buyStocks() {
  std::map<std::string, HoldingStock>::iterator iter =
      holding_stocks_.begin();
  int stock_count = holding_stocks_.size();
  double temp_asset = 0;
  for (; iter != holding_stocks_.end(); iter++) {
    double money = total_asset_ / stock_count;
    double trade_position = 1 / stock_count;
    iter->second.buy(money, trade_position);
    temp_asset += money;
  }
  current_cash_ = total_asset_ - temp_asset;
  current_stock_asset_ = temp_asset;
}

void StockAccount::run() {
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
      break;
    }
  }
}

void StockAccount::cycle_run() {
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
      this->transfer_day_ = date;
      this->snapshot();
      this->startStrategy();
      traded_day_ = 0;
    }
  }
}

void StockAccount::setDeathDateBySuspention() {
  std::string date = "";
  std::map<std::string, HoldingStock>::iterator iter =
      holding_stocks_.begin();
  for (; iter != holding_stocks_.end(); iter++) {
    const std::string& sale_date = iter->second.getSaleDate();
    if (date == "") {
      date = sale_date;
    } else if (date < sale_date) {
      date = sale_date;
    }
  }
  setDeathDate(date);
}

void StockAccount::setCurrentDateBySuspention() {
  std::string date = "";
  std::map<std::string, HoldingStock>::iterator iter =
      holding_stocks_.begin();
  for (; iter != holding_stocks_.end(); iter++) {
    std::string current_date = iter->second.current_date_;
    if (date == "") {
      date = current_date;
    } else if (date < current_date) {
      date = current_date;
    }
  }
  current_date_ = date;
}

void StockAccount::printSelf() {
  LOG_MSG("Account info.");
  LOG_MSG2("account_name=%s,industry_name=%s,birthday=%s,death_time=%s",
           account_name_.c_str(),
           industry_name_.c_str(),
           birthday_.c_str(),
           current_trade_date_.c_str());
  LOG_MSG2("total_asset=%f,current_cash=%f,current_stock_asset=%f,current_profit=%f,current_profit_ratio=%f",
           total_asset_,
           current_cash_,
           current_stock_asset_,
           current_profit_,
           current_profit_ratio_);
  /*std::map<std::string, HoldingStock>::iterator iter =
      holding_stocks_.begin();
  for (; iter != holding_stocks_.end(); iter++) {
    iter->second.printSelf();
  }*/
}

std::string StockAccount::serialize() {
  std::string result = "";
  if (holding_stocks_.size() < 1)
    return result;
  serializeMember(result, account_name_, true);
  serializeMember(result, max_hold_day_, true);
  serializeMember(result, "当天盘中股价跌幅大于-5%", true);
  //serializeMember(result, limited_ratio_, true);
  //serializeMember(result, stop_ratio_, true);
  serializeMember(result, industry_name_, true);
  serializeMember(result, birthday_, true);
  serializeMember(result, current_date_, true);
  //serializeMember(result, death_date_, true);
  serializeMember(result, principal_, true);
  serializeMember(result, total_asset_, true);
  //serializeMember(result, current_cash_, true);
  //serializeMember(result, current_stock_asset_, true);
  serializeMember(result, current_profit_, true);
  serializeMember(result, current_profit_ratio_, true);
  serializeStocks(result, false);
  result += '\n';
  LOG_MSG2("result=%s", result.c_str());
  serialize_str_ += result;
  return serialize_str_;
}

std::string StockAccount::snapshot() {
  std::string result = "";
  if (holding_stocks_.size() < 1)
    return result;
  serializeMember(result, account_name_, true);
  serializeMember(result, max_hold_day_, true);
  serializeMember(result, "当天盘中股价跌幅大于-5%", true);
  //serializeMember(result, limited_ratio_, true);
  //serializeMember(result, stop_ratio_, true);
  serializeMember(result, industry_name_, true);
  //serializeMember(result, birthday_, true);
  serializeMember(result, transfer_day_, true);
  serializeMember(result, current_date_, true);
  //serializeMember(result, death_date_, true);
  serializeMember(result, principal_, true);
  serializeMember(result, total_asset_, true);
  //serializeMember(result, current_cash_, true);
  //serializeMember(result, current_stock_asset_, true);
  serializeMember(result, current_profit_, true);
  serializeMember(result, current_profit_ratio_, true);
  serializeStocks(result, false);
  result += '\n';
  LOG_MSG2("snapshot=%s", result.c_str());
  //return result;
  serialize_str_ += result;
  LOG_MSG2("serialize_str_=%s", serialize_str_.c_str());
  return result;
}

void StockAccount::serializeStocks(std::string& serialize_str, bool hasComma) {
  std::string stocks = "stock:";
  std::map<std::string, HoldingStock>::iterator iter =
      holding_stocks_.begin();
  for (; iter != holding_stocks_.end(); iter++) {
    stocks += iter->first;
    stocks += ";";
  }
  serialize_str += stocks;
  if (hasComma)
    serialize_str += ",";
}

void StockAccount::serializeMember(std::string& serialize_str, std::string member, bool hasComma) {
  serialize_str += member;
  if (hasComma)
    serialize_str += ",";
}

void StockAccount::serializeMember(std::string& serialize_str, double member, bool hasComma) {
  char temp_str[50];
  sprintf(temp_str, "%f", member);
  serialize_str += temp_str;
  if (hasComma)
    serialize_str += ",";
}

void StockAccount::serializeMember(std::string& serialize_str, int member, bool hasComma) {
  char temp_str[50];
  sprintf(temp_str, "%d", member);
  serialize_str += temp_str;
  if (hasComma)
    serialize_str += ",";
}

double StockAccount::getTradePosition() const {
  return trade_position_;
}

void StockAccount::setTradePosition(double tradePosition) {
  trade_position_ = tradePosition;
}

const std::string& StockAccount::getCurrentTradeDate() const {
  return current_trade_date_;
}

void StockAccount::setCurrentTradeDate(const std::string& currentTradeDate) {
  current_trade_date_ = currentTradeDate;
}

const std::string& StockAccount::getIndustryName() const {
  return industry_name_;
}

void StockAccount::setIndustryName(const std::string& industryName) {
  industry_name_ = industryName;
}

std::string StockAccount::getAccountName() {
  return account_name_;
}

void StockAccount::setAccountName(std::string account_name) {
  account_name_ = account_name;
}

} /* namespace stock_logic */

const std::string& stock_logic::StockAccount::getBirthday() const {
  return birthday_;
}

void stock_logic::StockAccount::setBirthday(const std::string& birthday) {
  birthday_ = birthday;
}

double stock_logic::StockAccount::getCurrentProfit() const {
  return current_profit_;
}

void stock_logic::StockAccount::setCurrentProfit(double currentProfit) {
  current_profit_ = currentProfit;
}

double stock_logic::StockAccount::getCurrentStockAsset() const {
  return current_stock_asset_;
}

void stock_logic::StockAccount::setCurrentStockAsset(double currentStockAsset) {
  current_stock_asset_ = currentStockAsset;
}

const std::string& stock_logic::StockAccount::getDeathDate() const {
  return death_date_;
}

void stock_logic::StockAccount::setDeathDate(const std::string& deathDate) {
  death_date_ = deathDate;
}

bool stock_logic::StockAccount::isIsDeath() const {
  return is_death_;
}

void stock_logic::StockAccount::setIsDeath(bool isDeath) {
  //TODO
  current_cash_ = total_asset_;
  is_death_ = isDeath;
}


