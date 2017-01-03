/*
 * IndustryStockHotRankObserver.h
 *
 *  Created on: 2016年9月4日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_INDUSTRYSTOCKHOTRANK_H_
#define GEORGE_MASTER_PLUGINS_STOCK_INDUSTRYSTOCKHOTRANK_H_

#include "Observer.h"

namespace stock_logic {

#include <string>
#include <list>
#include <algorithm>
#include <map>
#include <vector>

using namespace std;

class IndustryStockRankUnit {
 public:

  IndustryStockRankUnit() {
    stock_code_ = "";
    visit_num_ = 0;
    thirty_standard_deviation_ = 0;
  }

  static inline bool cmp(const IndustryStockRankUnit& t_rank_unit,
                         const IndustryStockRankUnit& s_rand_unit) {
    return t_rank_unit.visit_num_ > s_rand_unit.visit_num_;
  }

  std::string stock_code_;
  int visit_num_;
  double thirty_standard_deviation_;
  std::string date_;
};

class IndustryStockRank {
 public:

  IndustryStockRank() {

  }

  void addList(std::string, std::list<IndustryStockRankUnit>& rank_list);

  std::map< std::string, std::list<IndustryStockRankUnit> > rank_map_;
};

class IndustryStockHotRankObserver : public Observer {
 public:
  IndustryStockHotRankObserver();
  virtual ~IndustryStockHotRankObserver();

  void Update(int opcode, stock_logic::Subject*);
  void updateIndustryHotRank();
  void UpdateHotRandPerIndustry(BasicIndustryInfo& info);
  bool getTwentyHotStocksOfIndustry(
      std::string& date,
      std::string& industry_name,
      std::list<IndustryStockRankUnit>& stocks);
  bool countVisitByDate(std::map<std::string, DataPerDay>& visit_datas,
                        std::string& date,
                        int& count);
  bool countThirtyStandardDeviation(IndustryStockRankUnit& unit);


  std::string attr_name_;

};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_INDUSTRYSTOCKHOTRANK_H_ */
