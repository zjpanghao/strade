//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月15日 Author: kerry

#ifndef GEORGE_VIP_BASIC_INFO_H_
#define GEORGE_VIP_BASIC_INFO_H_

#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "logic/logic_comm.h"
#include <math.h>
#include <list>
#include <set>
#include <string>
#include <limits>
#include "stock_proto_buf.h"
#include "StockholderInfo.h"
#include "ExtAttribute.h"
//#include "StockFilter.h"
//#include "WeightAnalyzer.h"

namespace stock_logic {

#define HSSANBAI "hs300"

typedef struct YieldInfoUnit {
  YieldInfoUnit() {
    trade_time_ = 0;
    change_percent_ = 0;
    trade_ = 0.0;
    open_ = 0;
    high_ = 0;
    low_ = 0;
    settlement_ = 0;
    volume_ = 0;
    yield_ = 0.0;
    price_change_direction_ = 0;
    price_change_ = 0;
    visit_count_ = -1;
  }

  void set_yield_data(int trade_time, double yield) {
    trade_time_ = trade_time;
    yield_ = yield;
  }

  void set_all_data(int trade_time, double change_percent, double trade,
                    double open, double high, double low, double settlement,
                    double volume, double yield) {
    trade_time_ = trade_time;
    change_percent_ = change_percent;
    trade_ = trade;
    open_ = open;
    high_ = high;
    low_ = low;
    settlement_ = settlement;
    volume_ = volume;
    yield_ = yield;
  }

  int trade_time_;
  double change_percent_;
  double trade_;
  double open_;
  double high_;
  double low_;
  double settlement_;
  double volume_;
  double yield_;
  //相对于上一分钟的价格变化方向，涨则为1，跌则为-1，平则为0
  int price_change_direction_;
  //相对于上一分钟的价格变化量
  double price_change_;
  int visit_count_;
} YieldInfoUnit;

typedef struct HistDataPerDay {
public:

  HistDataPerDay() {
    date_ = "";
    month_init_date_ = "";
    open_ = 0.0;
    high_ = 0.0;
    close_ = 0.0;
    low_ = 0.0;
    support_pos_ = 0.0;
    pressure_pos_ = 0.0;
    mid_price_ = 0.0;
    qfq_close_ = 0.0;
    day_yield_ = 0.0;
    adjusted_day_yield_ = 0.0;
    month_init_price_ = 0.0;
    day_volatility_ = 0.0;
    adjusted_day_volatility_ = 0.0;
    visit_num_ = 0;
    amplitude_ = 0;
    turnoverratio_ = 0;
    volume_ = 0.0;
    obv_ = 0;
    ma5_ = 0;
    ma10_ = 0;
    ma20_ = 0;
    v_ma5_ = 0;
    v_ma10_ = 0;
    v_ma20_ = 0;
    yingxian_length_ = 0;
    shiti_length_ = 0;
  }
  std::string date_;
  std::string month_init_date_;
  double open_;
  double high_;
  double close_;                                    //股价
  double low_;
  double support_pos_;
  double pressure_pos_;
  double mid_price_;
  double qfq_close_;
  double day_yield_;                                //涨跌幅,当日收益率
  double adjusted_day_yield_;
  double month_init_price_;
  double day_volatility_;                          //(最高价-最低价)/昨日收盘价
  double adjusted_day_volatility_;
  int visit_num_;
  double amplitude_;                               //振幅
  double turnoverratio_;                           //换手率
  double volume_;                                  //成交量
  double obv_;                                     //成交额
  double ma5_;
  double ma10_;
  double ma20_;
  double v_ma5_;
  double v_ma10_;
  double v_ma20_;
  double yingxian_length_;
  double shiti_length_;
} HistDataPerDay;

class StockUtil {
 public:

  StockUtil() {
    holiday_in_2016_.insert("2016-06-09");
    holiday_in_2016_.insert("2016-06-10");
    holiday_in_2016_.insert("2016-09-15");
    holiday_in_2016_.insert("2016-09-16");
    holiday_in_2016_.insert("2016-10-01");
    holiday_in_2016_.insert("2016-10-02");
    holiday_in_2016_.insert("2016-10-03");
    holiday_in_2016_.insert("2016-10-04");
    holiday_in_2016_.insert("2016-10-05");
    holiday_in_2016_.insert("2016-10-06");
    holiday_in_2016_.insert("2016-10-07");

  }

  static StockUtil* Instance() {
    if (NULL == instance_)
      instance_ = new StockUtil();
    return instance_;
  }

  bool check_double_valid(double d) {
    double d2compare = d + 1;
    char d_str[20];
    char d2_str[20];
    sprintf(d_str, "%f", d);
    sprintf(d2_str, "%f", d2compare);
    return strcmp(d_str, d2_str) != 0;
  }

  std::string get_first_day_of_month() {
    std::string first_day_of_month;
    time_t current_time = time(NULL);
    struct tm* current_tm = localtime(&current_time);
    char str_time[20];
    sprintf(str_time, "%d-%02d-%02d", current_tm->tm_year + 1900,
            current_tm->tm_mon + 1, 1);
    first_day_of_month = str_time;
    return first_day_of_month;
  }

  std::string get_first_day_of_week() {
    std::string first_day_of_week;
    time_t current_time = time(NULL);
    struct tm* current_tm = localtime(&current_time);
    int week = current_tm->tm_wday;
    int diff = 0;
    if (0 == week)
      diff = 7 * 24 * 60 * 60;
    else
      diff = (week - 1) * 24 * 60 * 60;
    long seconds_of_first_day_in_week = current_time - diff;
    struct tm*first_day_in_week_tm = localtime(&seconds_of_first_day_in_week);
    char str_time[20];
    sprintf(str_time, "%d-%02d-%02d", first_day_in_week_tm->tm_year + 1900,
            first_day_in_week_tm->tm_mon + 1, first_day_in_week_tm->tm_mday);
    first_day_of_week = str_time;
    return first_day_of_week;
  }

  int get_current_hour() {
    time_t current_time = time(NULL);
    struct tm* current_tm = localtime(&current_time);
    return current_tm->tm_hour;
  }

  std::string get_current_day_str() {
    time_t current_time = time(NULL);
    struct tm* current_tm = localtime(&current_time);
    char str_today[20];
    sprintf(str_today, "%d-%02d-%02d", current_tm->tm_year + 1900,
            current_tm->tm_mon + 1, current_tm->tm_mday);
    return std::string(str_today);
  }

  bool is_trading_time() {
    time_t current_time = time(NULL);
    struct tm* current_tm = localtime(&current_time);
    //LOG_MSG2("current_tm->tm_wday=%d", current_tm->tm_wday);
    if (0 == current_tm->tm_wday || 6 == current_tm->tm_wday)
      return false;
    char str_hour_time[20];
    sprintf(str_hour_time, "%02d%02d%02d", current_tm->tm_hour,
            current_tm->tm_min, current_tm->tm_sec);
    //LOG_MSG2("str_hour_time=%s", str_hour_time);
    if (strcmp(str_hour_time, "092500") < 0
        || strcmp(str_hour_time, "150000") > 0
        || (strcmp(str_hour_time, "113000") > 0
            && strcmp(str_hour_time, "130000") < 0))
      return false;
    char str_today[20];
    sprintf(str_today, "%d-%02d-%02d", current_tm->tm_year + 1900,
            current_tm->tm_mon + 1, current_tm->tm_mday);
    //LOG_MSG2("str_today=%s", str_today);
    if (holiday_in_2016_.end() != holiday_in_2016_.find(str_today)) {
      //LOG_MSG2("return false%d",1);
      return false;
    }
    //LOG_MSG2("return true%d",1);
    return true;
  }

  bool is_trading_day() {
    time_t current_time = time(NULL);
    struct tm* current_tm = localtime(&current_time);
    if (0 == current_tm->tm_wday || 6 == current_tm->tm_wday)
      return false;
    char str_today[20];
    sprintf(str_today, "%d-%02d-%02d", current_tm->tm_year + 1900,
            current_tm->tm_mon + 1, current_tm->tm_mday);
    if (holiday_in_2016_.end() != holiday_in_2016_.find(str_today)) {
      return false;
    }
    return true;
  }

  void stock_split(std::string& s, std::string& delim,
                   std::vector<std::string>& ret) {
    size_t last = 0;
    size_t index = s.find_first_of(delim, last);
    while (index != std::string::npos) {
      ret.push_back(s.substr(last, index - last));
      last = index + 1;
      index = s.find_first_of(delim, last);
    }
    if (index - last > 0) {
      ret.push_back(s.substr(last, index - last));
    }
  }

  void delete_old_yield_info(
      std::map<int, YieldInfoUnit>& industry_yield_infos) {
    int yields_size = industry_yield_infos.size();
    if (yields_size < 1)
      return;
    int latest_trade_time = 0;
    for (std::map<int, YieldInfoUnit>::reverse_iterator iter =
        industry_yield_infos.rbegin(); iter != industry_yield_infos.rend();) {
      if (0 == latest_trade_time) {
        latest_trade_time = iter->first;
        latest_trade_time -= 10 * 3600;
        break;
      }
    }
    for (std::map<int, YieldInfoUnit>::iterator iter = industry_yield_infos
        .begin(); iter != industry_yield_infos.end();) {
      if (latest_trade_time > iter->first) {
        industry_yield_infos.erase(iter++);
      } else
        iter++;
    }
  }

  bool serialize(base_logic::DictionaryValue* value, std::string& json) {
    base_logic::ValueSerializer* serializer =
        base_logic::ValueSerializer::Create(base_logic::IMPL_JSON);
    bool r = serializer->Serialize(*value, &json);
    delete serializer;
    serializer = NULL;
    return r;
  }

  bool jsonp_serialize(base_logic::DictionaryValue* value, std::string& jsonp) {
    base_logic::ValueSerializer* serializer =
        base_logic::ValueSerializer::Create(base_logic::IMPL_JSONP);
    bool r = serializer->Serialize(*value, &jsonp);
    delete serializer;
    serializer = NULL;
    return r;
  }

  bool wrap_jsonp_serialize(stock_logic::net_reply::VIPNewsList* value,
                            george_logic::AttachField* attach,
                            std::string list_info, std::string& wraped_jsonp) {
    if (NULL != attach)
      value->attach_field()->set_callback(attach->callback());
    base_logic::ValueSerializer* serializer =
        base_logic::ValueSerializer::Create(base_logic::IMPL_JSONP);
    bool r = serializer->Serialize(*value->packet(), &wraped_jsonp);
    delete serializer;
    serializer = NULL;
    return r;
  }

  void filter_character(std::string& dest_str, const char c) {
    std::string filtered_str = "";
    for (int i = 0; i < dest_str.length(); i++) {
      if (dest_str[i] == c)
        continue;
      filtered_str += dest_str[i];
    }
    dest_str = filtered_str;
  }

  void filter_redundant_quotes(std::string& src_str) {
    //LOG_MSG2("src_str=%s", src_str.c_str());
    std::string jsonp_result_str = "\"jsonp_result\"";
    std::size_t found = src_str.find(jsonp_result_str);
    found++;
    found += jsonp_result_str.length();
    if (found != std::string::npos) {
      while (src_str[found] != '"') {
        found++;
      }
      src_str[found] = ' ';
    }
    found = src_str.find("\"list\": [  ]");
    found--;
    if (found != std::string::npos) {
      while (src_str[found] != '"') {
        found--;
      }
      src_str[found] = ' ';
    } LOG_MSG2("src_str=%s", src_str.c_str());
  }

  bool get_json_of_hist_data(
      std::map<std::string, HistDataPerDay>& hs300_hist_data,
      std::string start_date,
      std::string& end_date,
      std::map<std::string, HistDataPerDay>& hist_data,
      std::string& json_str) {
    bool r = true;
    json_str = "";
    LOG_MSG2("start_date=%s,end_date=%s,hist_data.size=%d",
             start_date.c_str(),
             end_date.c_str(),
             hist_data.size());
    std::map<std::string, HistDataPerDay>::iterator start_date_iter = hist_data
        .begin();
    double pre_adjusted_yield = -100;
    double hs300_pre_adjusted_yield = -100;
    double hs300_init_price = -100;
    double init_price = -100;
    std::string current_day_str = StockUtil::Instance()->get_current_day_str();
    for (; start_date_iter != hist_data.end(); start_date_iter++) {
      std::string t_date = start_date_iter->first;
      //TODO去掉当天数据，由于tushare更新不及时可能导致误差
      if (t_date == current_day_str)
        continue;
      if (start_date <= t_date && t_date <= end_date) {
        if (hs300_init_price < 1) {
          //TODO更新起始日期
          start_date = t_date;
          hs300_init_price = hs300_hist_data[t_date].close_;
          hs300_pre_adjusted_yield =
              hs300_hist_data[t_date].adjusted_day_yield_ = 0.0;
          pre_adjusted_yield = 1.0;
          start_date_iter->second.adjusted_day_yield_ = 1.0;
        } else {
          double hs300_close = hs300_hist_data[t_date].close_;
          //TODO去除脏数据
          if (1 > hs300_close)
            hs300_hist_data[t_date].adjusted_day_yield_ =
                hs300_pre_adjusted_yield;
          else {
            hs300_hist_data[t_date].adjusted_day_yield_ =
                (hs300_hist_data[t_date].close_ - hs300_init_price)
                    / hs300_init_price;
            hs300_pre_adjusted_yield = hs300_hist_data[t_date]
                .adjusted_day_yield_;
          }
          start_date_iter->second.adjusted_day_yield_ = pre_adjusted_yield
              + pre_adjusted_yield * start_date_iter->second.day_yield_;
          pre_adjusted_yield = start_date_iter->second.adjusted_day_yield_;
        }
      }
    }
    stock_logic::net_reply::VIPNewsList* vip_list =
        new stock_logic::net_reply::VIPNewsList();
    std::map<std::string, HistDataPerDay>::reverse_iterator iter = hist_data
        .rbegin();
    double hs300_adjusted_day_yield = 0.0;
    for (; iter != hist_data.rend(); iter++) {
      if (current_day_str == iter->first)
        continue;
      if (end_date < iter->first)
        continue;
      if (start_date > iter->first) {
        LOG_MSG2("break!start_date=%s,iter->first=%s", start_date.c_str(), iter->first.c_str());
        break;
      }
      HistDataPerDay& data_per_day = iter->second;
      if (!StockUtil::Instance()->check_double_valid(data_per_day.day_yield_))
        continue;
      stock_logic::net_reply::STOIndustryData* kline_data =
          new stock_logic::net_reply::STOIndustryData();

      std::string trade_day = iter->first;
      std::map<std::string, HistDataPerDay>::iterator hs300_iter =
          hs300_hist_data.find(trade_day);
      if (hs300_hist_data.end() != hs300_iter) {
        kline_data->setHS300DayYield(hs300_iter->second.close_);
        hs300_adjusted_day_yield = hs300_iter->second.adjusted_day_yield_;
        kline_data->setHS300AdjustedDayYield(hs300_adjusted_day_yield);
      } else {
        kline_data->setHS300DayYield(hs300_adjusted_day_yield);
      }
      kline_data->setDayYield(data_per_day.day_yield_);
      kline_data->setAdjustedDayYield(data_per_day.adjusted_day_yield_ - 1);
      kline_data->setClose(data_per_day.close_);
      kline_data->setDate(iter->first);
      LOG_MSG2("date=%s", iter->first.c_str());
      vip_list->set_vip_news(kline_data->get());
      delete kline_data;
      kline_data = NULL;
    }
    base_logic::DictionaryValue* kline_value = vip_list->packet();
    r = serialize(kline_value, json_str);
    delete kline_value;
    kline_value = NULL;
    delete vip_list;
    vip_list = NULL;
    return r;
  }

  bool get_json_of_yield_data(std::map<int, YieldInfoUnit>& hs300_yield_data,
                              std::map<int, YieldInfoUnit>& yield_infos,
                              std::string& json_str,
                              std::string& name) {
    LOG_MSG2("get_json_of_yield_data,name=%s", name.c_str());
    bool r = true;
    double last_hs300_yield = 0.0;
    json_str = "";
    std::map<int, YieldInfoUnit>::iterator iter = yield_infos.begin();
    int start_time = iter->first;
    double open_price = iter->second.yield_;
    stock_logic::net_reply::VIPNewsList* vip_list =
        new stock_logic::net_reply::VIPNewsList();
    for (; iter != yield_infos.end(); iter++) {
      stock_logic::net_reply::STOIndustryData* yield_data =
          new stock_logic::net_reply::STOIndustryData();
      YieldInfoUnit& unit = iter->second;
      int trade_time = iter->first;
      std::map<int, YieldInfoUnit>::iterator hs300_iter = hs300_yield_data.find(
          trade_time);
      if (hs300_yield_data.end() != hs300_iter) {
        //last_hs300_yield = hs300_iter->second.trade_;
        last_hs300_yield = hs300_iter->second.yield_ / 100;
        yield_data->setHS300DayYield(last_hs300_yield);
      } else {
        yield_data->setHS300DayYield(last_hs300_yield);
      }
      yield_data->setDayYield(unit.yield_ / 100);
      if (name == "visit") {
        if (unit.visit_count_ != -1)
          yield_data->setVisit(unit.visit_count_);
        else
          yield_data->setVisit(0);
      }
      LOG_MSG2("yield_=%f,trade_time_=%d,visit_count=%d",
               unit.yield_/100,
               unit.trade_time_,
               unit.visit_count_);
      yield_data->setTime(unit.trade_time_);
      vip_list->set_vip_news(yield_data->get());
      delete yield_data;
      yield_data = NULL;
    }
    base_logic::DictionaryValue* yield_values = vip_list->packet();
    r = serialize(yield_values, json_str);
    delete yield_values;
    yield_values = NULL;
    delete vip_list;
    vip_list = NULL;
    return r;
  }

  base_logic::DictionaryValue* get_backtest_of_hist_data(
      std::map<std::string, HistDataPerDay>& hs300_hist_data,
      std::string start_date,
      std::string& end_date,
      std::map<std::string, HistDataPerDay>& hist_data);

  static StockUtil* instance_;
  std::set<std::string> holiday_in_2016_;
};

class DataPerDay;

class StockBasicInfo {
 public:

  StockBasicInfo() {
    code_ = "";
    name_ = "";
    industry_ = "";
    totalAssets_ = 0.0;
    bvps_ = 0.0;
    pb_ = 0.0;
    market_value_ = 0.0;
    current_trade_ = 0.0;
    current_trade_time_ = 0;
    current_visit_num_ = 0;
    current_visit_time_ = 0;
    current_open_ = 0.0;
    current_high_ = 0.0;
    current_low_ = 0.0;
    current_settlement_ = 0.0;
    surged_limit_price_ = 0.0;
    decline_limit_price_ = 0.0;
    change_percent_ = 0.0;
    week_change_percent_ = 0.0;
    month_change_percent_ = 0.0;
    volume_ = 0.0;
    week_volume_ = 0.0;
    month_volume_ = 0.0;
    week_trade_day_ = "";
    month_trade_day_ = "";
    is_surged_limit_ = false;
    is_decline_limit_ = false;
    qfq_close_ = 0.0;
    qfq_high_ = 0.0;
    qfq_low_ = 0.0;
    qfq_open_ = 0.0;
    pe_ = 0;
    reserved_ = 0;
    reservedPerShare_ = 0;
    liquidAssets_ = 0;
    fixedAssets_ = 0;
    outstanding_ = 0;
    totals_ = 0;
    eps_ = 0;
    amplitude_ = 0;
    chengjiaoe_ = 0;
    turnoverratio_ = 0;
    liquidMarketValue_ = 0;
    liquidScale_ = 0;
  }

  std::string code() {
    return code_;
  }

  void set_code(std::string code) {
    code_ = code;
  }

  std::string week_trade_day() {
    return week_trade_day_;
  }

  void set_week_trade_day(std::string week_trade_day) {
    week_trade_day_ = week_trade_day;
  }

  std::string month_trade_day() {
    return month_trade_day_;
  }

  void set_month_trade_day(std::string month_trade_day) {
    month_trade_day_ = month_trade_day;
  }

  std::string name() {
    return name_;
  }

  void set_name(std::string name) {
    name_ = name;
  }

  std::string industry() {
    return industry_;
  }

  void set_industry(std::string industry) {
    industry_ = industry;
  }

  double volume() const {
    return volume_;
  }

  void set_volume(double volume) {
    volume_ = volume;
  }

  double week_volume() const {
    return week_volume_;
  }

  void set_week_volume(double week_volume) {
    week_volume_ = week_volume;
  }

  double month_volume() const {
    return month_volume_;
  }

  void set_month_volume(double month_volume) {
    month_volume_ = month_volume;
  }

  double change_percent() {
    return change_percent_;
  }

  void set_change_percent(double change_percent) {
    change_percent_ = change_percent;
  }

  double week_change_percent() {
    return week_change_percent_;
  }

  void set_week_change_percent(double week_change_percent) {
    week_change_percent_ = week_change_percent;
  }

  double month_change_percent() {
    return month_change_percent_;
  }

  void set_month_change_percent(double month_change_percent) {
    month_change_percent_ = month_change_percent;
  }

  double totalAssets() {
    return totalAssets_;
  }

  void set_totalAssets(double totalAssets) {
    totalAssets_ = totalAssets;
  }

  double bvps() {
    return bvps_;
  }

  void set_bvps(double bvps) {
    bvps_ = bvps;
  }

  double pb() {
    return pb_;
  }

  void set_pb(double pb) {
    pb_ = pb;
  }

  double market_value() {
    return totalAssets_ * bvps_ * pb_;
  }

  void set_market_value(double market_value) {
    market_value_ = market_value;
  }

  double current_trade() {
    return current_trade_;
  }

  void set_current_trade(double current_trade) {
    current_trade_ = current_trade;
  }

  double current_open() {
    return current_open_;
  }

  void set_current_open(double current_open) {
    current_open_ = current_open;
  }

  int current_trade_time() {
    return current_trade_time_;
  }

  void set_current_trade_time(double current_trade_time) {
    current_trade_time_ = current_trade_time;
  }

  bool is_surged_limit() {
    return is_surged_limit_;
  }

  bool is_decline_limit() {
    return is_decline_limit_;
  }

  double qfq_close() {
    return qfq_close_;
  }

  void set_qfq_close(double qfq_close) {
    qfq_close_ = qfq_close;
  }

  double round(double r) {
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
  }

  double get_surged_limit_price() {
    double rounded_range = round(current_settlement_ * 0.1 * 1e3) / 1e3;
    surged_limit_price_ = rounded_range + current_settlement_;
    return surged_limit_price_;
  }

  double get_decline_limit_price() {
    double rounded_range = round(current_settlement_ * 0.1 * 1e3) / 1e3;
    decline_limit_price_ = current_settlement_ - rounded_range;
    return decline_limit_price_;
  }

  void check_limit() {
    get_surged_limit_price();
    get_decline_limit_price();
    if (current_trade_ > current_settlement_) {
      double diff = surged_limit_price_ - current_trade_;
      if (diff < 0.01 && diff > -0.01)
        is_surged_limit_ = true;
    } else if (current_trade_ < current_settlement_) {
      double diff = decline_limit_price_ - current_trade_;
      if (diff < 0.01 && diff > -0.01) {
        is_decline_limit_ = true;
      }
    }
  }

  void add_yield_info(int trade_time, double trade, double yield) {
    if (trade_time < 1467259129)
      return;
    YieldInfoUnit unit;
    unit.trade_time_ = trade_time;
    unit.trade_ = trade;
    unit.yield_ = yield;
    yield_infos_[trade_time] = unit;
  }

  void add_visit_info(int trade_time, int count) {
    update_current_visit_info(trade_time, count);
    std::map<int, YieldInfoUnit>::iterator iter = yield_infos_.find(trade_time);
    if (iter == yield_infos_.end())
      return;
    YieldInfoUnit& unit = iter->second;
    unit.visit_count_ = count;
    if (code_ == "601288" || code_ == "000002")
      LOG_MSG2("code=%s,trade_time=%d,visit_count=%d",
               code_.c_str(),
               trade_time,
               count);
  }

  bool get_visit_by_time(int trade_time, int& visit_count) {
    std::map<int, YieldInfoUnit>::iterator iter = yield_infos_.find(trade_time);
    if (iter == yield_infos_.end())
      return false;
    visit_count = iter->second.visit_count_;
    if (-1 == visit_count)
      return false;
    else
      return true;
  }

  double get_yield_by_time(int trade_time) {
    std::map<int, YieldInfoUnit>::iterator iter = yield_infos_.find(trade_time);
    if (iter != yield_infos_.end()) {
      return iter->second.yield_;
    }
    return 0.0;
  }

  std::map<int, YieldInfoUnit>& get_yield_infos() {
    return yield_infos_;
  }

  void ValueSerialization(base_logic::DictionaryValue* dict);

  void RealtimeValueSerialization(base_logic::DictionaryValue* dict);

  void AllRealtimeValueSerialization(base_logic::DictionaryValue* dict);

  void MonthWeekDataValueSerialization(base_logic::DictionaryValue* dict);

  void YieldValueSerialization(base_logic::DictionaryValue* dict);

  void VisitDataSerialization(base_logic::DictionaryValue* dict);

  void OfflineVisitSerialization(base_logic::DictionaryValue* dict);

  void UpdateTodayKLine();

  bool add_offline_visit_data(std::string date, int hour, int visit_num);

  int current_visit_time();

  void set_current_visit_time(int current_visit_time);

  int current_visit_num();

  void set_current_visit_num(int current_visit_num);

  void update_current_visit_info(int visit_time, int visit_num);

  void update_amplitude();

  double update_liquidScale();

  void update_chengjiaoe();

  std::string code_;
  std::string name_;
  std::string industry_;
  double volume_;                                       //成交量
  double chengjiaoe_;                                   //成交额
  double week_volume_;
  double month_volume_;
  double change_percent_;                               //涨跌幅,收益率
  double week_change_percent_;
  double month_change_percent_;
  std::string area_;                                    //地区
  double pe_;                                           //市盈率
  double outstanding_;                                  //流通股本
  double totals_;                                       //总股本
  double totalAssets_;                                  //总资产(万)
  double liquidAssets_;                                 //流动资产
  double fixedAssets_;                                  //固定资产
  double reserved_;                                     //公积金
  double reservedPerShare_;                             //每股公积金
  double eps_;                                          //每股收益
  double bvps_;                                         //每股净资
  double pb_;                                           //市净率
  double market_value_;                                 //总市值
  double liquidMarketValue_;                            //流通市值
  double liquidScale_;                                  //流通比例
  std::string timeToMarket_;                            //上市日期
  double amplitude_;                                    //振幅
  double turnoverratio_;                                //换手率
  double current_trade_;                                //股价
  double current_open_;
  double current_high_;
  double current_low_;
  double current_settlement_;
  int current_trade_time_;
  int current_visit_time_;
  int current_visit_num_;
  double surged_limit_price_;
  double decline_limit_price_;
  std::string week_trade_day_;
  std::string month_trade_day_;
  bool is_surged_limit_;
  bool is_decline_limit_;
  double qfq_close_;
  double qfq_open_;
  double qfq_high_;
  double qfq_low_;
  std::map<int, YieldInfoUnit> yield_infos_;
  std::map<std::string, DataPerDay> data_per_day_;
  StockholderInfo holder_info_;
};

class DataPerHour {
public:

  DataPerHour();
  ~DataPerHour();

  int visit_per_hour_num_;
};

class DataPerDay {
public:

  DataPerDay();
  ~DataPerDay();

  void recount_visit_num(std::string& stock_date);

  std::string date;
  std::vector<DataPerHour> data_per_hour_;
  int visit_per_day_num_;
  double changepercent_;
  double close_;
};

class StockHistDataInfo {
 public:

  void add_hist_data(std::string date, double open, double high, double close,
                     double low, double qfq_price);

  void set_code(std::string code) {
    code_ = code;
  }

  std::string code() {
    return code_;
  }

  void ValueSerialization(base_logic::DictionaryValue* dict);

  void CountSupportPressurePos();

  void CountQFQYield();

  void set_volume_by_date(std::string date, double volume);

  void set_ma5_by_date(std::string date, double ma5);

  void set_ma10_by_date(std::string date, double ma10);

  void set_ma20_by_date(std::string date, double ma20);

  void set_v_ma5_by_date(std::string date, double v_ma5);

  void set_v_ma10_by_date(std::string date, double v_ma10);

  void set_v_ma20_by_date(std::string date, double v_ma20);

  void set_turnover_by_date(std::string date, double turnover);

  void set_yingxian_length_by_date(std::string date, double yingxian_length);

  void update_yingxian_length_by_date(std::string date);

  void set_shiti_length_by_date(std::string date, double shiti_length);

  void update_shiti_length_by_date(std::string date);

  void update_amplitude_by_date(std::string date);

  std::map<std::string, HistDataPerDay> stock_hist_data_;
  std::string code_;
};

class StockTotalInfo: public ExtAttribute {
 public:
  StockTotalInfo();
  ~StockTotalInfo();

  int score() {
    return score_;
  }

  void setScore(int score) {
    score_ = score;
  }

  void countScore();

  void ValueSerialization(base_logic::DictionaryValue* dict);

  void add_stock_industry(std::string industry_name) {
    basic_info_.set_industry(industry_name);
  }

  void update_kline_json();

  const std::string& getLineJson() const {
    return KLine_json_;
  }

  std::string getCharmJson(std::map<int, YieldInfoUnit>& hs300_yield_data,
                           std::string& name) {
    std::string yield_json = "";
    StockUtil::Instance()->get_json_of_yield_data(hs300_yield_data,
                                                  basic_info_.yield_infos_,
                                                  yield_json,
                                                  name);
    return yield_json;
  }

  void setLineJson(const std::string& lineJson) {
    KLine_json_ = lineJson;
  }

  const std::string& getKLineJsonp() const {
    return KLine_jsonp_;
  }

  void setKLineJsonp(std::string jsonp) {
    KLine_jsonp_ = jsonp;
  }

  void clear_stock_yield_info(int end_time) {
    std::map<int, YieldInfoUnit>::iterator iter =
        basic_info_.yield_infos_.begin();
    for (; iter != basic_info_.yield_infos_.end();) {
      if (end_time > iter->first)
        basic_info_.yield_infos_.erase(iter++);
      else
        break;
    }
    //basic_info_.yield_infos_.clear();
  }

  StockHistDataInfo hist_data_info_;
  StockBasicInfo basic_info_;
  double score_;
  std::string KLine_json_;
  std::string KLine_jsonp_;
};

class HotDiagramPerStock {
 public:
  HotDiagramPerStock() {
    code_ = "";
    name_ = "";
    volume_ = 0.0;
    week_volume_ = 0.0;
    month_volume_ = 0.0;
    change_percent_ = 0.0;
    week_change_percent_ = 0.0;
    month_change_percent_ = 0.0;
  }
  std::string code() const {
    return code_;
  }

  void set_code(std::string code) {
    code_ = code;
  }

  std::string name() const {
    return name_;
  }

  void set_name(std::string name) {
    name_ = name;
  }

  double volume() const {
    return volume_;
  }

  void set_volume(double volume) {
    volume_ = volume;
  }

  double week_volume() const {
    return week_volume_;
  }

  void set_week_volume(double week_volume) {
    week_volume_ = week_volume;
  }

  double month_volume() const {
    return month_volume_;
  }

  void set_month_volume(double month_volume) {
    month_volume_ = month_volume;
  }

  double change_percent() const {
    return change_percent_;
  }

  void set_change_percent(double change_percent) {
    change_percent_ = change_percent;
  }

  double week_change_percent() const {
    return week_change_percent_;
  }

  void set_week_change_percent(double week_change_percent) {
    week_change_percent_ = week_change_percent;
  }

  double month_change_percent() const {
    return month_change_percent_;
  }

  void set_month_change_percent(double month_change_percent) {
    month_change_percent_ = month_change_percent;
  }

  static bool cmp(const HotDiagramPerStock& t_stock,
                  const HotDiagramPerStock& d_stock) {
    return fabs(t_stock.change_percent()) > fabs(d_stock.change_percent());
  }

  static bool week_cmp(const HotDiagramPerStock& t_stock,
                       const HotDiagramPerStock& d_stock) {
    return fabs(t_stock.week_change_percent())
        > fabs(d_stock.week_change_percent());
  }

  static bool month_cmp(const HotDiagramPerStock& t_stock,
                        const HotDiagramPerStock& d_stock) {
    return fabs(t_stock.month_change_percent())
        > fabs(d_stock.month_change_percent());
  }

  std::string code_;
  std::string name_;
  double volume_;
  double week_volume_;
  double month_volume_;
  double change_percent_;
  double week_change_percent_;
  double month_change_percent_;
};

class BasicIndustryInfo :public ExtAttribute{
 public:

  BasicIndustryInfo() {
    industry_name_ = "";
    industry_market_value_ = 0.0;
    industry_changepoint_ = 0.0;
    industry_month_changepoint_ = 0.0;
    industry_week_changepoint_ = 0.0;
    industry_volume_ = 0.0;
    week_industry_volume_ = 0.0;
    month_industry_volume_ = 0.0;
    fall_stock_num_ = 0;
    rise_stock_num_ = 0;
    type_ = 0;
    //weight_analyzer_ = NULL;
    //stock_filter_ = NULL;
  }

  std::string industry_name() {
    return industry_name_;
  }

  void set_industry_name(std::string industry_name) {
    industry_name_ = industry_name;
  }

  double industry_volume() {
    return industry_volume_;
  }

  void set_industry_volume(double industry_volume) {
    industry_volume_ = industry_volume;
  }

  double week_industry_volume() {
    return week_industry_volume_;
  }

  void set_week_industry_volume(double week_industry_volume) {
    week_industry_volume_ = week_industry_volume;
  }

  double month_industry_volume() {
    return month_industry_volume_;
  }

  void set_month_industry_volume(double month_industry_volume) {
    month_industry_volume_ = month_industry_volume;
  }

  double industry_market_value() {
    return industry_market_value_;
  }

  void set_industry_market_value(double industry_market_value) {
    industry_market_value_ = industry_market_value;
  }

  void add_price_info(std::string stock_name, double price) {
    stock_price_info_[stock_name] = price;
  }

  double industry_changepoint() const {
    return industry_changepoint_;
  }

  void set_industry_changepoint(double industry_changepoint) {
    industry_changepoint_ = industry_changepoint;
  }

  double industry_week_changepoint() const {
    return industry_week_changepoint_;
  }

  void set_industry_week_changepoint(double industry_week_changepoint) {
    industry_week_changepoint_ = industry_week_changepoint;
  }

  double industry_month_changepoint() const {
    return industry_month_changepoint_;
  }

  void set_industry_month_changepoint(double industry_month_changepoint) {
    industry_month_changepoint_ = industry_month_changepoint;
  }

  int rise_stock_num() const {
    return rise_stock_num_;
  }

  void set_rise_stock_num(int rise_stock_num) {
    rise_stock_num_ = rise_stock_num;
  }

  int fall_stock_num() const {
    return fall_stock_num_;
  }

  void set_fall_stock_num(int fall_stock_num) {
    fall_stock_num_ = fall_stock_num;
  }

  int type() {
    return type_;
  }

  void set_type(int type) {
    type_ = type;
  }

  std::string ten_hot_diagram_stock_json() const {
    return ten_hot_diagram_stock_json_;
  }

  void set_ten_hot_diagram_stock_json(std::string ten_hot_diagram_stock_json) {
    ten_hot_diagram_stock_json_ = ten_hot_diagram_stock_json;
  }

  static bool cmp(const BasicIndustryInfo& t_info,
                  const BasicIndustryInfo& s_info) {
    return fabs(t_info.industry_changepoint())
        > fabs(s_info.industry_changepoint());
  }

  static bool week_cmp(const BasicIndustryInfo& t_info,
                       const BasicIndustryInfo& s_info) {
    return fabs(t_info.industry_week_changepoint())
        > fabs(s_info.industry_week_changepoint());
  }

  static bool month_cmp(const BasicIndustryInfo& t_info,
                        const BasicIndustryInfo& s_info) {
    return fabs(t_info.industry_month_changepoint())
        > fabs(s_info.industry_month_changepoint());
  }

  void update_hot_diagram_stock_json();

  void update_hot_diagram_stock_json_by_day() {
    hot_diagram_stocks_.sort(HotDiagramPerStock::cmp);
    ten_hot_diagram_stock_json_ = get_stocks_json("day");
  }

  void update_hot_diagram_stock_json_by_week() {
    hot_diagram_stocks_.sort(HotDiagramPerStock::week_cmp);
    week_ten_hot_diagram_stock_json_ = get_stocks_json("week");
  }

  void update_hot_diagram_stock_json_by_month() {
    hot_diagram_stocks_.sort(HotDiagramPerStock::month_cmp);
    month_ten_hot_diagram_stock_json_ = get_stocks_json("month");
  }

  std::string get_stocks_json(std::string type);

  std::map<std::string, double> get_stocks_in_industry() {
    return stock_price_info_;
  }

  std::string get_hot_diagram_by_type(std::string type) {
    if ("week" == type)
      return week_ten_hot_diagram_stock_json_;
    else if ("month" == type)
      return month_ten_hot_diagram_stock_json_;
    return ten_hot_diagram_stock_json_;
  }

  void add_industry_yield_info(int trade_time, double yield) {
    if (trade_time < 1467259129)
      return;
    YieldInfoUnit unit;
    unit.trade_time_ = trade_time;
    unit.yield_ = yield;
    industry_yield_infos_[trade_time] = unit;
  }

  void set_industry_visit_data(int trade_time, int visit_data) {
    /*LOG_MSG2("set_industry_visit_dataindustry_name=%s,trade_time=%d,visit_data=%d",
                   industry_name_.c_str(),
                   trade_time,
                   visit_data);*/
    std::map<int, YieldInfoUnit>::iterator iter = industry_yield_infos_.find(trade_time);
    if (iter != industry_yield_infos_.end()) {
      iter->second.visit_count_ = visit_data;
      /*LOG_MSG2("insert industry_name=%s,trade_time=%d,visit_data=%d",
               industry_name_.c_str(),
               trade_time,
               visit_data);*/
    }
  }

  void update_hist_data();

  void update_hist_data_by_weight();

  void add_yield_info_by_stock(std::string date, double stock_market_value,
                               HistDataPerDay& hist_data_unit);

  void add_volatility_info_by_stock(std::string date, double stock_market_value,
                                 HistDataPerDay& hist_data_unit);

  void clear_yield_info();

  bool get_chart_json(std::string& yield_json,
                      std::map<int, YieldInfoUnit>& hs300_yield_data,
                      std::string& name);

  bool get_hist_data_json(
      std::map<std::string, HistDataPerDay>& hs300_hist_data,
      std::string& start_date,
      std::string& end_date,
      std::string& json_str);

  void add_stock_weight_info(std::string& stock_code,
                             double weight);

  void add_stocks_weight(std::map<std::string, double>& stock_weight_info);

  void set_stocks_price_info(std::map<std::string, double>& stocks_info);

  double get_stock_weight_by_code(std::string& stock_code);

  void count_stock_visit_num();

  void update_industry_market_value();

  void update_current_yield_info();

  std::string industry_name_;
  double industry_volume_;
  double week_industry_volume_;
  double month_industry_volume_;
  double industry_changepoint_;
  double industry_week_changepoint_;
  double industry_month_changepoint_;
  double industry_market_value_;
  int type_;
  std::map<std::string, double> stock_price_info_;
  int rise_stock_num_;
  int fall_stock_num_;
  std::list<HotDiagramPerStock> hot_diagram_stocks_;
  std::string ten_hot_diagram_stock_json_;
  std::string week_ten_hot_diagram_stock_json_;
  std::string month_ten_hot_diagram_stock_json_;
  std::map<int, YieldInfoUnit> industry_yield_infos_;
  StockHistDataInfo industry_hist_data_info_;
  std::string weight_name_;
  std::string weight_;
  std::map<std::string, double> stock_weight_info_;
  //WeightAnalyzer* weight_analyzer_;
  //StockFilter* stock_filter_;
};

class HotDiagramPerIndustry {
 public:
  HotDiagramPerIndustry() {
    industry_name_ = "";
    industry_change_percent_ = 0.0;
    industry_week_change_percent_ = 0.0;
    industry_month_change_percent_ = 0.0;
  }
  std::string industry_name() const {
    return industry_name_;
  }

  void set_industry_name(std::string industry_name) {
    industry_name_ = industry_name;
  }

  double industry_change_percent() const {
    return industry_change_percent_;
  }

  void set_industry_change_percent(double industry_change_percent) {
    industry_change_percent_ = industry_change_percent;
  }

  double industry_week_change_percent() const {
    return industry_week_change_percent_;
  }

  void set_industry_week_change_percent(double industry_week_change_percent) {
    industry_week_change_percent_ = industry_week_change_percent;
  }

  double industry_month_change_percent() const {
    return industry_month_change_percent_;
  }

  void set_industry_month_change_percent(double industry_month_change_percent) {
    industry_month_change_percent_ = industry_month_change_percent;
  }

  static bool cmp(const HotDiagramPerIndustry& t_info,
                  const HotDiagramPerIndustry& s_info) {
    return fabs(t_info.industry_change_percent())
        > fabs(s_info.industry_change_percent());
  }

  static bool week_cmp(const HotDiagramPerIndustry& t_info,
                       const HotDiagramPerIndustry& s_info) {
    return fabs(t_info.industry_week_change_percent())
        > fabs(s_info.industry_week_change_percent());
  }

  static bool month_cmp(const HotDiagramPerIndustry& t_info,
                        const HotDiagramPerIndustry& s_info) {
    return fabs(t_info.industry_month_change_percent())
        > fabs(s_info.industry_month_change_percent());
  }

  std::string industry_name_;
  double industry_change_percent_;
  double industry_week_change_percent_;
  double industry_month_change_percent_;
};

class IndustryInfo {
 public:

  void ValueSerialization(base_logic::DictionaryValue* dict);

  void EventsValueSerialization(base_logic::DictionaryValue* dict);

  void CustomEventsValueSerialization(base_logic::DictionaryValue* dict);

  void update_hottest_industry() {
    hot_diagram_industry_.clear();
    std::map<std::string, BasicIndustryInfo>::iterator iter = industry_info_map_
        .begin();
    for (; iter != industry_info_map_.end(); iter++) {
      if (iter->second.type() == 0)
        hot_diagram_industry_.push_back(iter->second);
    }

    update_hottest_industry_by_day();
    update_hottest_industry_by_week();
    update_hottest_industry_by_month();
    LOG_MSG2("day_industry_hot_diagram_=%s", day_industry_hot_diagram_.c_str()); LOG_MSG2("week_industry_hot_diagram_=%s", week_industry_hot_diagram_.c_str()); LOG_MSG2("month_industry_hot_diagram_=%s", month_industry_hot_diagram_.c_str());
  }

  void update_hottest_industry_by_day() {
    hot_diagram_industry_.sort(BasicIndustryInfo::cmp);
    day_industry_hot_diagram_ = set_industry_json("day", hot_diagram_industry_);
  }

  void update_hottest_industry_by_week() {
    hot_diagram_industry_.sort(BasicIndustryInfo::week_cmp);
    week_industry_hot_diagram_ = set_industry_json("week",
                                                   hot_diagram_industry_);
  }

  void update_hottest_industry_by_month() {
    hot_diagram_industry_.sort(BasicIndustryInfo::month_cmp);
    month_industry_hot_diagram_ = set_industry_json("month",
                                                    hot_diagram_industry_);
  }

  void set_jsonp_by_type(std::string type, std::string jsonp_str) {
    industry_hot_diagram_jsonp_[type] = jsonp_str;
  }

  std::string get_jsonp_by_type(std::string type) {
    return industry_hot_diagram_jsonp_[type];
  }

  std::string set_industry_json(std::string type,
                                std::list<BasicIndustryInfo>& industry_list);

  void set_dapan_info(stock_logic::net_reply::VIPNewsList* vip_list);

  BasicIndustryInfo& get_industry_by_name(std::string industry_name) {
    return industry_info_map_[industry_name];
  }

  std::string get_industry_hot_diagram_by_type(std::string type,
                                               std::string format) {
    //if ("jsonp" == format)
    //	return industry_hot_diagram_jsonp_[type];
    if ("week" == type)
      return week_industry_hot_diagram_;
    else if ("month" == type)
      return month_industry_hot_diagram_;
    return day_industry_hot_diagram_;
  }

  std::string get_event_hot_diagram() {
    return event_hot_diagram_;
  }

  int get_count_by_industry_type(int type) {
    std::map<std::string, BasicIndustryInfo>::iterator iter = industry_info_map_
        .begin();
    int count = 0;
    for (; iter != industry_info_map_.end(); iter++) {
      if (iter->second.type() == type)
        count++;
    }
    return count;
  }

  bool check_event(std::string& event_name) {
    LOG_MSG2("event_name=%s", event_name.c_str());
    int event_count = get_count_by_industry_type(2);
    int industry_count = get_count_by_industry_type(0);
    int concept_count = get_count_by_industry_type(1);
    LOG_MSG2("event_count=%d,industry_count=%d,concept_count=%d",
        event_count,industry_count,concept_count);
    std::map<std::string, BasicIndustryInfo>::iterator iter = industry_info_map_
        .find(event_name);
    //if (industry_info_map_.end() != iter && iter->second.type() == 1)
    if (industry_info_map_.end() != iter) {
      LOG_MSG("found event!");
      std::string stock_lists = "";
      BasicIndustryInfo& basic_info = iter->second;
      std::map<std::string, double>::iterator stock_iter = basic_info
          .stock_price_info_.begin();
      for (; stock_iter != basic_info.stock_price_info_.end(); stock_iter++) {
        stock_lists += stock_iter->first;
        stock_lists += ",";
      } LOG_MSG2("stock lists=%s", stock_lists.c_str());
      return true;
    } else {
      LOG_MSG("not found event!");
      std::string industry_list = "";
      for (iter = industry_info_map_.begin(); iter != industry_info_map_.end();
          iter++) {
        industry_list += iter->first;
        industry_list += ",";
      } LOG_MSG2("industry_list=%s", industry_list.c_str());
      return false;
    }
  }

  void clear_industry_yield_info(int end_time) {
    std::map<std::string, BasicIndustryInfo>::iterator iter = industry_info_map_
        .begin();
    for (; iter != industry_info_map_.end(); iter++) {
      std::map<int, YieldInfoUnit>& yield_info = iter->second
          .industry_yield_infos_;
      std::map<int, YieldInfoUnit>::iterator yield_iter = yield_info.begin();
      for (; yield_iter != yield_info.end();) {
        if (end_time > yield_iter->first)
          yield_info.erase(yield_iter++);
        else
          break;
      }
    }
  }

  std::map<std::string, BasicIndustryInfo> industry_info_map_;
  std::string day_industry_hot_diagram_;
  std::string week_industry_hot_diagram_;
  std::string month_industry_hot_diagram_;
  std::string event_hot_diagram_;
  std::map<std::string, std::string> industry_hot_diagram_jsonp_;
  std::list<BasicIndustryInfo> hot_diagram_industry_;
};

class FuncTimeCount {
 public:
  FuncTimeCount(std::string func_name) {
    begin_time_ = (int) time(NULL);
    func_name_ = func_name;
    time_used_ = 0;
    func_id_ = 0;
    LOG_MSG2("func %s begin %d!", func_name.c_str(), begin_time_);
  }

  FuncTimeCount(int func_id) {
    func_name_ = "";
    func_id_ = func_id;
    time_used_ = 0;
    begin_time_ = (int) time(NULL);
    LOG_MSG2("func id %d begin %d!", func_id, begin_time_);
  }

  ~FuncTimeCount() {
    int current_time = (int) time(NULL);
    time_used_ = current_time - begin_time_;
    LOG_MSG2("func %s func_id=%d end,time_used_=%d", func_name_.c_str(), func_id_, time_used_);
  }

  std::string func_name_;
  int func_id_;
  int time_used_;
  int begin_time_;
};

}

#endif
