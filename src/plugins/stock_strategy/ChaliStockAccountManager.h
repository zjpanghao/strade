/*
 * ChaliStockAccountManager.h
 *
 *  Created on: 2016年8月30日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_CHALISTOCKACCOUNTMANAGER_H_
#define GEORGE_MASTER_PLUGINS_STOCK_CHALISTOCKACCOUNTMANAGER_H_

#include "StockAccountManager.h"

namespace stock_logic {

class ChaliStockAccountManager : public StockAccountManager {
 public:
  ChaliStockAccountManager();
  virtual ~ChaliStockAccountManager();



  void Update(int opcode, stock_logic::Subject* subject);
  void init();
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_CHALISTOCKACCOUNTMANAGER_H_ */
