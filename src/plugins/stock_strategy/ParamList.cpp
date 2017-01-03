/*
 * ParamList.cpp
 *
 *  Created on: 2016年7月29日
 *      Author: Maibenben
 */

#include "ParamList.h"

namespace stock_logic {

ParamList::ParamList() {
  // YGTODO Auto-generated constructor stub

}

ParamList::~ParamList() {
  // YGTODO Auto-generated destructor stub
}

bool ParamList::add_str_param(string& key, string& value) {
  std::map<string, string>::iterator iter = str_params.find(key);
  if (iter != str_params.end()) {
    return false;
  }
  else {
    str_params[key] = value;
    return true;
  }
}

bool ParamList::get_str_param(string& key, string& value) {
  std::map<string, string>::iterator iter = str_params.find(key);
  if (iter != str_params.end()) {
    value = iter->second;
    return true;
  }
  else
    return false;
}

} /* namespace stock_logic */
