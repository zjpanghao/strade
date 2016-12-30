/*
 * stock_redis_db.h
 *
 *  Created on: 2016年8月25日
 *      Author: harvey
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_STOCK_REDIS_DB_H_
#define GEORGE_MASTER_PLUGINS_STOCK_STOCK_REDIS_DB_H_

#include "logic/base_values.h"
#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "basic/scoped_ptr.h"
#include "stock_basic_info.h"
#include "storage/data_engine.h"

#include "storage/redis_controller.h"

#include <string>
#include <list>
#include <map>

namespace stock_logic {

class StockRedisDB {
 public:
    virtual ~StockRedisDB();
    static StockRedisDB* GetPtr();
    void InitReids(config::FileConfig* config);

 private:
    StockRedisDB();
    StockRedisDB(const StockRedisDB& rhs);
    StockRedisDB &operator=(const StockRedisDB& rhs);

 public:


 public:
    base_logic::RedisController* theRedisEngine(){
        return redis_engine_;
    }

 private:
    base_logic::RedisController* redis_engine_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_STOCK_REDIS_DB_H_ */
