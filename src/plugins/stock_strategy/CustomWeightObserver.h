/*
 * CustomWeightObserver.h
 *
 *  Created on: 2016年8月10日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_CUSTOMWEIGHTOBSERVER_H_
#define GEORGE_MASTER_PLUGINS_STOCK_CUSTOMWEIGHTOBSERVER_H_

#include "Observer.h"

namespace stock_logic {

class CustomWeightObserver : public Observer {
 public:
  CustomWeightObserver(stock_logic::Subject* subject);
  virtual ~CustomWeightObserver();

  base_logic::DictionaryValue* Request(base_logic::DictionaryValue* params);
  void Update(int opcode, stock_logic::Subject* subject);

  std::map<std::string, double> GetStocksInfo(base_logic::DictionaryValue* params);
  base_logic::DictionaryValue* GetCustomStocksField(
      std::map<std::string, double>& stocks_info_map,
      std::string& start_date,
      std::string& end_date);
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_CUSTOMWEIGHTOBSERVER_H_ */
