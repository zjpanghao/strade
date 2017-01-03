/*
 * HistEventsObserver.h
 *
 *  Created on: 2016年9月29日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_HISTEVENTSOBSERVER_H_
#define GEORGE_MASTER_PLUGINS_STOCK_HISTEVENTSOBSERVER_H_

#include "Observer.h"
#include "storage/data_engine.h"

/*
#include <string>
#include <vector>
#include <map>
#include <set>

using namespace std;
*/

namespace stock_logic {

class EventInfo {
 public:

  std::string event_name_;
  int start_time_;
  std::string start_time_str_;
  int end_time_;
  std::string end_time_str_;
  std::vector<std::string> related_stocks_;
};

class StocksPerDay {
 public:

  void add_stock(std::string& stock_code, std::string& event_name);

  std::multimap<std::string, std::string> stocks_info_;
};

class HistEventsObserver : public Observer {
 public:
  HistEventsObserver(stock_logic::Subject* subject);
  virtual ~HistEventsObserver();

  void Update(int opcode, stock_logic::Subject* subject);
  void add_stock_by_day(std::string& date,
                        std::string& stock_code,
                        std::string& event_name);

  bool fetch_hist_events_info();
  static void CallFectchEventsInfo(void* param, base_logic::Value* value);
  void ValueSerialization(base_logic::DictionaryValue* value);
  bool getEventStocksByDate(std::string date, std::vector<std::string>& stock_codes);
  bool getEventStocksByDate(std::string date, std::set<std::string>& stock_codes);

  std::map<std::string, StocksPerDay> event_stocks_;
  std::map<std::string, EventInfo> event_infos_;
  base_logic::DataEngine* mysql_engine_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_HISTEVENTSOBSERVER_H_ */
