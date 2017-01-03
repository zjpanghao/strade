/*
 * HoldingStock.cpp
 *
 *  Created on: 2016年8月30日
 *      Author: Maibenben
 */

#include "HoldingStock.h"
#include "DateUtil.h"
#include "logic/logic_comm.h"

namespace stock_logic {

HoldingStock::HoldingStock() {
  plan_hold_days_ = 0;
  hold_days_ = 0;
  stock_num_ = 0;
  stop_price_ = 0;
  current_price_ = 0;
  cash_ = 0;
  profit_ratio_ = 0;
  profit_ = 0;
  limited_price_ = 0;
  buy_price_ = 0;
  sell_price_ = 0;
  trading_position_ = 0;
  cost_ = 0;
  principal_ = 0;
  is_hold_ = false;
  stop_ratio_ = 0;
  limited_ratio_ = 0;
  max_price_in_hold_days_ = 0;
  min_price_in_hold_days_ = 0;
}

HoldingStock::~HoldingStock() {
}

const std::string& HoldingStock::getBuyDate() const {
  return buy_date_;
}

void HoldingStock::setBuyDate(const std::string& buyDate) {
  buy_date_ = buyDate;
}

double HoldingStock::getBuyPrice() const {
  return buy_price_;
}

void HoldingStock::setBuyPrice(double buyPrice) {
  buy_price_ = buyPrice;
}

double HoldingStock::getCash() const {
  return cash_;
}

void HoldingStock::setCash(double cash) {
  cash_ = cash;
}

double HoldingStock::getCurrentPrice() const {
  return current_price_;
}

void HoldingStock::setCurrentPrice(double currentPrice) {
  current_price_ = currentPrice;
}

int HoldingStock::getHoldDays() const {
  return hold_days_;
}

void HoldingStock::setHoldDays(int holdDays) {
  hold_days_ = holdDays;
}

double HoldingStock::getLimitedPrice() const {
  return limited_price_;
}

void HoldingStock::setLimitedPrice(double limitedPrice) {
  limited_price_ = limitedPrice;
}

int HoldingStock::getPlanHoldDays() const {
  return plan_hold_days_;
}

void HoldingStock::setPlanHoldDays(int planHoldDays) {
  plan_hold_days_ = planHoldDays;
}

double HoldingStock::getProfit() const {
  return profit_;
}

void HoldingStock::setProfit(double profit) {
  profit_ = profit;
}

double HoldingStock::getProfitRatio() const {
  return profit_ratio_;
}

void HoldingStock::setProfitRatio(double profitRatio) {
  profit_ratio_ = profitRatio;
}

const std::string& HoldingStock::getSaleDate() const {
  return sale_date_;
}

void HoldingStock::setSaleDate(const std::string& saleDate) {
  sale_date_ = saleDate;
}

double HoldingStock::getSellPrice() const {
  return sell_price_;
}

void HoldingStock::setSellPrice(double sellPrice) {
  sell_price_ = sellPrice;
}

double HoldingStock::getStockNum() const {
  return stock_num_;
}

void HoldingStock::setStockNum(double stockNum) {
  stock_num_ = stockNum;
}

double HoldingStock::getStopPrice() const {
  return stop_price_;
}

void HoldingStock::setStopPrice(double stopPrice) {
  stop_price_ = stopPrice;
}

double HoldingStock::getTradingPosition() const {
  return trading_position_;
}

void HoldingStock::setTradingPosition(double tradingPosition) {
  trading_position_ = tradingPosition;
}

bool HoldingStock::sale(std::string& sale_date, double sale_price, double sale_ratio) {
  //YGTODO 判断能否卖出
  setIsHold(false);
  setSaleDate(sale_date);
  countHoldDays();
  setSellPrice(sale_price);
  countProfit();
  setTradingPosition(0);
  LOG_MSG2("sell stocks,current_date=%s,buy_date=%s,buy_price=%f,sale_date=%s,sale_price=%f,hold_days=%d,profit_ratio=%f,profit=%f,stop_price=%f,limited_price=%f,cash=%f",
           current_date_.c_str(),
           buy_date_.c_str(),
           buy_price_,
           sale_date.c_str(),
           sell_price_,
           hold_days_,
           profit_ratio_,
           profit_,
           stop_price_,
           limited_price_,
           cash_
           );
  return true;
}

bool HoldingStock::saleAsSuspension(double sale_ratio) {
  setIsHold(false);
  setSaleDate(current_date_);
  countHoldDays();
  setSellPrice(current_price_);
  //setSellPrice(max_price_in_hold_days_);
  countProfit();
  setTradingPosition(0);
  LOG_MSG2("saleAsSuspension stocks,current_date=%s,buy_date=%s,buy_price=%f,sale_date=%s,sale_price=%f,hold_days=%d,profit_ratio=%f,profit=%f,stop_price=%f,limited_price=%f,cash=%f",
           current_date_.c_str(),
           buy_date_.c_str(),
           buy_price_,
           sale_date_.c_str(),
           sell_price_,
           hold_days_,
           profit_ratio_,
           profit_,
           stop_price_,
           limited_price_,
           cash_
           );
  return true;
}

bool HoldingStock::buy(std::string& buy_date, double buy_price, double stock_num, double trade_position) {
  //YGTODO 判断能否买入
  if (stock_num < 100 || buy_price < 1)
    return false;
  setIsHold(true);
  setBuyDate(buy_date);
  setBuyPrice(buy_price);
  setStockNum(stock_num);
  setTradingPosition(trade_position);
  principal_ = buy_price * stock_num;
  cash_ = principal_;
  return true;
}

bool HoldingStock::buy(double money, double trade_position) {
  setIsHold(true);
  setStockNum(money / buy_price_);
  setTradingPosition(1);
  principal_ = money;
  cash_ = principal_;
  return true;
}

int HoldingStock::countHoldDays() {
  hold_days_ = DateUtil::instance()->DaysBetween2Date(sale_date_, buy_date_);
  return hold_days_;
}

int HoldingStock::countProfit() {
  profit_ratio_ = (sell_price_ - buy_price_) / buy_price_;
  //profit_ = profit_ratio_ * principal_;
  profit_ = (profit_ratio_ - 0.003) * principal_;
  cash_ = principal_ + profit_;
  return profit_;
}

void HoldingStock::settle() {
}

bool HoldingStock::isShortPosition() {
  if (trading_position_ < 0.01)
    return true;
  else
    return false;
}

bool HoldingStock::isFullPosition() {
  if (trading_position_ > 0.99)
    return true;
  else
    return false;
}

void HoldingStock::printSelf() {
  LOG_MSG2("stock_code=%s,buy_price=%f,sell_price=%f",
           stock_code_.c_str(),
           buy_price_,
           sell_price_);
}

void HoldingStock::updateStopRatio(double price_ratio) {
  stop_ratio_ = price_ratio;
}

void HoldingStock::updateLimitedRatio(double price_ratio) {
  limited_ratio_ = price_ratio;
}

void HoldingStock::update() {
  stop_price_ = buy_price_ + buy_price_ * stop_ratio_;
  limited_price_ = buy_price_ + buy_price_ * limited_ratio_;
}

} /* namespace stock_logic */

double stock_logic::HoldingStock::getCost() const {
  return cost_;
}

void stock_logic::HoldingStock::setCost(double cost) {
  cost_ = cost;
}

const std::string& stock_logic::HoldingStock::getStockCode() const {
  return stock_code_;
}

void stock_logic::HoldingStock::setStockCode(const std::string& stockCode) {
  stock_code_ = stockCode;
}

bool stock_logic::HoldingStock::isIsHold() const {
  return is_hold_;
}

void stock_logic::HoldingStock::setIsHold(bool isHold) {
  is_hold_ = isHold;
}

double stock_logic::HoldingStock::getPrincipal() const {
  return principal_;
}

void stock_logic::HoldingStock::setPrincipal(double principal) {
  principal_ = principal;
}

const std::string& stock_logic::HoldingStock::getTradeDay() const {
  return trade_day_;
}

void stock_logic::HoldingStock::setTradeDay(const std::string& tradeDay) {
  trade_day_ = tradeDay;
}

double stock_logic::HoldingStock::getLimitedRatio() const {
  return limited_ratio_;
}

void stock_logic::HoldingStock::setLimitedRatio(double limitedRatio) {
  limited_ratio_ = limitedRatio;
}

double stock_logic::HoldingStock::getStopRatio() const {
  return stop_ratio_;
}

void stock_logic::HoldingStock::setStopRatio(double stopRatio) {
  stop_ratio_ = stopRatio;
}

double stock_logic::HoldingStock::getMaxPriceInHoldDays() const {
  return max_price_in_hold_days_;
}

void stock_logic::HoldingStock::setMaxPriceInHoldDays(
    double maxPriceInHoldDays) {
  max_price_in_hold_days_ = maxPriceInHoldDays;
}

double stock_logic::HoldingStock::getMinPriceInHoldDays() const {
  return min_price_in_hold_days_;
}

void stock_logic::HoldingStock::setMinPriceInHoldDays(
    double minPriceInHoldDays) {
  min_price_in_hold_days_ = minPriceInHoldDays;
}
