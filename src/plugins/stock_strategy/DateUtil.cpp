/*
 * DateUtil.cpp
 *
 *  Created on: 2016年8月31日
 *      Author: Maibenben
 */

#include "DateUtil.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

namespace stock_logic {

DateUtil* DateUtil::instance_ = NULL;

DateUtil::DateUtil() {
  // YGTODO Auto-generated constructor stub

}

DateUtil::~DateUtil() {
  // YGTODO Auto-generated destructor stub
}

DateUtil* DateUtil::instance() {
  if (instance_ == NULL)
    instance_ = new DateUtil();
  return instance_;
}

void DateUtil::FreeInstance() {
  if (NULL != instance_) {
    delete instance_;
    instance_ = NULL;
  }
}

int DateUtil::IsLeap(int m_Year)
{
  int t_leap=0;
  if(m_Year%4==0)
  {
    if(m_Year%100==0)
    {
      if(m_Year%400==0)
        t_leap= 1;
      else
        t_leap= 0;
    }else{
      t_leap= 1;
    }
  }
  return t_leap;
}

bool DateUtil::StringToDate(string date, int& year, int& month, int& day)
{
    year = atoi((date.substr(0,4)).c_str());
    month = atoi((date.substr(5,2)).c_str());
    day = atoi((date.substr(8,2)).c_str());
    int DAY[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    if(IsLeap(year)){
         DAY[1] = 29;
    }
    return year >= 0 && month<=12 && month>0 && day<=DAY[month] && day>0;
}

int DateUtil::DayInYear(int year, int month, int day)
{
    int DAY[12]={31,28,31,30,31,30,31,31,30,31,30,31};
    if(IsLeap(year))
        DAY[1] = 29;
    for(int i=0; i<month - 1; ++i)
    {
        day += DAY[i];
    }
    return day;
}

int DateUtil::DaysBetween2Date(string date1, string date2)
{
     int year1, month1, day1;
     int year2, month2, day2;
     if(!StringToDate(date1, year1, month1, day1) || !StringToDate(date2, year2,month2,day2))
        return -1;
     if(year1 == year2 && month1 == month2)
     {
          return day1 > day2 ? day1 - day2 : day2 - day1;

     }else if(year1 == year2)
     {
         int d1, d2;
         d1 = DayInYear(year1, month1, day1);
         d2 = DayInYear(year2, month2, day2);
         return d1 > d2 ? d1 - d2 : d2 - d1;

     }else{
         if(year1 > year2)
         {
             swap(year1, year2);
             swap(month1, month2);
             swap(day1, day2);
         }
         int d1,d2,d3;
         if(IsLeap(year1))
             d1 = 366 - DayInYear(year1,month1, day1);
         else
             d1 = 365 - DayInYear(year1,month1, day1);
         d2 = DayInYear(year2,month2,day2);

         d3 = 0;
         for(int year = year1 + 1; year < year2; year++)
         {
             if(IsLeap(year))
                 d3 += 366;
             else
                 d3 += 365;
         }
         return d1 + d2 + d3;
     }
}

bool DateUtil::timestampToDate(int timestamp, std::string& date) {
  time_t tmp_timestamp = (time_t)timestamp;
  struct tm current_tm = *localtime(&tmp_timestamp);
  char str_day[80];
  strftime(str_day, 64, "%Y-%m-%d", &current_tm);
  //sprintf(str_day, "%d-%02d-%02d", current_tm->tm_year + 1900,
  //        current_tm->tm_mon + 1, current_tm->tm_mday);
  date = str_day;
  return true;
}

bool DateUtil::int64timestampToDate(long long timestamp, std::string& date) {
  int tmp_timestamp = timestamp/1000;
  return timestampToDate(tmp_timestamp, date);
}

} /* namespace stock_logic */
