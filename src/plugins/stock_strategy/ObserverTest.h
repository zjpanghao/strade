/*
 * ObserverTest.h
 *
 *  Created on: 2016年8月4日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_OBSERVERTEST_H_
#define GEORGE_MASTER_PLUGINS_STOCK_OBSERVERTEST_H_

#include "Observer.h"

namespace stock_logic {

class ObserverTest : public Observer {
 public:
  ObserverTest(stock_logic::Subject* subject);
  virtual ~ObserverTest();

  void Update(int opcode, stock_logic::Subject*);
  base_logic::DictionaryValue* Request(base_logic::DictionaryValue* params);
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_OBSERVERTEST_H_ */
