//  Copyright (c) 2015-2015 The geprge Authors. All rights reserved.
//  Created on: 2016年5月18日 Author: kerry

#include <mysql.h>
#include "logic/logic_comm.h"
#include <sstream>
#include "stock_db.h"
#include "stock_factory.h"
#include "stock_user_mgr.h"

namespace stock_logic {

StockDB::StockDB(config::FileConfig* config) {
  mysql_engine_ = base_logic::DataEngine::Create(MYSQL_TYPE);
  mysql_engine_->InitParam(config->mysql_db_list_);
}

StockDB::~StockDB() {
  if (mysql_engine_) {
    delete mysql_engine_;
    mysql_engine_ = NULL;
  }
}

bool StockDB::FecthHexunDailyData(
      std::string min_time,
      std::map<std::string, stock_logic::StockTotalInfo>& map) {
  bool r = false;
    std::string sql = "call proc_GetHexunDailyData('";
    sql += min_time;
    sql += "');";
    scoped_ptr<base_logic::DictionaryValue> dict(
        new base_logic::DictionaryValue());
    base_logic::ListValue* listvalue;
    dict->SetString(L"sql", sql);
    r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                                CallFectchHexunDailyData);
    LOG_MSG2("r=%d", (int)r);
    if (!r)
      return false;
    dict->GetList(L"resultvalue", &listvalue);
    bool has_value = false;
    LOG_MSG2("listvalue->GetSize()=%d", listvalue->GetSize());
    int count = listvalue->GetSize();
    while (count > 0) {
      base_logic::Value* result_value;
      listvalue->Remove(--count, &result_value);
      base_logic::DictionaryValue* dict_result_value =
          (base_logic::DictionaryValue*) (result_value);
      std::string code = "";
      dict_result_value->GetString(L"v_stock", &code);
      stock_logic::StockTotalInfo& stock_info = map[code];
      stock_info.basic_info_.OfflineVisitSerialization(dict_result_value);
      delete dict_result_value;
      dict_result_value = NULL;
      has_value = true;
    }
    LOG_MSG("FecthOffLineVisitData finished");
    return has_value;
}

bool StockDB::FectchStockHoldersInfo(
        std::map<std::string, stock_logic::StockTotalInfo>& map) {
  bool r = false;
  std::string sql = "call proc_FectchStockHoldersInfo();";
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallFectchStockHoldersInfo);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  int count = listvalue->GetSize();
  StockFactory* factory = StockFactory::GetInstance();
  while (count > 0) {
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    std::string stock_code = "";
    dict_result_value->GetString(L"stock_code", &stock_code);
    if ("" != stock_code) {
      stock_logic::StockBasicInfo& basic_info = factory->GetBasicInfoByCode(stock_code);
      basic_info.holder_info_.HolderMapValueSerialization(dict_result_value);
      if ("000333" == stock_code) {
        std::string test_date = "";
        dict_result_value->GetString(L"date", &test_date);
        stock_logic::StockBasicInfo& test_basic_info = factory->GetBasicInfoByCode(stock_code);
        LOG_MSG2("basic_info=%p,test_basic_info=%p", &basic_info, &test_basic_info);
        test_basic_info.holder_info_.stockholder_map_[test_date].printSelf();
      }
    }
    delete dict_result_value;
    dict_result_value = NULL;
  }
  return true;
}

bool StockDB::FectchStockShareInfo(
      std::map<std::string, stock_logic::StockTotalInfo>& map) {
  bool r = false;
  std::string sql = "call proc_FectchStockShareInfo();";
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallFectchStockShareInfo);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  int count = listvalue->GetSize();
  LOG_MSG2("count=%d", count);
  StockFactory* factory = StockFactory::GetInstance();
  while (count > 0) {
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    std::string stock_code = "";
    dict_result_value->GetString(L"stock_code", &stock_code);
    //LOG_MSG2("stock_code=%s", stock_code.c_str());
    if ("" != stock_code) {
      stock_logic::StockBasicInfo& basic_info = factory->GetBasicInfoByCode(stock_code);
      basic_info.holder_info_.ValueSerialization(dict_result_value);
    }
    delete dict_result_value;
    dict_result_value = NULL;
  }
  return true;
}

bool StockDB::FectchStockBasicInfo(
    std::map<std::string, stock_logic::StockTotalInfo>& map) {
  bool r = false;
  std::string sql = "call proc_GetStockBasicInfo2();";
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallFectchStockInfo);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  int count = listvalue->GetSize();
  while (count > 0) {
    stock_logic::StockTotalInfo stock_info;
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    stock_info.basic_info_.ValueSerialization(dict_result_value);
    //list->push_back(task);
    std::string code = stock_info.basic_info_.code();
    if ("" != code)
      map[code] = stock_info;
    delete dict_result_value;
    dict_result_value = NULL;
  }
  return true;
}

bool StockDB::FectchStockHistData(
    std::string stock_code,
    std::map<std::string, stock_logic::StockTotalInfo>& map) {
  bool r = false;
  std::string sql = "call proc_GetStockHistData3('";
  sql += stock_code;
  sql += "');";
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  //LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallFectchStockHistData);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  bool test = true;
  int count = listvalue->GetSize();
  while (count > 0) {
    stock_logic::StockTotalInfo& stock_info = map[stock_code];
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    stock_info.hist_data_info_.ValueSerialization(dict_result_value);
    stock_info.hist_data_info_.set_code(stock_code);
    if (stock_code == "600692") {
      double turnoverratio = stock_info.hist_data_info_.stock_hist_data_["2016-09-12"].turnoverratio_;
      double obv = stock_info.hist_data_info_.stock_hist_data_["2016-09-12"].obv_;
      LOG_MSG2("600692,turnoverratio=%f,obv=%f", turnoverratio, obv);
    }
    //list->push_back(task);
    delete dict_result_value;
    dict_result_value = NULL;
  }
  return true;
}

bool StockDB::FectchStockDayKLineData(
    std::string max_time,
    std::map<std::string, stock_logic::StockTotalInfo>& map) {
  bool r = false;
  std::string sql = "call proc_GetStockDayKLineDataByTime('";
  sql += max_time;
  sql += "');";
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  //LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallFectchStockHistData);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  bool has_value = false;
  int count = listvalue->GetSize();
  while (count > 0) {
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    std::string code = "";
    dict_result_value->GetString(L"code", &code);
    stock_logic::StockTotalInfo& stock_info = map[code];
    stock_info.hist_data_info_.ValueSerialization(dict_result_value);
    delete dict_result_value;
    dict_result_value = NULL;
    has_value = true;
  }
  return has_value;
}

bool StockDB::FecthOffLineVisitData(
      std::string min_time,
      std::map<std::string, stock_logic::StockTotalInfo>& map) {
  bool r = false;
    std::string sql = "call proc_GetOfflineVisitData('";
    sql += min_time;
    sql += "');";
    scoped_ptr<base_logic::DictionaryValue> dict(
        new base_logic::DictionaryValue());
    base_logic::ListValue* listvalue;
    dict->SetString(L"sql", sql);
    r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                                CallFectchOfflineVisitData);
    LOG_MSG2("r=%d", (int)r);
    if (!r)
      return false;
    dict->GetList(L"resultvalue", &listvalue);
    bool has_value = false;
    LOG_MSG2("listvalue->GetSize()=%d", listvalue->GetSize());
    int count = listvalue->GetSize();
    while (count > 0) {
      base_logic::Value* result_value;
      listvalue->Remove(--count, &result_value);
      base_logic::DictionaryValue* dict_result_value =
          (base_logic::DictionaryValue*) (result_value);
      std::string code = "";
      dict_result_value->GetString(L"v_stock", &code);
      stock_logic::StockTotalInfo& stock_info = map[code];
      stock_info.basic_info_.OfflineVisitSerialization(dict_result_value);
      delete dict_result_value;
      dict_result_value = NULL;
      has_value = true;
    }
    LOG_MSG("FecthOffLineVisitData finished");
    return has_value;
}

bool StockDB::FectchStockVisitData(
    int min_time,
    int max_time,
    std::map<std::string, stock_logic::StockTotalInfo>& map) {
  LOG_MSG("FectchStockVisitData");
  bool r = false;
  std::string sql = "call proc_GetVisitData2(";
  char min_time_str[20];
  sprintf(min_time_str, "%d", min_time);
  char max_time_str[20];
  sprintf(max_time_str, "%d", max_time);
  sql += min_time_str;
  sql += ",";
  sql += max_time_str;
  sql += ");";
  scoped_ptr<base_logic::DictionaryValue> dict(
      new base_logic::DictionaryValue());
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) (dict.get()),
                              CallFectchStockVisitData);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  bool has_value = false;
  int old_trade_time = 0;
  int trade_time = 0;
  LOG_MSG2("size=%d", listvalue->GetSize());
  StockUserManager* stock_usr_mgr = StockFactory::GetInstance()->stock_usr_mgr_;
  int count = listvalue->GetSize();
  while (count > 0) {
    //stock_logic::StockTotalInfo& stock_info = map[tock_code];
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    std::string code = "";
    dict_result_value->GetString(L"stock_code", &code);
    stock_logic::StockTotalInfo& stock_info = map[code];
    stock_info.basic_info_.VisitDataSerialization(dict_result_value);
    dict_result_value->GetInteger(L"timeTamp", &trade_time);
    if (trade_time != old_trade_time) {
      if (old_trade_time != 0)
        stock_usr_mgr->UpdateIndustryVisitData(old_trade_time);
      old_trade_time = trade_time;
    }
    delete dict_result_value;
    dict_result_value = NULL;
    has_value = true;
  }
  if (0 != trade_time)
    stock_usr_mgr->UpdateIndustryVisitData(trade_time);
  return has_value;
}

bool StockDB::FectchIndustryInfo(stock_logic::IndustryInfo& map) {
  bool r = false;
  std::string sql = "call proc_GetIndustryInfo()";
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  //LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) dict,
                              CallFectchIndustryInfo);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  int count = listvalue->GetSize();
  while (count > 0) {
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    map.ValueSerialization(dict_result_value);
    delete dict_result_value;
    dict_result_value = NULL;
  }
  delete dict;
  dict = NULL;
  return true;
}

bool StockDB::FetchYieldData(
    std::map<std::string, stock_logic::StockTotalInfo>& stock_total_info) {
  bool r = false;
  std::string sql = "CALL proc_GetYieldData();";
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  r = mysql_engine_->ReadData(0, (base_logic::Value*) dict, CallFecthYieldData);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  int count = listvalue->GetSize();
  while (count > 0) {
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    std::string code;
    dict_result_value->GetString(L"code", &code);
    StockBasicInfo& stock_basic_info = stock_total_info[code].basic_info_;
    stock_basic_info.YieldValueSerialization(dict_result_value);

  }
}

bool StockDB::FectchEventsInfo(stock_logic::IndustryInfo& map) {
  bool r = false;
  std::string sql = "call proc_GetEventsInfo()";
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  //LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) dict,
                              CallFectchEventsInfo);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  int count = listvalue->GetSize();
  while (count > 0) {
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    map.EventsValueSerialization(dict_result_value);
    delete dict_result_value;
    dict_result_value = NULL;
  }
  delete dict;
  dict = NULL;
  return true;
}

bool StockDB::LoadCustomEvent(stock_logic::IndustryInfo& map) {
  bool r = false;
  std::string sql = "call proc_LoadCustomEventsInfo()";
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  //LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) dict,
                              CallLoadCustomEventsInfo);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  int count = listvalue->GetSize();
  while (count > 0) {
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    map.CustomEventsValueSerialization(dict_result_value);
    delete dict_result_value;
    dict_result_value = NULL;
  }
  delete dict;
  dict = NULL;
  return true;
}

bool StockDB::UpdateRealtimeStockInfo(
    std::map<std::string, stock_logic::StockTotalInfo>& stock_total_info) {
  bool r = false;
  LOG_MSG("UpdateRealtimeStockInfo");
  std::string sql = "call proc_GetRealtimeStockInfo();";
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) dict,
                              CallFectchRealtimeStockInfo);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  int count = listvalue->GetSize();
  while (count > 0) {
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    std::string code;
    dict_result_value->GetString(L"code", &code);
    StockBasicInfo& stock_basic_info = stock_total_info[code].basic_info_;
    stock_basic_info.RealtimeValueSerialization(dict_result_value);
    if (StockUtil::Instance()->is_trading_time())
      stock_basic_info.UpdateTodayKLine();
    delete dict_result_value;
    dict_result_value = NULL;
  }
  delete dict;
  dict = NULL;
  return true;
}

bool StockDB::UpdateALLRealtimeStockInfo(
    std::map<std::string, stock_logic::StockTotalInfo>& stock_total_info) {
  bool r = false;
  LOG_MSG("UpdateRealtimeStockInfo");
  std::string sql = "call proc_GetAllRealtimeStockInfo();";
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) dict,
                              CallFectchRealtimeStockInfo);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  int count = listvalue->GetSize();
  while (count > 0) {
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    std::string code;
    dict_result_value->GetString(L"code", &code);
    StockBasicInfo& stock_basic_info = stock_total_info[code].basic_info_;
    stock_basic_info.AllRealtimeValueSerialization(dict_result_value);
    delete dict_result_value;
    dict_result_value = NULL;
  }
  delete dict;
  dict = NULL;
  return true;
}

bool StockDB::UpdateWeekMonthData(
    std::map<std::string, stock_logic::StockTotalInfo>& stock_total_info) {
  bool r = false;
  LOG_MSG("UpdateWeekMonthData");
  std::string sql = "call proc_GetIndustryHistData();";
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) dict,
                              CallFectchWeekMonthData);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  int count = listvalue->GetSize();
  while (count > 0) {
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    std::string code;
    dict_result_value->GetString(L"code", &code);
    StockBasicInfo& stock_basic_info = stock_total_info[code].basic_info_;
    stock_basic_info.MonthWeekDataValueSerialization(dict_result_value);
    delete dict_result_value;
    dict_result_value = NULL;
  }
  delete dict;
  dict = NULL;
  return true;
}

void StockDB::CallFectchWeekMonthData(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"code", rows[0]);
      if (rows[1] != NULL)
        info_value->SetString(L"date", rows[1]);
      if (rows[2] != NULL)
        info_value->SetString(L"type", rows[2]);
      if (rows[3] != NULL)
        info_value->SetReal(L"p_change", (double) atof(rows[3]));
      if (rows[4] != NULL)
        info_value->SetReal(L"volume", (double) atof(rows[4]));
      std::string code = rows[0];
      if ("000001" == code) {
        std::string date = rows[1];
        std::string type = rows[2];
        double p_change = (double) atof(rows[3]);
        double volume = 0.0;
        if (rows[4] != NULL)
          volume = (double) atof(rows[4]);
      }
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void StockDB::CallFectchRealtimeStockInfo(void* param,
                                          base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"code", rows[0]);
      if (rows[1] != NULL)
        info_value->SetReal(L"changepercent", (double) atof(rows[1]));
      if (rows[2] != NULL)
        info_value->SetReal(L"trade", (double) atof(rows[2]));
      if (rows[3] != NULL)
        info_value->SetReal(L"open", (double) atof(rows[3]));
      if (rows[4] != NULL)
        info_value->SetReal(L"high", (double) atof(rows[4]));
      if (rows[5] != NULL)
        info_value->SetReal(L"low", (double) atof(rows[5]));
      if (rows[6] != NULL)
        info_value->SetReal(L"settlement", (double) atof(rows[6]));
      if (rows[7] != NULL)
        info_value->SetInteger(L"time", (int) atoi(rows[7]));
      if (rows[8] != NULL)
        info_value->SetReal(L"volume", (double) atof(rows[8]));
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

bool StockDB::GetLimitData(
    std::map<int, std::vector<int> >& market_limit_info) {
  bool r = false;
  std::string sql = "call proc_GetLimitData();";
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  base_logic::ListValue* listvalue;
  dict->SetString(L"sql", sql);
  LOG_MSG2("sql=%s",sql.c_str());
  r = mysql_engine_->ReadData(0, (base_logic::Value*) dict,
                              CallFectchGetLimitData);
  if (!r)
    return false;
  dict->GetList(L"resultvalue", &listvalue);
  int count = listvalue->GetSize();
  while (count > 0) {
    base_logic::Value* result_value;
    listvalue->Remove(--count, &result_value);
    base_logic::DictionaryValue* dict_result_value =
        (base_logic::DictionaryValue*) (result_value);
    int trade_time;
    dict_result_value->GetInteger(L"time", &trade_time);
    trade_time = (int) (trade_time / 60) * 60;
    if (market_limit_info.end() == market_limit_info.find(trade_time)) {
      std::vector<int> tmp_limit_info;
      tmp_limit_info.push_back(0);
      tmp_limit_info.push_back(0);
      market_limit_info[trade_time] = tmp_limit_info;
    }
    std::vector<int>& limit_info = market_limit_info[trade_time];
    dict_result_value->GetInteger(L"surged_stock_num", &limit_info[0]);
    dict_result_value->GetInteger(L"decline_stock_num", &limit_info[1]);
    delete dict_result_value;
    dict_result_value = NULL;
  }
  delete dict;
  dict = NULL;
  return true;
}

void StockDB::CallFectchGetLimitData(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetInteger(L"time", (int) atoi(rows[0]));
      if (rows[1] != NULL)
        info_value->SetInteger(L"surged_stock_num", (int) atoi(rows[1]));
      if (rows[2] != NULL)
        info_value->SetInteger(L"decline_stock_num", (int) atoi(rows[2]));
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

bool StockDB::WriteLimitData(int time, int surged_count, int decline_count) {
  bool r = false;
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  std::string sql;
  std::stringstream os;
  os << "call proc_SetLimitData(" << time << "," << surged_count << ","
     << decline_count << ");";
  sql = os.str();
  LOG_MSG2("sql=%s", sql.c_str());
  dict->SetString(L"sql", sql);
  r = mysql_engine_->WriteData(0, (base_logic::Value*) dict);
  delete dict;
  dict = NULL;
  if (!r)
    return false;
  return true;
}

bool StockDB::DeleteOldYieldInfo(int end_time) {
  bool r = false;
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  std::string sql;
  std::stringstream os;
  os << "call proc_DeleteOldYieldInfo(" << end_time << ");";
  sql = os.str();
  //LOG_MSG2("sql=%s", sql.c_str());
  dict->SetString(L"sql", sql);
  r = mysql_engine_->WriteData(0, (base_logic::Value*) dict);
  delete dict;
  dict = NULL;
  if (!r)
    return false;
  return true;
}

bool StockDB::UpdateYieldInfo(std::string code, int trade_time, double yield) {
  bool r = false;
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  std::string sql;
  std::stringstream os;
  os << "call proc_InsertYieldInfo('" << code << "'," << trade_time << ","
     << yield << ");";
  sql = os.str();
  //LOG_MSG2("sql=%s", sql.c_str());
  dict->SetString(L"sql", sql);
  r = mysql_engine_->WriteData(0, (base_logic::Value*) dict);
  delete dict;
  dict = NULL;
  if (!r)
    return false;
  return true;
}

bool StockDB::BatchUpdateYieldInfo(std::string code,
                                   std::map<int, YieldInfoUnit>& yield_info) {
  bool r = false;
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  std::string sql = "";
  std::stringstream os;
  os << "INSERT INTO algo_yield_info(code,time,yield) VALUES";
  std::map<int, YieldInfoUnit>::iterator iter = yield_info.begin();
  for (;;) {
    os << "('";
    os << code.c_str();
    os << "',";
    YieldInfoUnit& yield_info_unit = iter->second;
    os << iter->first;
    os << ',';
    os << yield_info_unit.yield_;
    os << ')';
    iter++;
    if (iter != yield_info.end())
      os << ',';
    else
      break;
  }
  os << ";";
  sql = os.str();
  //LOG_MSG2("sql=%s", sql.c_str());
  dict->SetString(L"sql", sql);
  r = mysql_engine_->WriteData(0, (base_logic::Value*) dict);
  delete dict;
  dict = NULL;
  if (!r)
    return false;
  return true;
}

void StockDB::CallFectchStockHistData(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"code", rows[0]);
      if (rows[1] != NULL)
        info_value->SetString(L"date", rows[1]);
      if (rows[2] != NULL)
        info_value->SetReal(L"open", (double) atof(rows[2]));
      if (rows[3] != NULL)
        info_value->SetReal(L"high", (double) atof(rows[3]));
      if (rows[4] != NULL)
        info_value->SetReal(L"close", (double) atof(rows[4]));
      if (rows[5] != NULL)
        info_value->SetReal(L"low", (double) atof(rows[5]));
      if (rows[6] != NULL)
        info_value->SetReal(L"qfq_close", (double) atof(rows[6]));
      if (rows[7] != NULL)
        info_value->SetReal(L"volume", (double) atof(rows[7]));
      if (rows[8] != NULL)
        info_value->SetReal(L"ma5", (double) atof(rows[8]));
      if (rows[9] != NULL)
        info_value->SetReal(L"ma10", (double) atof(rows[9]));
      if (rows[10] != NULL)
        info_value->SetReal(L"ma20", (double) atof(rows[10]));
      if (rows[11] != NULL)
        info_value->SetReal(L"v_ma5", (double) atof(rows[11]));
      if (rows[12] != NULL)
        info_value->SetReal(L"v_ma10", (double) atof(rows[12]));
      if (rows[13] != NULL)
        info_value->SetReal(L"v_ma20", (double) atof(rows[13]));
      if (rows[14] != NULL)
        info_value->SetReal(L"turnover", (double) atof(rows[14]));
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void StockDB::CallFectchStockVisitData(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"stock_code", rows[0]);
      if (rows[1] != NULL)
        info_value->SetInteger(L"timeTamp", (int)atoi((const char*)rows[1]));
      if (rows[2] != NULL)
        info_value->SetInteger(L"count", (int) atoi((const char*)rows[2]));
      std::string stock_code = (const char*)rows[0];
      int timeTamp = (int)atoi((const char*)rows[1]);
      int count = (int) atoi((const char*)rows[2]);
      if (stock_code == "601288") {
        LOG_MSG2("stock_code=%s,timeTamp=%d,count=%d",
                 stock_code.c_str(),
                 timeTamp,
                 count);
      }
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void StockDB::CallFectchHexunDailyData(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"date", rows[0]);
      if (rows[1] != NULL)
        info_value->SetInteger(L"heat", (int)atoi((const char*)rows[1]));
      if (rows[2] != NULL)
        info_value->SetInteger(L"time_stamp", (int) atoi((const char*)rows[2]));
      if (rows[3] != NULL)
        info_value->SetString(L"code", rows[3]);
      std::string stock_code = (const char*)rows[3];
      std::string date = (const char*)rows[0];
      int count = (int) atoi((const char*)rows[1]);
      int time_stamp = (int) atoi((const char*)rows[2]);
      if (stock_code == "601288") {
        LOG_MSG2("stock_code=%s,date=%s,time_stamp=%d,heat=%d",
                 stock_code.c_str(),
                 date.c_str(),
                 time_stamp,
                 count);
      }
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void StockDB::CallFectchOfflineVisitData(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"v_stock", rows[0]);
      if (rows[1] != NULL)
        info_value->SetString(L"v_hour", rows[1]);
      if (rows[2] != NULL)
        info_value->SetInteger(L"i_frequency", (int) atoi((const char*)rows[2]));
      std::string stock_code = (const char*)rows[0];
      std::string v_hour = (const char*)rows[1];
      int count = (int) atoi((const char*)rows[2]);
      if (stock_code == "601288") {
        LOG_MSG2("stock_code=%s,v_hour=%s,count=%d",
                 stock_code.c_str(),
                 v_hour.c_str(),
                 count);
      }
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void StockDB::CallFectchIndustryInfo(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"symbol", rows[0]);
      if (rows[1] != NULL)
        info_value->SetString(L"keyname", rows[1]);
      if (rows[2] != NULL)
        info_value->SetString(L"boardcode", rows[2]);
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void StockDB::CallFectchEventsInfo(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"name", rows[0]);
      if (rows[1] != NULL)
        info_value->SetString(L"stock", rows[1]);
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void StockDB::CallLoadCustomEventsInfo(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"event_name", rows[0]);
      if (rows[1] != NULL)
        info_value->SetString(L"stocks", rows[1]);
      if (rows[2] != NULL)
        info_value->SetString(L"weight_name", rows[2]);
      if (rows[3] != NULL)
        info_value->SetString(L"weight", rows[3]);
      if (rows[4] != NULL)
        info_value->SetString(L"description", rows[4]);
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void StockDB::CallFecthYieldData(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"code", rows[0]);
      if (rows[1] != NULL)
        info_value->SetInteger(L"time", (int) atoi(rows[1]));
      if (rows[2] != NULL)
        info_value->SetReal(L"yield", (double) atof(rows[2]));
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void StockDB::CallFectchStockHoldersInfo(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"stock_code", rows[0]);
      if (rows[1] != NULL)
        info_value->SetString(L"date", rows[1]);
      if (rows[2] != NULL)
        info_value->SetReal(L"holder_count", (double) atof(rows[2]));
      if (rows[3] != NULL)
        info_value->SetReal(L"compare_change", (double) atof(rows[3]));
      if (rows[4] != NULL)
        info_value->SetReal(L"float_stock_num", (double) atof(rows[4]));
      if (rows[5] != NULL)
        info_value->SetReal(L"top_float_stock_num", (double) atof(rows[5]));
      if (rows[6] != NULL)
        info_value->SetReal(L"top_float_stock_ratio", (double) atof(rows[6]));
      if (rows[7] != NULL)
        info_value->SetReal(L"top_stock_num", (double) atof(rows[7]));
      if (rows[8] != NULL)
        info_value->SetReal(L"top_stock_ratio", (double) atof(rows[8]));
      if (rows[9] != NULL)
        info_value->SetReal(L"institution_stock_num", (double) atof(rows[9]));
      if (rows[10] != NULL)
        info_value->SetReal(L"institution_stock_ratio", (double) atof(rows[10]));
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void StockDB::CallFectchStockShareInfo(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"stock_code", rows[0]);
      if (rows[1] != NULL)
        info_value->SetInteger(L"identifier", (int) atoi(rows[1]));
      if (rows[2] != NULL)
        info_value->SetReal(L"shares_price", (double) atof(rows[2]));
      if (rows[3] != NULL)
        info_value->SetReal(L"shares_number", (double) atof(rows[3]));
      if (rows[4] != NULL)
        info_value->SetReal(L"float_number", (double) atof(rows[4]));
      if (rows[5] != NULL)
        info_value->SetReal(L"total_number", (double) atof(rows[5]));
      if (rows[6] != NULL)
        info_value->SetReal(L"ratio", (double) atof(rows[6]));
      list->Append((base_logic::Value*) (info_value));
      LOG_MSG2("stock_code=%s", (char*)rows[0]);
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

void StockDB::CallFectchStockInfo(void* param, base_logic::Value* value) {
  base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*) (value);
  base_logic::ListValue* list = new base_logic::ListValue();
  base_storage::DBStorageEngine* engine =
      (base_storage::DBStorageEngine*) (param);
  MYSQL_ROW rows;
  int32 num = engine->RecordCount();
  if (num > 0) {
    while (rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc)) {
      base_logic::DictionaryValue* info_value =
          new base_logic::DictionaryValue();
      if (rows[0] != NULL)
        info_value->SetString(L"code", rows[0]);
      if (rows[1] != NULL)
        info_value->SetString(L"name", rows[1]);
      if (rows[2] != NULL)
        info_value->SetReal(L"pe", (double) atof(rows[2]));
      if (rows[3] != NULL)
        info_value->SetReal(L"outstanding", (double) atof(rows[3]));
      if (rows[4] != NULL)
        info_value->SetReal(L"totals", (double) atof(rows[4]));
      if (rows[5] != NULL)
        info_value->SetReal(L"totalAssets", (double) atof(rows[5]));
      if (rows[6] != NULL)
        info_value->SetReal(L"liquidAssets", (double) atof(rows[6]));
      if (rows[7] != NULL)
        info_value->SetReal(L"fixedAssets", (double) atof(rows[7]));
      if (rows[8] != NULL)
        info_value->SetReal(L"reserved", (double) atof(rows[8]));
      if (rows[9] != NULL)
        info_value->SetReal(L"reservedPerShare", (double) atof(rows[9]));
      if (rows[10] != NULL)
        info_value->SetReal(L"eps", (double) atof(rows[10]));
      if (rows[11] != NULL)
        info_value->SetReal(L"bvps", (double) atof(rows[11]));
      if (rows[12] != NULL)
        info_value->SetReal(L"pb", (double) atof(rows[12]));
      if (rows[13] != NULL)
        info_value->SetReal(L"mtm", (double) atof(rows[13]));
      if (rows[14] != NULL)
        info_value->SetString(L"industry", rows[14]);
      if (rows[15] != NULL)
        info_value->SetString(L"area", rows[15]);
      if (rows[16] != NULL)
        info_value->SetString(L"timeToMarket", rows[16]);
      list->Append((base_logic::Value*) (info_value));
    }
  }
  dict->Set(L"resultvalue", (base_logic::Value*) (list));
}

}
