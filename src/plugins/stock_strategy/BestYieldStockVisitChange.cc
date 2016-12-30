/*
 * inc_ten_stock.cc
 *
 *  Created on: 2016年8月10日
 *      Author: harvey
 */

#include "BestYieldStockVisitChange.h"

#include "stock_basic_info.h"
#include "stock_logic.h"
#include "stock_factory.h"
#include "stock_user_mgr.h"

#include "logic/logic_comm.h"

namespace stock_logic {

typedef std::map<std::string, DataPerDay> DATAPERDAY_MAP;
typedef std::map<std::string, HistDataPerDay> HISTDATAPERDAY_MAP;

BestYieldStockVisitChange::BestYieldStockVisitChange(
        stock_logic::Subject* subject) {
    name_ = "BestYieldStockVisitChange";
    subject_ = subject;
}

BestYieldStockVisitChange::~BestYieldStockVisitChange() {

}

void BestYieldStockVisitChange::Update(int opcode,
                                        stock_logic::Subject* subject) {
    switch (opcode) {
        case 0: {
            UpdateStockHistData(subject);
            break;
        }
        case UPDATE_STOCK_OFFLINE_VISIT_DATA: {
            UpdateStockHistData(subject);
            break;
        }
        default:
            break;
    }
}

base_logic::DictionaryValue* BestYieldStockVisitChange::Request(
        base_logic::DictionaryValue* params) {
    bool r = true;
    std::string start_date = ",2016-01-01";
    r = params->GetString(L"start_date", &start_date);
    if (start_date.size() > 1) {
        start_date.erase(start_date.begin());
    }
    std::string end_date = ",2016-06-30";
    //std::string end_date = ",2016-08-01";
    r = params->GetString(L"end_date", &end_date);
    if (end_date.size() > 1) {
        end_date.erase(end_date.begin());
    }LOG_MSG2("end_date=%s", end_date.c_str());

    if ( !end_date.empty() && end_date < start_date) {
        end_date = start_date;
    }

    base_logic::DictionaryValue* total_dict = new base_logic::DictionaryValue();
    BEST_YIELD_VISIT_MAP::iterator iter_map = stock_visit_map_.begin();
    LOG_MSG2("stock_visit_map_.size()=%d", stock_visit_map_.size());
    for (; iter_map != stock_visit_map_.end(); iter_map++) {
        std::string date = iter_map->first;
        if (date < start_date ||
                (!end_date.empty() && date > end_date)) {
            continue;
        }
        StockYieldMgr& info = iter_map->second;
        int count = 0;
        base_logic::ListValue *list_dict = new base_logic::ListValue();
        StockYieldMgr::STOCK_SET::iterator iter_set(
                info.stock_set_.begin());
        for (; iter_set != info.stock_set_.end() && count < 10; ++iter_set) {
            StockSortByYield* one_stock = (*iter_set);
            base_logic::DictionaryValue* stock_dict =
                    new base_logic::DictionaryValue();
            std::string stock_code = one_stock->stock_code_;

            HandleBestYieldStockVisitChange(stock_code, date, one_stock, 10);

            stock_dict->SetString(L"stock_code", stock_code.c_str());
            stock_dict->SetString(L"stock_name", one_stock->stock_name_);
            stock_dict->SetReal(L"change_percent", one_stock->change_percent_);
            stock_dict->SetInteger(L"visit_num", one_stock->visit_num_);
            stock_dict->SetReal(L"close", one_stock->close_);
            stock_dict->SetInteger(L"before_visit_num",
                                   one_stock->before_ten_visit_num_);
            stock_dict->SetInteger(L"after_visit_num",
                                   one_stock->after_ten_visit_num_);
            list_dict->Append(stock_dict);
            ++count;
        }
        total_dict->Set(std::wstring(date.begin(), date.end()), list_dict);
    }
    return total_dict;
}

void BestYieldStockVisitChange::UpdateStockHistData(
        stock_logic::Subject* subject) {
    LOG_MSG("IncreaseStockVisit::UpdateStockHistData");
    StockFactory* factory = (StockFactory*) subject;
    StockUserManager* user_mgr = factory->stock_usr_mgr_;
    StockUserCache* cache = factory->stock_usr_mgr_->stock_user_cache_;
    STOCKINFO_MAP& stock_total_map = cache->stock_total_info_;
    STOCKINFO_MAP::iterator total_iter = stock_total_map.begin();
    for (; total_iter != stock_total_map.end(); total_iter++) {
        std::string stock_code = total_iter->first;
        StockBasicInfo& stock_base_info = total_iter->second.basic_info_;
        DATAPERDAY_MAP& data_per_day = total_iter->second.basic_info_.data_per_day_;
        HISTDATAPERDAY_MAP& stock_hist_data = total_iter->second.hist_data_info_.stock_hist_data_;
        HISTDATAPERDAY_MAP::iterator hist_iter = stock_hist_data.begin();

        for (; hist_iter != stock_hist_data.end(); hist_iter++) {
            //每天更新UPDATE_STOCK_OFFLINE_VISIT_DATA，防止重复更新历史数据
            std::string date = hist_iter->first;
            if (CheckUpdateDateExists(date, stock_code)) {
                continue;
            }

            //获得股票当天访问量
            user_mgr->load_stock_offline_visit_num(stock_code, date);

            StockYieldMgr& per_day_visit_info = stock_visit_map_[date];
            double stock_close = hist_iter->second.close_;
            double stock_change_percent = hist_iter->second.day_yield_;

            data_per_day[date].changepercent_ = stock_change_percent;
            data_per_day[date].close_ = stock_close;

            StockSortByYield* one_stock = per_day_visit_info.CreateOrGetVisitInfo(
                    stock_code);
            one_stock->stock_code_ = stock_code;
            one_stock->change_percent_ = stock_change_percent;
            one_stock->close_ = stock_close;
            one_stock->stock_name_ = stock_base_info.name();

            //当前日期的热度
            one_stock->visit_num_ = data_per_day[date].visit_per_day_num_;

            //插入排序 set
            per_day_visit_info.stock_set_.insert(one_stock);
        }
    }
}

void BestYieldStockVisitChange::HandleBestYieldStockVisitChange(
        const std::string& stock_code, const std::string &date,
        StockSortByYield* one_stock, int range_num) {

    StockFactory* factory = dynamic_cast<StockFactory*>(subject_);
    StockUserCache* cache = factory->stock_usr_mgr_->stock_user_cache_;
    STOCKINFO_MAP& stock_total_map = cache->stock_total_info_;
    STOCKINFO_MAP::iterator stock_total_iter(stock_total_map.find(stock_code));
    if (stock_total_iter == stock_total_map.end()) {
        LOG_MSG2("CalcRangeVisitNum not find stock: %s", stock_code.c_str());
        return;
    }

    int before_ten_visit_num = 0;
    int after_ten_visit_num = 0;
    int before_count = 0;
    int after_count = 0;
    DATAPERDAY_MAP& data_per_day = stock_total_iter->second.basic_info_.data_per_day_;
    DATAPERDAY_MAP::iterator stock_date_iter(data_per_day.find(date));
    if (stock_date_iter == data_per_day.end()) {
        LOG_MSG2("CalcRangeVisitNum not find stock: %s, date: %s",
                stock_code.c_str(), date.c_str());
        return;
    }

    //计算指定日期 前 range_num 天数的总热度
    DATAPERDAY_MAP::reverse_iterator before_temp_iter(stock_date_iter);
    for (; before_temp_iter != data_per_day.rend();
            ++before_temp_iter, ++before_count) {
        if (before_count >= range_num) {
            one_stock->before_ten_visit_num_ = before_ten_visit_num;
            break;
        }
        before_ten_visit_num += before_temp_iter->second.visit_per_day_num_;
    }

    //计算指定日期 后 rang_num 天数的总热度
    DATAPERDAY_MAP::iterator after_temp_iter(stock_date_iter);
    for (++after_temp_iter; after_temp_iter != data_per_day.end();
            ++after_temp_iter, ++after_count) {
        if (after_count >= range_num) {
            one_stock->after_ten_visit_num_ = after_ten_visit_num;
            break;
        }
        after_ten_visit_num += after_temp_iter->second.visit_per_day_num_;
    } LOG_MSG2("stock = %s, date = %s, current = %d, before 10 = %d, after 10 = %d",
            stock_code.c_str(), date.c_str(), one_stock->visit_num_,
            before_ten_visit_num, after_ten_visit_num);
}

} /* namespace stock_logic */
