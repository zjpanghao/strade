/*
 * StockHighMinuxClose.h
 *
 *  Created on: 2016年9月14日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_STOCKHIGHMINUXCLOSE_H_
#define GEORGE_MASTER_PLUGINS_STOCK_STOCKHIGHMINUXCLOSE_H_

#include "Observer.h"

namespace stock_logic {

class StockHighMinuxClose : public Observer {
 public:
  StockHighMinuxClose();
  virtual ~StockHighMinuxClose();

  void Update(int opcode, stock_logic::Subject* subject);
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_STOCKHIGHMINUXCLOSE_H_ */
