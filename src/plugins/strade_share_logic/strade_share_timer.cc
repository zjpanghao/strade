//
// Created by Harvey on 2017/1/19.
//

#include "strade_share_timer.h"

using namespace base_logic;
using namespace strade_share;
using namespace strade_logic;

namespace strade_share_logic {

strade_share::SSEngine* StradeShareTimer::ss_engine_ = NULL;
StradeShareTimer::StradeShareTimer() {
  if (!InitParam()) {
    LOG_ERROR("StradeShareTimer init error");
    assert(0);
  }
}

StradeShareTimer* StradeShareTimer::GetInstance() {
  static StradeShareTimer instance;
  return &instance;
}

bool StradeShareTimer::InitParam() {
  strade_share::SSEngine* (* ss_engine)(void);
  std::string strade_share_library = "./strade_share/strade_share.so";
  std::string strade_share_func = "GetStradeShareEngine";
  ss_engine = (strade_share::SSEngine* (*)(void))
      base_logic::SomeUtils::GetLibraryFunction(
          strade_share_library, strade_share_func);
  ss_engine_ = (*ss_engine)();
  if (!ss_engine_) {
    return false;
  }
  ss_engine_->Init();
  mysql_engine_ = ss_engine_->GetMysqlEngine();
  if (!mysql_engine_) {
    return false;
  }
  return true;
}

bool StradeShareTimer::OnTimeLoadStockVisit() {
  static const std::string LOAD_STOCK_VISIT_SQL =
      "SELECT stock_code, SUM(COUNT) FROM `stock_visit` GROUP BY stock_code";
  mysql_engine_->AddAsyncMysqlJob(
      LOAD_STOCK_VISIT_SQL, OnTimeLoadStockVisitCallback, MYSQL_READ);
  return true;
}

void StradeShareTimer::OnTimeLoadStockVisitCallback(
    std::vector<MYSQL_ROW>& rows_vec) {
  bool r = false;
  StockTotalInfo stock_total_info;
  std::vector<MYSQL_ROW>::iterator iter(rows_vec.begin());
  for (; iter != rows_vec.end(); ++iter) {
    MYSQL_ROW& rows = (*iter);
    std::string stock_code("");
    if (!rows[0]) {
      stock_code = rows[0];
    }
    int32 count = 0;
    if (!stock_code.empty() && rows[1]) {
      count = atoi(rows[1]);
      if (count <= 0) {
        continue;
      }
      r = ss_engine_->GetStockTotalInfoByCode(
          stock_code, stock_total_info);
      if (!r) {
        continue;
      }
      stock_total_info.set_visit_num(count);
    }
  }
  LOG_DEBUG2("load stock visit num finshed! size=%d",
            rows_vec.size());
}

} /* namespace strade_share_logic */