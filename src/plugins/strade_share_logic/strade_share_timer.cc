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
  return true;
}

bool StradeShareTimer::OnTimeLoadStockVisit() {
  static const std::string LOAD_STOCK_VISIT_SQL =
      "SELECT stock_code, SUM(COUNT) FROM `stock_visit` GROUP BY stock_code";
  ss_engine_->AddMysqlAsyncJob(2, LOAD_STOCK_VISIT_SQL, OnTimeLoadStockVisitCallback, MYSQL_READ);
  return true;
}

void StradeShareTimer::OnTimeLoadStockVisitCallback(
    int column_num, MYSQL_ROWS_VEC& rows_vec) {
  bool r = false;
  StockTotalInfo stock_total_info;
  MYSQL_ROWS_VEC::iterator row_iter(rows_vec.begin());
  for (; row_iter != rows_vec.end(); ++row_iter) {
    std::vector<std::string> column_vec = (*row_iter);
    assert(column_vec.size() == column_num);

    std::string stock_code = column_vec[0];
    if(stock_code.empty()) {
      continue;
    }
    int visit_count = atoi(column_vec[1].c_str());
    r = ss_engine_->GetStockTotalInfoByCode(
        stock_code, stock_total_info);
    if (!r) {
      LOG_ERROR2("stock_code=%s, not find stock_total_info",
                 stock_code.c_str());
      continue;
    }
    stock_total_info.set_visit_num(visit_count);

  }
  LOG_DEBUG2("load stock visit num finshed! size=%d", rows_vec.size());
}

} /* namespace strade_share_logic */