/*
 * ExtAttribute.cpp
 *
 *  Created on: 2016年8月30日
 *      Author: Maibenben
 */

#include "ExtAttribute.h"
#include "logic/logic_comm.h"

namespace stock_logic {

ExtAttribute::ExtAttribute() {
}

ExtAttribute::~ExtAttribute() {
}

bool ExtAttribute::addAttr(std::string& attr_name, void* attr_value) {
  ATTRS_MAP::iterator iter = attrs_.find(attr_name);
  if (iter != attrs_.end())
    return false;
  else {
    attrs_[attr_name] = attr_value;
    return true;
  }
}

void ExtAttribute::delAttr(std::string& attr_name) {
  ATTRS_MAP::iterator iter = attrs_.find(attr_name);
  if (iter != attrs_.end())
    attrs_.erase(iter);
}

void* ExtAttribute::getAttr(std::string& attr_name) {
  ATTRS_MAP::iterator iter = attrs_.find(attr_name);
  if (iter != attrs_.end())
    return iter->second;
  else
    return NULL;
}

void* ExtAttribute::updateAttr(std::string& attr_name, void* attr_value) {
  if (attr_name.length() == 0 || NULL == attr_value) {
    LOG_MSG("invalid attrs");
    return NULL;
  }
  ATTRS_MAP::iterator iter = attrs_.find(attr_name);
  if (iter != attrs_.end()) {
    void* re = iter->second;
    iter->second = attr_value;
    return re;
  }
  else {
    attrs_[attr_name] = attr_value;
    return NULL;
  }
}

} /* namespace stock_logic */
