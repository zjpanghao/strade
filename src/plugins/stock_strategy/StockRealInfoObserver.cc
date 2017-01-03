/*
 * StockRealInfoObserver.cpp
 *
 *  Created on: 2016年8月17日
 *      Author: harvey
 */

#include "StockRealInfoObserver.h"
#include "stock_basic_info.h"
#include "stock_logic.h"
#include "stock_factory.h"
#include "stock_user_mgr.h"

namespace stock_logic {

StockRealInfoObserver::StockRealInfoObserver(stock_logic::Subject* subject) {
    subject_ = subject;
    name_ = "StockRealInfoObserver";
    Initialize();
}

StockRealInfoObserver::~StockRealInfoObserver() {
    STOCK_CACHE_MAP::iterator iter(stock_cache_map_.begin());
    for( ; iter != stock_cache_map_.end();  ){
        STOCK_CACHE_MAP::iterator iter_temp(iter);
        ++iter;
        delete iter_temp->second;
        stock_cache_map_.erase(iter_temp);
    }
}

//init kafka stock cache
void StockRealInfoObserver::Initialize(){
    StockFactory* factory = (StockFactory*) subject_;
    StockUserCache* cache = factory->stock_usr_mgr_->stock_user_cache_;
    STOCKINFO_MAP& stock_total_map = cache->stock_total_info_;
    STOCKINFO_MAP::iterator total_iter(stock_total_map.begin());
    for( ; total_iter != stock_total_map.end(); ++total_iter ){
        std::string stock_code = total_iter->first;
        StockBasicInfo& stock_base_info = total_iter->second.basic_info_;
        StockKafkaInfo* stock_info = new StockKafkaInfo(stock_code);
        stock_cache_map_[stock_code] = stock_info;
    }
}

void StockRealInfoObserver::Update(int opcode, stock_logic::Subject* subject) {
    switch (opcode) {
        case UPDATE_REALTIME_STOCK_INFO: {
            if( StockUtil::Instance()->is_trading_time() ){
                HandleStockRealInfo();
            }
            break;
        }
        default: {
            break;
        }
    }
}

void StockRealInfoObserver::HandleStockRealInfo() {
    StockFactory* factory = (StockFactory*) subject_;
    StockUserCache* cache = factory->stock_usr_mgr_->stock_user_cache_;
    STOCKINFO_MAP& stock_total_map = cache->stock_total_info_;

    //find hs300 trade time
    int current_trade_time = time(NULL);
    STOCKINFO_MAP::iterator hs300_iter(stock_total_map.find(HSSANBAI));
    if( hs300_iter != stock_total_map.end()){
        current_trade_time = hs300_iter->second.basic_info_.current_trade_time();
    }else{
        LOG_ERROR("Not find hs300 stock trade time");
    }

    int total_count = 0;
    std::list<StockKafkaInfo*> stock_list;
    STOCKINFO_MAP::iterator total_iter(stock_total_map.begin());
    while( total_iter != stock_total_map.end() ){
        std::string stock_code = total_iter->first;
        StockBasicInfo& stock_base_info = total_iter->second.basic_info_;

        //get kafka stock cache
        StockKafkaInfo* stock_info = NULL;
        STOCK_CACHE_MAP::iterator stock_cache_iter(stock_cache_map_.find(stock_code));
        if( stock_cache_iter == stock_cache_map_.end() ){
            stock_info = new StockKafkaInfo(stock_code);
            stock_cache_map_[stock_code] = stock_info;
        }else{
            stock_info = stock_cache_iter->second;
        }

        //reset change value
        stock_info->Reset(stock_base_info.change_percent_);
        stock_list.push_back(stock_info);
        if( stock_list.size() >= 5 ){
            stock_kafka_mgr_.ReportStockRealInfo(current_trade_time, stock_list);
            total_count += stock_list.size();
            stock_list.clear();
        }
        ++total_iter;
    }

    //发送剩余不够的
    if( stock_list.size() > 0){
        stock_kafka_mgr_.ReportStockRealInfo(current_trade_time, stock_list);
        total_count += stock_list.size();
    }

    LOG_MSG2("write kafka stock real success, stock_num = %d, current_trade_time = %d",
             total_count, current_trade_time);
}

} /* namespace stock_logic */
