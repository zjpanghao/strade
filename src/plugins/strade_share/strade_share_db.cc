//
// Created by Harvey on 2017/1/4.
//

#include "strade_share_db.h"

#include "basic/scoped_ptr.h"
#include "strade_share_engine.h"

namespace strade_share {

StradeShareDB::StradeShareDB(config::FileConfig* config,
                             SSEngineImpl* engine)
    : engine_impl_(engine) {
  assert(config->mysql_db_list_.size() >= 2);
  LOG_DEBUG2("db_list size=%d", config->mysql_db_list_.size());
  base::ConnAddr read_conn = config->mysql_db_list_.front();
  config->mysql_db_list_.pop_front();
  base::ConnAddr write_conn = config->mysql_db_list_.front();
  config->mysql_db_list_.pop_front();
  mysql_engine_ = new base_logic::MysqlThreadPool(read_conn, write_conn, 1);
}

StradeShareDB::~StradeShareDB() {
  if (NULL != mysql_engine_) {
    delete mysql_engine_;
    mysql_engine_ = NULL;
  }
}

bool StradeShareDB::FetchStockBasicInfo() {
  // sql 语句替代存储过程
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  base_logic::ListValue* column_name_list = new base_logic::ListValue();

  // 组装sql语句
  dict->SetString(MYSQL_QUERY_TYPE, "SELECT");
  AppendStringValue(column_name_list, "code");
  AppendStringValue(column_name_list, "name");
  AppendStringValue(column_name_list, "totalAssets");
  AppendStringValue(column_name_list, "bvps");
  AppendStringValue(column_name_list, "pb");

  dict->Set(MYSQL_QUERY_PARAM, column_name_list);

  dict->SetString(MYSQL_QUERY_CONDITION,
                  "FROM algo_get_stock_basics WHERE CODE IS NOT NULL;");

  // 添加任务
  mysql_engine_->QuerySync(base_logic::MYSQL_READ, dict,
                           OnFetchStockBasicInfoCallback, this);
  return true;
}

void StradeShareDB::OnFetchStockBasicInfoCallback(
    base_logic::DictionaryValue* dict, void* param) {
  StradeShareDB* pthis = static_cast<StradeShareDB*>(param);
  // 获取返回结果
  base_logic::ListValue* result_list = NULL;
  dict->GetList(MYSQL_RESULT_LIST, &result_list);
  if (NULL != result_list) {
    std::list<strade_logic::StockTotalInfo> list;
    size_t count = result_list->GetSize();
    while (count > 0) {
      base_logic::Value* result_value;
      result_list->Remove(--count, &result_value);
      if (NULL == result_value) {
        continue;
      }
      scoped_ptr<base_logic::DictionaryValue> dict_result_value(
          dynamic_cast<base_logic::DictionaryValue*>(result_value));
      std::string stock_code;
      dict_result_value->GetString(L"code", &stock_code);
      if (!stock_code.empty()) {
        strade_logic::StockTotalInfo stock_total_info;
        stock_total_info.DeserializeStockBasicInfo(*dict_result_value);
        list.push_back(stock_total_info);
      }
    }
    pthis->engine_impl_->OnLoadAllStockBasicInfo(list);
  }
}

bool StradeShareDB::FetchStockHistInfo(
    const std::string& stock_code) {
  if (stock_code.empty()) {
    LOG_ERROR("FetchStockHistInfo stock_code empty");
    return false;
  }
  LOG_DEBUG2("begin FetchStockHistInfo stock_code=%s", stock_code.c_str());
  // sql 语句替代存储过程
  base_logic::DictionaryValue* dict = new base_logic::DictionaryValue();
  base_logic::ListValue* column_name_list = new base_logic::ListValue();

  // 组装sql语句
  dict->SetString(MYSQL_QUERY_TYPE, "SELECT");
  AppendStringValue(column_name_list, "code");
  AppendStringValue(column_name_list, "date");
  AppendStringValue(column_name_list, "open");
  AppendStringValue(column_name_list, "high");
  AppendStringValue(column_name_list, "low");
  AppendStringValue(column_name_list, "qfq_close");
  dict->Set(MYSQL_QUERY_PARAM, column_name_list);

  std::stringstream ss;
  ss << " FROM algo_get_hist_data WHERE CODE = '";
  ss << stock_code << "'";
  ss << " ORDER BY DATE DESC LIMIT 60;";
  dict->SetString(MYSQL_QUERY_CONDITION, ss.str());

  mysql_engine_->QuerySync(base_logic::MYSQL_READ, dict,
                           OnFetchStockHistInfoCallback, this);
  return true;
}

void StradeShareDB::OnFetchStockHistInfoCallback(
    base_logic::DictionaryValue* dict, void* param) {
  StradeShareDB* pthis = static_cast<StradeShareDB*>(param);
  // 获取返回结果
  base_logic::ListValue* result_list = NULL;
  dict->GetList(MYSQL_RESULT_LIST, &result_list);
  if (NULL != result_list) {
    size_t count = result_list->GetSize();
    strade_logic::StockTotalInfo* stock_total_info =
        static_cast<strade_logic::StockTotalInfo*>(param);
    std::string stock_code;
    std::vector<strade_logic::StockHistInfo> stock_hist_vec;
    while (count > 0) {
      base_logic::Value* result_value;
      result_list->Remove(--count, &result_value);
      base_logic::DictionaryValue* dict_result_value =
          (base_logic::DictionaryValue*) (result_value);
      std::string date;
      dict_result_value->GetString(L"date", &date);
      dict_result_value->GetString(L"code", &stock_code);
      if (stock_code.empty() || date.empty()) {
        continue;
      }
      strade_logic::StockHistInfo stock_hist_info;
      stock_hist_info.Deserialize(*dict_result_value);
      stock_hist_vec.push_back(stock_hist_info);
    }
    pthis->engine_impl_->UpdateStockHistDataVec(stock_code, stock_hist_vec);
    LOG_DEBUG2("OnFetchStockHistInfoCallback stock_code=%s, hist_data size=%d",
               stock_code.c_str(), stock_hist_vec.size());
  }
}

} /* namespace strade_share */