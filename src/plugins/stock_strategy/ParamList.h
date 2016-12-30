/*
 * ParamList.h
 *
 *  Created on: 2016年7月29日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_PARAMLIST_H_
#define GEORGE_MASTER_PLUGINS_STOCK_PARAMLIST_H_

#include <map>
#include <string>

using namespace std;

namespace stock_logic {

class ParamList {
 public:
  ParamList();
  virtual ~ParamList();

  bool add_str_param(string& key, string& value);

  bool get_str_param(string& key, string& value);

  std::map<string, string> str_params;
  std::map<string, int> int_params;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_PARAMLIST_H_ */
