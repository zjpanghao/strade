/*
 * inc_ten_stock.h
 *
 *  Created on: 2016年8月10日
 *      Author: harvey
 */

/*
 * 上海电信数据1月份-6月份当天收益最好的10支股票，在前10天和后10天的热度变化
 * */

#ifndef KID_PLUGINS_STOCK_BestYieldStockVisitChange_H_
#define KID_PLUGINS_STOCK_BestYieldStockVisitChange_H_

#include "Observer.h"
#include <set>

using base_logic::DictionaryValue;

namespace stock_logic {

namespace {

class StockSortByYield {
 public:
    StockSortByYield()
            : before_ten_visit_num_(0),
              after_ten_visit_num_(0),
              stock_code_(""),
              visit_num_(0),
              change_percent_(0),
              close_(0),
              stock_name_(""){
    }

    std::string stock_code_;
    std::string stock_name_;
    double change_percent_;
    int visit_num_;
    double close_;
    int before_ten_visit_num_;
    int after_ten_visit_num_;
};

class StockYieldCmp {
 public:
    bool operator()(const StockSortByYield* lhs, const StockSortByYield* rhs) const {
        return lhs->change_percent_ > rhs->change_percent_;
    }
};

class StockYieldMgr {
 public:
    typedef std::set<StockSortByYield*, StockYieldCmp> STOCK_SET;
    typedef std::map<std::string, StockSortByYield*> STOCK_MAP;

    //创建或添加
    StockSortByYield* CreateOrGetVisitInfo(const std::string& stock_code) {
        STOCK_MAP::iterator iter(stock_map_.find(stock_code));
        if (iter == stock_map_.end()) {
            StockSortByYield* temp = new StockSortByYield();
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

}

class BestYieldStockVisitChange : public Observer {
 public:
    typedef std::map<std::string, StockYieldMgr> BEST_YIELD_VISIT_MAP;
    BestYieldStockVisitChange(stock_logic::Subject* subject);
    virtual ~BestYieldStockVisitChange();
    void UpdateStockHistData(stock_logic::Subject* subject);

 public:
    virtual void Update(int opcode, stock_logic::Subject*);
    virtual base_logic::DictionaryValue* Request(
            base_logic::DictionaryValue* params);

    bool CheckUpdateDateExists(const std::string &date,
                               const std::string& stock_code) {
        BEST_YIELD_VISIT_MAP::iterator iter(stock_visit_map_.find(date));
        if (iter == stock_visit_map_.end()) {
            return false;
        }
        return iter->second.CheckStockIsUpdate(stock_code);
    }

 private:
    void HandleBestYieldStockVisitChange(const std::string& stock_code,
                                         const std::string &date,
                                         StockSortByYield* one_stock, int range_num);

 private:
    BEST_YIELD_VISIT_MAP stock_visit_map_;
};

} /* namespace stock_logic */

#endif /* KID_PLUGINS_STOCK1_STOCK_BestYieldStockVisitChange_H_ */
