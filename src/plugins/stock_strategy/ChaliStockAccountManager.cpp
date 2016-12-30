/*
 * ChaliStockAccountManager.cpp
 *
 *  Created on: 2016年8月30日
 *      Author: Maibenben
 */

#include "ChaliStockAccountManager.h"

namespace stock_logic {

ChaliStockAccountManager::ChaliStockAccountManager() {
  // YGTODO Auto-generated constructor stub

}

ChaliStockAccountManager::~ChaliStockAccountManager() {
  // YGTODO Auto-generated destructor stub
}

void ChaliStockAccountManager::Update(int opcode, stock_logic::Subject* subject) {
  LOG_MSG("ChaliStockAccountManager Update");
}

void ChaliStockAccountManager::init() {
  LOG_MSG("ChaliStockAccountManager init");
  factory_ = StockFactory::GetInstance();
  std::string start_date = "2016-06-01";
  std::map<std::string, HistDataPerDay>& hs300_hist_data =
      factory_->GetHistDataByCode(HSSANBAI);
  std::map<std::string, HistDataPerDay>::iterator iter =
      hs300_hist_data.begin();
  for (; iter != hs300_hist_data.end(); iter++) {

  }
}

} /* namespace stock_logic */
