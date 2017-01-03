/*
 * DateUtil.h
 *
 *  Created on: 2016年8月31日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_DATEUTIL_H_
#define GEORGE_MASTER_PLUGINS_STOCK_DATEUTIL_H_

#include <string>

using namespace std;

namespace stock_logic {

class DateUtil {
 public:
  DateUtil();
  virtual ~DateUtil();

  static DateUtil* instance();
  static void FreeInstance();

  int IsLeap(int m_Year);
  bool StringToDate(string date, int& year, int& month, int& day);
  int DayInYear(int year, int month, int day);
  int DaysBetween2Date(string date1, string date2);
  bool timestampToDate(int timestamp, std::string& date);
  bool int64timestampToDate(long long timestamp, std::string& date);

  static DateUtil* instance_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_DATEUTIL_H_ */
