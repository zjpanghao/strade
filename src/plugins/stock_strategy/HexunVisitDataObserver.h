/*
 * HexunVisitDataObserver.h
 *
 *  Created on: 2016年9月12日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_HEXUNVISITDATAOBSERVER_H_
#define GEORGE_MASTER_PLUGINS_STOCK_HEXUNVISITDATAOBSERVER_H_

#include "Observer.h"

namespace stock_logic {

class HexunVisitDataObserver : public Observer {
 public:
  HexunVisitDataObserver(stock_logic::Subject* subject);
  virtual ~HexunVisitDataObserver();

  void Update(int opcode, stock_logic::Subject* subject);

  std::string attr_name_;
  std::string max_daily_date_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_HEXUNVISITDATAOBSERVER_H_ */
