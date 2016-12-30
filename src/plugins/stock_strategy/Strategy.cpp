/*
 * Strategy.cpp
 *
 *  Created on: 2016年8月30日
 *      Author: Maibenben
 */

#include "Strategy.h"
#include "stock_factory.h"
#include "DateUtil.h"
#include <assert.h>

namespace stock_logic {

Strategy::Strategy() {
  sell_ratio_ = 0;
  stop_ratio_ = 0;
  max_hold_date_ = 0;
}

Strategy::~Strategy() {
  // YGTODO Auto-generated destructor stub
}

} /* namespace stock_logic */

int stock_logic::Strategy::getMaxHoldDate() {
  return max_hold_date_;
}

void stock_logic::Strategy::setMaxHoldDate(int maxHoldDate) {
  max_hold_date_ = maxHoldDate;
}

const std::string& stock_logic::Strategy::getName() const {
  return name_;
}

void stock_logic::Strategy::setName(const std::string& name) {
  name_ = name;
}

double stock_logic::Strategy::getSellRatio() const {
  return sell_ratio_;
}

void stock_logic::Strategy::setSellRatio(double sellRatio) {
  sell_ratio_ = sellRatio;
}

double stock_logic::Strategy::getStopRatio() const {
  return stop_ratio_;
}

void stock_logic::Strategy::setStopRatio(double stopRatio) {
  stop_ratio_ = stopRatio;
}

const std::string& stock_logic::Strategy::getType() const {
  return type_;
}

void stock_logic::Strategy::setType(const std::string& type) {
  type_ = type;
}

bool stock_logic::Strategy::getGoodStocks(StockAccount* account) {
  return true;
}

bool stock_logic::Strategy::setStockInfo(HoldingStock stock_info) {
  return true;
}

bool stock_logic::Strategy::toSale(std::string hist_date, HoldingStock& stock, int traded_day) {
  HistDataPerDay hist_data;
  const std::string& stock_code = stock.getStockCode();
  StockFactory* factory = StockFactory::GetInstance();
  LOG_MSG2("hist_date=%s,stock_code=%s", hist_date.c_str(),stock_code.c_str());
  if (factory->GetHistDataByDate(stock_code, hist_date, hist_data)) {
    double high = hist_data.high_;
    double low = hist_data.low_;
    double close = hist_data.close_;
    double limit_price = stock.getLimitedPrice();
    double stop_price = stock.getStopPrice();
    double buy_price = stock.getBuyPrice();
    const std::string& buy_date = stock.getBuyDate();
    LOG_MSG2("stock_code=%s,high=%f,low=%f,limit_price=%f,stop_price=%f,buy_date=%s,buy_price=%f",
             stock_code.c_str(),
             high,
             low,
             limit_price,
             stop_price,
             buy_date.c_str(),
             buy_price);
    int hold_day = DateUtil::instance()->DaysBetween2Date(hist_date, buy_date);
    LOG_MSG2("hold_day=%d,max_hold_date_=%d,max_traded_day=%d,traded_day=%d",
             hold_day,
             max_hold_date_,
             max_traded_day_,
             traded_day);
    HistDataPerDay hs300_hist_data;
    factory->GetHistDataByDate("hs300", hist_date, hs300_hist_data);
    double p_hs300_change = hs300_hist_data.day_yield_;
    LOG_MSG2("p_hs300_change=%f", p_hs300_change);
    if (traded_day > max_traded_day_) {
    //if (hold_day > max_hold_date_) {
      //stock.setCurrentPrice(stock.getMaxPriceInHoldDays());
      //stock.setCurrentPrice(stock.getMinPriceInHoldDays());
      stock.saleAsSuspension();
      //stock.sale(hist_date, low);
      LOG_MSG2("sale stocks, hold_day=%d,max_hold_date_=%d", hold_day, max_hold_date_);
      return true;
    }
    else {
      double pre_close = stock.current_price_;
      assert(pre_close > 1 && low > 1);
      double temp_stop_price = pre_close * (1 - 0.05);
      double temp_stop_to_buy_price = stock.buy_price_ * (1 - 0.05);
      if (temp_stop_price > low) {
        stock.sale(hist_date, temp_stop_price);
        LOG_MSG2("low to temp_stop_price=%f, sale stocks", temp_stop_price);
      }

      /*double temp_sale_price = pre_close * (1 + p_hs300_change + 0.01);
      if (high > temp_sale_price) {
        stock.sale(hist_date, temp_sale_price);
        LOG_MSG2("high to temp_sale_price=%f, sale stocks", temp_sale_price);
      }*/
      /*if (temp_stop_to_buy_price > low) {
        stock.sale(hist_date, temp_stop_to_buy_price);
        LOG_MSG2("low to temp_stop_to_buy_price=%f, sale stocks", temp_stop_to_buy_price);
      }*/
      stock.setCurrentPrice(close);
      stock.current_date_ = hist_date;
      if (stock.max_price_in_hold_days_ < high)
        stock.setMaxPriceInHoldDays(high);
      if (stock.min_price_in_hold_days_ > low && low > 1)
        stock.setMinPriceInHoldDays(low);
      //stock.setCurrentDate(hist_date);
    }

    /*if (high > stock.getLimitedPrice() && high > 1) {
      LOG_MSG("up to limited price");
      stock.sale(hist_date, limit_price);
    }
    else if (low < stop_price && low > 1) {
      LOG_MSG("low to stop price");
      stock.sale(hist_date, stop_price);
    }*/
    return true;
  }
  else
    return false;
}

void stock_logic::Strategy::Update(int opcode, stock_logic::Subject*) {

}
