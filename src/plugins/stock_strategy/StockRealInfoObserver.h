/*
 * StockRealInfoObserver.h
 *
 *  Created on: 2016骞�8鏈�17鏃�
 *      Author: harvey
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_STOCKREALINFOOBSERVER_H_
#define GEORGE_MASTER_PLUGINS_STOCK_STOCKREALINFOOBSERVER_H_

#include "Observer.h"

#include "StockRealInfoKafka.h"

#include <map>

namespace stock_logic {

class StockRealInfoObserver : public Observer {
 public:
    typedef std::map<std::string, StockKafkaInfo*>      STOCK_CACHE_MAP;
    StockRealInfoObserver(stock_logic::Subject* subject);
    virtual ~StockRealInfoObserver();

 public:
    virtual void Update(int opcode, stock_logic::Subject* subject);

 private:
    void Initialize();
    void HandleStockRealInfo();

 private:
    StockRealInfoKafka          stock_kafka_mgr_;
    STOCK_CACHE_MAP             stock_cache_map_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_STOCKREALINFOOBSERVER_H_ */
