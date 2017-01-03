/*
 * ExtAttribute.h
 *
 *  Created on: 2016年8月30日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_EXTATTRIBUTE_H_
#define GEORGE_MASTER_PLUGINS_STOCK_EXTATTRIBUTE_H_

#include <string>
#include <map>

using namespace std;

typedef std::map<std::string, void*> ATTRS_MAP;

namespace stock_logic {

class ExtAttribute {
 public:
  ExtAttribute();
  virtual ~ExtAttribute();

  bool addAttr(std::string& attr_name, void* attr_value);

  void delAttr(std::string& attr_name);

  void* getAttr(std::string& attr_name);

  void* updateAttr(std::string& attr_name, void* attr_value);

  std::map<std::string, void*> attrs_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_EXTATTRIBUTE_H_ */
