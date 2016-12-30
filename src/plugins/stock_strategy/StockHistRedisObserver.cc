/*
 * StockHistRedisObserver.cc
 *
 *  Created on: 2016年8月23日
 *      Author: harvey
 */

#include "StockHistRedisObserver.h"

#include "stock_basic_info.h"
#include "stock_logic.h"
#include "stock_factory.h"
#include "stock_user_mgr.h"
#include "stock_redis_db.h"

#define STOCK_INIT_SUCCESS     0
#define RECENT_UPDATE_DATE      "recent_update_date"

namespace stock_logic {
namespace {

STOCK_SPECIAL_REDIS_INFO_MAP StockRedisTotalInfo::total_once_map;
StockRedisTotalInfo::StockRedisTotalInfo(StockBasicInfo& stock_base_info)
        : stock_base_info_(stock_base_info) {
}

StockRedisTotalInfo::~StockRedisTotalInfo() {
    STOCK_HIST_REDIS_MAP::iterator hist_iter(stock_hist_redis_map_.begin());
    for (; hist_iter != stock_hist_redis_map_.end();) {
        STOCK_HIST_REDIS_MAP::iterator hist_iter_temp(hist_iter);
        ++hist_iter;
        delete hist_iter_temp->second;
        stock_hist_redis_map_.erase(hist_iter_temp);
    }
}

void StockRedisTotalInfo::GetOrCreateRedisStockInfo(
        std::string& date, StockRedisDateInfo** stock_hist_info) {
    STOCK_HIST_REDIS_MAP::iterator iter(stock_hist_redis_map_.find(date));
    if (iter == stock_hist_redis_map_.end()) {
        *stock_hist_info = new StockRedisDateInfo();
        stock_hist_redis_map_[date] = *stock_hist_info;
        return;
    }
    *stock_hist_info = iter->second;
}

bool StockRedisTotalInfo::GetStockRecentUpateDate() {
    //get recent update hist stock date
    std::string stock_code = stock_base_info_.code_;
    std::string key = RECENT_UPDATE_DATE;
    StockRedisDB::GetPtr()->theRedisEngine()->GetHashElement(
            key, stock_code, recent_update_date_);
    LOG_MSG2("Stock = %s, RecentUpdate Redis Date = %s",
            stock_code.c_str(), recent_update_date_.c_str());
    return true;
}

void StockRedisTotalInfo::OnUpdate(int opcode) {
    switch (opcode) {
        case 0: {
            OnInitWriteRedisOnce();
            OnWriteStockRedisAllHist();
            break;
        }
        case UPDATE_STOCK_HIST_DATA: {
            OnWriteStockRedisAllHist();
            break;
        }
        case UPDATE_REALTIME_STOCK_INFO: {
            //TUDO
            break;
        }
        default: {
            break;
        }
    }
}

//每只股票只有一组的数据更新
void StockRedisTotalInfo::OnInitWriteRedisOnce() {
    bool r = false;
    base_logic::RedisController* redis_engine = StockRedisDB::GetPtr()
            ->theRedisEngine();
    std::string& stock_code = stock_base_info_.code_;
    PrepareWriteRedisOnceInfo();
    std::map<std::string, double>::iterator iter(
            once_write_redis_map_.begin());
    for (; iter != once_write_redis_map_.end(); ++iter) {
        std::string hash_name = iter->first;
        redis_engine->AddValueInZSet(hash_name.c_str(), stock_code.c_str(),
                                     stock_code.size(), iter->second);
    } LOG_MSG2("zadd stock once info success, stock_code = %s", stock_code.c_str());
}

//每只股票的所有每天一组的数据更新
void StockRedisTotalInfo::OnWriteStockRedisAllHist() {
    std::string& stock_code = stock_base_info_.code_;
    StockUserManager* stock_user_mgr = StockUserEngine::GetVIPUserManager();
    StockUserCache* cache = stock_user_mgr->stock_user_cache_;
    STOCKINFO_MAP& total_stock_map = cache->stock_total_info_;
    STOCKINFO_MAP::iterator stock_iter(total_stock_map.find(stock_code));
    if (stock_iter == total_stock_map.end()) {
        LOG_ERROR2("StockRedisTotalInfo::OnWriteStockRedisAllHist Not Find Stock = %s, hist_map",
                stock_code.c_str());
        return;
    }
    int count = 0;
    std::map<std::string, HistDataPerDay>& hist_data_map = stock_iter->second
            .hist_data_info_.stock_hist_data_;
    if( hist_data_map.empty() ){
      LOG_ERROR2("stock hist_data_map empty, stock_code = %s", stock_code.c_str());
      return;
    }
    std::map<std::string, HistDataPerDay>::reverse_iterator rev_iter(hist_data_map.rbegin());
    std::string curr_max_date = rev_iter->first;
    for( ; hist_data_map.rend() != rev_iter; ++rev_iter){
        std::string date = rev_iter->first;
        if( date > recent_update_date_ ){
            HistDataPerDay& hist_date_info = rev_iter->second;
            StockRedisDateInfo* stock_hist_info = NULL;
            GetOrCreateRedisStockInfo(recent_update_date_, &stock_hist_info);
            stock_hist_info->WriteStockHistPerDay(stock_code, &hist_date_info);
            ++count;
        }
    }
    recent_update_date_ = curr_max_date;
    std::string str_redis_key = RECENT_UPDATE_DATE;
    StockRedisDB::GetPtr()->theRedisEngine()->AddHashElement(
            str_redis_key, stock_code, recent_update_date_);
    LOG_MSG2("zadd stock hist info success, stock_code = %s, size = %d, recent_update_date = %s",
            stock_code.c_str(), count, recent_update_date_.c_str());
}

void StockRedisDateInfo::WriteStockHistPerDay(const std::string& stock_code,
                                              HistDataPerDay* hist_data_info) {
    std::string& date = hist_data_info->date_;
    PrepareWriteRedisHistInfo(hist_data_info);
    base_logic::RedisController* redis_engine = StockRedisDB::GetPtr()
            ->theRedisEngine();
    std::map<std::string, double>::iterator iter(hist_write_redis_map_.begin());
    for (; iter != hist_write_redis_map_.end(); ++iter) {
        std::string hash_name = iter->first + "_" + date;
        redis_engine->AddValueInZSet(hash_name.c_str(), stock_code.c_str(),
                                     stock_code.size(), iter->second);
    }
}

} /*namesapce*/

StockHistRedisObserver::StockHistRedisObserver(Subject* subject) {
    subject_ = subject;
    name_ = "StockHistRedisObserver";
    redis_engine_ = StockRedisDB::GetPtr()->theRedisEngine();
}

StockHistRedisObserver::~StockHistRedisObserver() {
    ALL_STOCK_REDIS_MAP::iterator iter(all_stock_redis_map_.begin());
    for (; iter != all_stock_redis_map_.end();) {
        ALL_STOCK_REDIS_MAP::iterator iter_temp(iter);
        ++iter;
        delete iter_temp->second;
        all_stock_redis_map_.erase(iter_temp);
    }
}

void StockHistRedisObserver::GetOrCreateStockRedisTotalInfo(
        StockBasicInfo& stock_base_info,
        StockRedisTotalInfo** stock_redis_info) {
    std::string& stock_code = stock_base_info.code_;
    ALL_STOCK_REDIS_MAP::iterator iter(all_stock_redis_map_.find(stock_code));
    if (iter == all_stock_redis_map_.end()) {
        *stock_redis_info = new StockRedisTotalInfo(stock_base_info);
        all_stock_redis_map_[stock_code] = (*stock_redis_info);
        return;
    }
    *stock_redis_info = iter->second;
}

void StockHistRedisObserver::UpdateSpecialStockInfo() {
    STOCK_SPECIAL_REDIS_INFO_MAP::iterator iter(StockRedisTotalInfo::total_once_map.begin());
    for( ; StockRedisTotalInfo::total_once_map.end() != iter; ++iter ){
        std::string hash_name = iter->first;
        std::map<std::string, std::string>& temp_map = iter->second;
        std::map<std::string, std::string>::iterator temp_iter( temp_map.begin() );
        for( ; temp_map.end() != temp_iter; ++temp_iter ){
            std::string key = temp_iter->first;
            std::string value = temp_iter->second;
            redis_engine_->AddHashElement(hash_name, key, value);
            LOG_MSG2("UpdateSpecialStockInfo hash_name=%s, key=%s, value=%s",
                     hash_name.c_str(), key.c_str(), value.c_str());
        }
    }
}

void StockHistRedisObserver::Update(int opcode, Subject* subject) {
    // 初始化隔天更新
    if (0 != opcode && UPDATE_STOCK_HIST_DATA != opcode) {
        return;
    }
    StockFactory* factory = (StockFactory*) subject_;
    StockUserCache* cache = factory->stock_usr_mgr_->stock_user_cache_;
    STOCKINFO_MAP& stock_total_map = cache->stock_total_info_;
    STOCKINFO_MAP::iterator total_stock_iter(stock_total_map.begin());
    std::ofstream ofs("./valid_stock.txt", std::ios::trunc);
    for (; total_stock_iter != stock_total_map.end(); ++total_stock_iter) {
        std::string stock_code = total_stock_iter->first;
        if (stock_code.empty()) {
            continue;
        }
        //TUDO 未上市股票
        StockBasicInfo& stock_base_info = total_stock_iter->second.basic_info_;
        if (stock_base_info.totals_ <= 0
                || stock_base_info.market_value_ <= 0) {
            ofs << stock_code << std::endl;
            continue;
        }
        StockRedisTotalInfo* stock_redis_info;
        GetOrCreateStockRedisTotalInfo(stock_base_info, &stock_redis_info);

        //获取该股票最近更新的交易日
        stock_redis_info->GetStockRecentUpateDate();
        stock_redis_info->OnUpdate(opcode);
    }
    ofs.close();

    //更新特殊需要一次更新所有股票的字段
    UpdateSpecialStockInfo();
}

} /* namespace stock_logic */
