//  Copyright (c) 2015-2015 The geprge Authors. All rights reserved.
//  Created on: 2016年5月18日 Author: kerry

#ifndef GEORGE_VIP_DB_H_
#define GEORGE_VIP_DB_H_

#include <string>
#include <list>
#include <map>
#include "logic/base_values.h"
#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "basic/scoped_ptr.h"
#include "stock_basic_info.h"
#include "storage/data_engine.h"

namespace stock_logic {

class StockDB {
 public:
  StockDB(config::FileConfig* config);
  virtual ~StockDB();
 public:

  bool FecthHexunDailyData(
        std::string min_time,
        std::map<std::string, stock_logic::StockTotalInfo>& map);

  bool FectchStockHoldersInfo(
        std::map<std::string, stock_logic::StockTotalInfo>& map);

  bool FectchStockShareInfo(
      std::map<std::string, stock_logic::StockTotalInfo>& map);

  bool FectchStockBasicInfo(
      std::map<std::string, stock_logic::StockTotalInfo>& map);

  bool FectchStockHistData(
      std::string stock_code,
      std::map<std::string, stock_logic::StockTotalInfo>& map);

  bool FectchStockDayKLineData(
      std::string max_time,
      std::map<std::string, stock_logic::StockTotalInfo>& map);

  bool FectchStockVisitData(
        int min_time,
        int max_time,
        std::map<std::string, stock_logic::StockTotalInfo>& map);

  bool FecthOffLineVisitData(
      std::string min_time,
      std::map<std::string, stock_logic::StockTotalInfo>& map);

  bool FectchIndustryInfo(stock_logic::IndustryInfo& map);

  bool FectchEventsInfo(stock_logic::IndustryInfo& map);

  bool FetchYieldData(
      std::map<std::string, stock_logic::StockTotalInfo>& stock_total_info);

  bool UpdateRealtimeStockInfo(
      std::map<std::string, stock_logic::StockTotalInfo>& stock_total_info);

  bool UpdateALLRealtimeStockInfo(
      std::map<std::string, stock_logic::StockTotalInfo>& stock_total_info);

  bool UpdateWeekMonthData(
      std::map<std::string, stock_logic::StockTotalInfo>& stock_total_info);

  bool GetLimitData(std::map<int, std::vector<int> >& market_limit_info);

  bool WriteLimitData(int time, int surged_count, int decline_count);

  bool DeleteOldYieldInfo(int end_time);

  bool LoadCustomEvent(stock_logic::IndustryInfo& map);

  bool UpdateYieldInfo(std::string code, int trade_time, double yield);

  bool BatchUpdateYieldInfo(std::string code,
                            std::map<int, YieldInfoUnit>& yield_info);

 public:

  static void CallFectchHexunDailyData(void* param, base_logic::Value* value);

  static void CallFectchStockHoldersInfo(void* param, base_logic::Value* value);

  static void CallFectchStockShareInfo(void* param, base_logic::Value* value);

  static void CallFectchStockInfo(void* param, base_logic::Value* value);

  static void CallFectchStockHistData(void* param, base_logic::Value* value);

  static void CallFectchStockVisitData(void* param, base_logic::Value* value);

  static void CallFectchOfflineVisitData(void* param, base_logic::Value* value);

  static void CallFectchIndustryInfo(void* param, base_logic::Value* value);

  static void CallFectchEventsInfo(void* param, base_logic::Value* value);

  static void CallLoadCustomEventsInfo(void* param, base_logic::Value* value);

  static void CallFectchRealtimeStockInfo(void* param,
                                          base_logic::Value* value);

  static void CallFectchGetLimitData(void* param, base_logic::Value* value);

  static void CallFectchWeekMonthData(void* param, base_logic::Value* value);

  static void CallFecthYieldData(void* param, base_logic::Value* value);
// private:
  base_logic::DataEngine* mysql_engine_;
};
}
#endif
