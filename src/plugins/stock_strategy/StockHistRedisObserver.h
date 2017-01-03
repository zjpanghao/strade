/*
 * StockHistRedisObserver.h
 *
 *  Created on: 2016年8月23日
 *      Author: harvey
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_STOCKHISTREDISOBSERVER_H_
#define GEORGE_MASTER_PLUGINS_STOCK_STOCKHISTREDISOBSERVER_H_

#include "Observer.h"
#include <iostream>
#include <sstream>
#include "basic/scoped_ptr.h"
#include "storage/redis_controller.h"
#include "stock_redis_db.h"

#include <math.h>
#include <fstream>

namespace stock_logic {

namespace {

//总共一组数据 key: hash_name, value: zset
typedef std::map<std::string, std::map<std::string, double> > ALL_ONCE_REDIS_HIST_MAP;

//每天一组数据 key: hash_name, value: zset
typedef std::map<std::string, std::map<std::string, double> > STOCK_EVERY_DAY_REDIS_MAP;

typedef std::map<std::string, std::map<std::string, std::string> > STOCK_SPECIAL_REDIS_INFO_MAP;

class StockRedisDateInfo {
 public:
  void WriteStockHistPerDay(const std::string &stock_code,
                            HistDataPerDay *hist_data_info);

 private:
  //历史数据每日一组
  void PrepareWriteRedisHistInfo(HistDataPerDay *hist_date_info) {
    hist_write_redis_map_.clear();
    // key 为 redis_key, value 为  zset score, zset value 为股票代码
    //涨跌幅，收益率
    hist_write_redis_map_["change_percent"] = hist_date_info->day_yield_;
    //振幅
    hist_write_redis_map_["amplitude"] = hist_date_info->amplitude_;
    //换手率
    hist_write_redis_map_["turnoverratio"] = hist_date_info->turnoverratio_;
    //成交量
    hist_write_redis_map_["volume"] = hist_date_info->volume_;
    //成交额
    hist_write_redis_map_["turnover"] = hist_date_info->obv_;
    //股价
    if (hist_date_info->close_ > 0.0) {
      hist_write_redis_map_["share_price"] = hist_date_info->close_;
    }
    
    //涨幅
    if (hist_date_info->day_yield_ > 0.0) {
      hist_write_redis_map_["inc_change_percent"] = hist_date_info->day_yield_;
    }
    //跌幅
    if (hist_date_info->day_yield_ < 0.0) {
      hist_write_redis_map_["dec_change_percent"] = fabs(hist_date_info->day_yield_);
    }
    //涨跌幅
    hist_write_redis_map_["absolute_change_percent"] = fabs(hist_date_info->day_yield_);

    //TUDO 其他
  }
 private:
  std::map<std::string, double> hist_write_redis_map_;
};

class StockRedisTotalInfo {
 public:

  // key: date, value: stock_redis_info   //历史数据
  typedef std::map<std::string, StockRedisDateInfo *> STOCK_HIST_REDIS_MAP;

  // key: timestamp, value: stock_redis_info  //实时数据
  typedef std::map<int, StockRedisDateInfo *> STOCK_YIELD_REDIS_MAP;

  StockRedisTotalInfo(StockBasicInfo &stock_base_info);
  ~StockRedisTotalInfo();

 public:
  void GetOrCreateRedisStockInfo(std::string &date,
                                 StockRedisDateInfo **stock_hist_info);
  bool GetStockRecentUpateDate();
  void OnUpdate(int opcode);
  void OnInitWriteRedisOnce();

 private:
  void OnWriteStockRedisAllHist();

 private:
  //总共一组数据
  void PrepareWriteRedisOnceInfo() {
    once_write_redis_map_.clear();
    StockholderInfo &stock_holder_info = stock_base_info_.holder_info_;
    std::string &stock_code = stock_base_info_.code_;

    // key 为 redis_key, value 为  zset value, zset key 为股票代码
    //总股本
    once_write_redis_map_["total_equity"] = stock_base_info_.totals_;
    //流通股本
    once_write_redis_map_["float_equity"] = stock_base_info_.outstanding_;
    //总市值
    once_write_redis_map_["market_value"] = stock_base_info_.market_value_;
    //流通市值
    once_write_redis_map_["liquid_market_value"] = stock_base_info_.liquidMarketValue_;
    //流通比例
    once_write_redis_map_["liquid_scale"] = stock_base_info_.liquidScale_;

    //获得最新持股信息
    if (!stock_holder_info.stockholder_map_.empty()) {
      std::map<std::string, StockholderPerQuarter>::reverse_iterator reverse_iter(
          stock_holder_info.stockholder_map_.rbegin());
      StockholderPerQuarter &stock_holder_per_quarter = reverse_iter->second;
      //十大股东持股比例
      once_write_redis_map_["top_stock_ratio"] = stock_holder_per_quarter.top_stock_ratio_;
      //股东户数
      once_write_redis_map_["holder_count"] = stock_holder_per_quarter.holder_count_;
      //户均持股数
      once_write_redis_map_["float_stock_num"] = stock_holder_per_quarter.float_stock_num_;

      //增减持
      once_write_redis_map_["change_in_holding"] = stock_holder_info.identifier_;

      std::map<std::string, std::string> &over_under_holding_map =
          total_once_map["over_or_under_weight_holding"];
      if (1 == stock_holder_info.identifier_) {
        //增持
        std::string &over_weight_stocks = over_under_holding_map["高管增股"];
        if (over_weight_stocks.empty()) {
          over_weight_stocks = stock_code;
        } else {
          over_weight_stocks = over_weight_stocks + "," + stock_code;
        }
      } else if (0 == stock_holder_info.identifier_) {
        //减持
        std::string &under_weight_stocks = over_under_holding_map["高管减股"];
        if (under_weight_stocks.empty()) {
          under_weight_stocks = stock_code;
        } else {
          under_weight_stocks = under_weight_stocks + "," + stock_code;
        }
      }

      //机构持股
      once_write_redis_map_["institution_stock_num"] = stock_holder_per_quarter.institution_stock_num_;
    } else {
      LOG_ERROR2("PrepareWriteRedisOnceInfo StockholderInfo::stockholder_map Empty, stock_code = %s",
                 stock_code.c_str());
    }
  }

 private:
  std::string recent_update_date_;
  StockBasicInfo &stock_base_info_;
  std::map<std::string, double> once_write_redis_map_;

 public:
  STOCK_HIST_REDIS_MAP stock_hist_redis_map_;         //股票历史信息
  STOCK_YIELD_REDIS_MAP stock_yield_redis_map_;       //实时信息

  static STOCK_SPECIAL_REDIS_INFO_MAP total_once_map;
};

}

class StockHistRedisObserver : public Observer {
 public:
  // key: stock_code, value: total_stock_info
  typedef std::map<std::string, StockRedisTotalInfo *> ALL_STOCK_REDIS_MAP;

  StockHistRedisObserver(Subject *subject);
  virtual ~StockHistRedisObserver();

 private:
  void GetOrCreateStockRedisTotalInfo(StockBasicInfo &stock_base_info,
                                      StockRedisTotalInfo **stock_redis_info);

  void UpdateSpecialStockInfo();

 public:
  virtual void Update(int opcode, stock_logic::Subject *subject);

 private:
  base_logic::RedisController *redis_engine_;
  ALL_STOCK_REDIS_MAP all_stock_redis_map_;

  ALL_ONCE_REDIS_HIST_MAP all_once_redis_hist_map_;

 public:

};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_STOCKHISTREDISOBSERVER_H_ */
