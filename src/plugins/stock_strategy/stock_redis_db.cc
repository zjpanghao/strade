/*
 * stock_redis_db.cc
 *
 *  Created on: 2016年8月25日
 *      Author: harvey
 */

#include "stock_redis_db.h"

namespace stock_logic {

StockRedisDB::StockRedisDB() {
    redis_engine_ =
            dynamic_cast<base_logic::RedisController*>(base_logic::DataEngine::Create(
                    REIDS_TYPE));
}

StockRedisDB::~StockRedisDB() {
    if (redis_engine_) {
        delete redis_engine_;
        redis_engine_ = NULL;
    }
}

void StockRedisDB::InitReids(config::FileConfig* config) {
    redis_engine_->InitParam(config->redis_list_);
}

StockRedisDB* StockRedisDB::GetPtr() {
    static StockRedisDB instance;
    return &instance;
}

} /* namespace stock_logic */
