/*
 * OffLineVisitStockObserver.h
 *
 *  Created on: 2016年8月5日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_OFFLINEVISITSTOCKOBSERVER_H_
#define GEORGE_MASTER_PLUGINS_STOCK_OFFLINEVISITSTOCKOBSERVER_H_

#include "Observer.h"
//#include "stock_basic_info.h"

namespace stock_logic {

class StockVisitInfo {
 public:
  StockVisitInfo() {
    stock_code_ = "";
    visit_num_ = 0;
    change_percent_ = 0;
    close_ = 0;
  }

  static inline bool cmp(const StockVisitInfo& t_visit_info,
              const StockVisitInfo& r_visit_info) {
          return t_visit_info.visit_num_ > r_visit_info.visit_num_;
      }
  std::string stock_code_;
  int visit_num_;
  double change_percent_;
  double close_;
};

class VisitInfo {
 public:

  void add_visit_info(std::string stock_code,
                      int visit_num);

  std::map<std::string, StockVisitInfo> stock_visit_info_;
};

class OffLineVisitStockObserver : public Observer {
 public:
  OffLineVisitStockObserver(stock_logic::Subject* subject);
  virtual ~OffLineVisitStockObserver();

  void Update(int opcode, stock_logic::Subject* subject);

  void UpdateStockOfflineVisitData(stock_logic::Subject* subject);

  void UpdateStockHistData(stock_logic::Subject* subject);

  base_logic::DictionaryValue* Request(base_logic::DictionaryValue* params);

  void CountVisitDataPerDay(std::string stock_code,
                           std::map<std::string, DataPerDay>& data_per_day);

  std::map<std::string, VisitInfo> visit_info_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_OFFLINEVISITSTOCKOBSERVER_H_ */
