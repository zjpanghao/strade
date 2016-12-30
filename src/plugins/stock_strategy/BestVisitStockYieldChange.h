/*
 * BestVisitStockYieldChange.h
 *
 *  Created on: 2016年8月16日
 *      Author: harvey
 */

/*
 * 上海电信数据1月份-6月份当天热度最好的10支股票，在前10天和后10天的收益率变化
 * */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_BESTVISITSTOCKYIELDCHANGE_H_
#define GEORGE_MASTER_PLUGINS_STOCK_BESTVISITSTOCKYIELDCHANGE_H_

#include "Observer.h"
#include <set>

namespace stock_logic {

namespace {

class StockInfoSortByVisit {
 public:
    StockInfoSortByVisit()
            : before_yield_num_(0),
              after_yield_num_(0),
              stock_code_(""),
              visit_num_(0),
              change_percent_(0),
              close_(0),
              stock_name_("") {
    }

    std::string stock_code_;
    std::string stock_name_;
    double change_percent_;
    int visit_num_;
    double close_;
    double before_yield_num_;
    double after_yield_num_;
};

class StockVisitCmp {
 public:
    bool operator()(const StockInfoSortByVisit* lhs, const StockInfoSortByVisit* rhs) const {
        return lhs->visit_num_ > rhs->visit_num_;
    }
};

class StockVisitMgr {
 public:
    typedef std::set<StockInfoSortByVisit*, StockVisitCmp> STOCK_SET;
    typedef std::map<std::string, StockInfoSortByVisit*> STOCK_MAP;

    //创建或添加
    StockInfoSortByVisit* CreateOrGetVisitInfo(const std::string& stock_code) {
        STOCK_MAP::iterator iter(stock_map_.find(stock_code));
        if (iter == stock_map_.end()) {
            StockInfoSortByVisit* temp = new StockInfoSortByVisit();
            stock_map_[stock_code] = temp;
            return temp;
        }
        return iter->second;
    }

    bool CheckStockIsUpdate(const std::string &stock_code) {
        STOCK_MAP::iterator iter(stock_map_.find(stock_code));
        if (iter == stock_map_.end()) {
            return false;
        }
        return true;
    }

 public:
    STOCK_SET stock_set_;
    STOCK_MAP stock_map_;
};

} /*namespace*/

class BestVisitStockYieldChange : public Observer {
 public:
    typedef std::map<std::string, StockVisitMgr> BEST_VISIT_YIELD_MAP;
    BestVisitStockYieldChange(stock_logic::Subject* subject);
    virtual ~BestVisitStockYieldChange();

 public:
    void UpdateStockHistData(stock_logic::Subject* subject);
    virtual void Update(int opcode, stock_logic::Subject* subject);
    virtual base_logic::DictionaryValue* Request(
            base_logic::DictionaryValue* params);

 private:
    bool CheckUpdateDateExists(const std::string &date,
                               const std::string& stock_code) {
        BEST_VISIT_YIELD_MAP::iterator iter(stock_visit_map_.find(date));
        if (iter == stock_visit_map_.end()) {
            return false;
        }
        return iter->second.CheckStockIsUpdate(stock_code);
    }
    void HandleBestVisitStockYieldInfo(const std::string& stock_code,
                                       const std::string &date,
                                       StockInfoSortByVisit* one_stock, int range_num);

 private:
    BEST_VISIT_YIELD_MAP stock_visit_map_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_BESTVISITSTOCKYIELDCHANGE_H_ */
