//
// Created by Harvey on 2017/1/4.
//

#ifndef STRADE_STRADE_SHARE_DB_H
#define STRADE_STRADE_SHARE_DB_H

#include "logic/strade_basic_info.h"
#include "storage/mysql_thread_pool.h"

namespace strade_share {

class SSEngineImpl;
class StradeShareDB {
 public:
  StradeShareDB(config::FileConfig* config,
                SSEngineImpl* engine);
  ~StradeShareDB();

  base_logic::MysqlThreadPool& MysqlEngine() {
    return *mysql_engine_;
  }

 public:
  // 载入股票基本数据
  bool FetchStockBasicInfo();

  // 载入股票历史数据
  bool FetchStockHistInfo(const std::string& stock_code);

 private:
  static void OnFetchStockBasicInfoCallback(
      base_logic::DictionaryValue* dict, void* param);
  static void OnFetchStockHistInfoCallback(
      base_logic::DictionaryValue* dict, void* param);

  void AppendStringValue(base_logic::ListValue* list,
                         const std::string& value) {
    list->Append(list->CreateStringValue(value));
  }

 public:
  SSEngineImpl* engine_impl_;

 private:
  base_logic::MysqlThreadPool* mysql_engine_;
};

} /* namespace strade_share */

#endif //STRADE_STRADE_SHARE_DB_H
