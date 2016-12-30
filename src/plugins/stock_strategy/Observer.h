/*
 * observer.h
 *
 *  Created on: 2016年8月1日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_OBSERVER_H_
#define GEORGE_MASTER_PLUGINS_STOCK_OBSERVER_H_

#include "Subject.h"
#include "logic/base_values.h"
#include "logic/logic_comm.h"
#include "stock_basic_info.h"
#include "stock_factory.h"

namespace stock_logic {

class StockFactory;

class Observer {
 public:
  Observer(stock_logic::Subject* subject);
  Observer();
  virtual ~Observer();

  std::string name();
  void set_name(std::string name);

  virtual void init();

  virtual void Update(int opcode, stock_logic::Subject* subject)=0;

  void Process(int socket, base_logic::DictionaryValue* params);

  virtual std::string Analysis(base_logic::DictionaryValue* params);

  virtual base_logic::DictionaryValue* Request(base_logic::DictionaryValue* params);

  void RegisterEvent(int opcode);

  std::string name_;
  stock_logic::Subject* subject_;
  StockFactory* factory_;
  StockUserCache* cache_;
  STOCKINFO_MAP* stock_total_map_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_OBSERVER_H_ */
