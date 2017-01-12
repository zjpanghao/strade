//
// Created by Harvey on 2017/1/4.
//

#include "strade_share_db.h"

#include "basic/scoped_ptr.h"
#include "strade_share_engine.h"

using namespace strade_logic;

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
  mysql_engine_ = new base_logic::MysqlEngine(read_conn, write_conn);
}

StradeShareDB::~StradeShareDB() {
  if (NULL != mysql_engine_) {
    delete mysql_engine_;
    mysql_engine_ = NULL;
  }
}

bool StradeShareDB::FetchAllStockList(std::vector<StockTotalInfo>& stock_vec) {
  static const std::string& SQL =
      "SELECT `code`, `name`, `outstanding`, `bvps`, `pb` FROM `stock_basic_data`;";
  return mysql_engine_->QuerySync<StockTotalInfo>(base_logic::MYSQL_READ, SQL, stock_vec);
}

bool StradeShareDB::FetchStockHistList(const std::string& stock_code,
                                       std::vector<StockHistInfo>& stock_hist_vec) {
  static const std::string SQL =
      "SELECT date, open, high, close, low, qfq_close FROM stock_hist_data WHERE CODE = '"
          + stock_code + "' ORDER BY DATE DESC LIMIT 60;";
  return mysql_engine_->QuerySync<StockHistInfo>(base_logic::MYSQL_READ, SQL, stock_hist_vec);

}

} /* namespace strade_share */