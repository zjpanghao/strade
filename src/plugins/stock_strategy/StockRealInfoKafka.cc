/*
 * StockRealInfoKafka.cpp
 *
 *  Created on: 2016年8月17日
 *      Author: harvey
 */

#include "StockRealInfoKafka.h"

#include <mysql/mysql.h>
#include <sstream>
#include <set>
#include "logic/logic_unit.h"
#include "logic/logic_comm.h"
#include "basic/basic_util.h"
#include "basic/template.h"
#include "basic/radom_in.h"
#include "basic/scoped_ptr.h"

namespace stock_logic {

StockRealInfoKafka::StockRealInfoKafka() {
    int ret = kafka_producer_.Init(0, "stock_yield_update", "61.147.114.85:9092",
                                   NULL);
    if (PRODUCER_INIT_SUCCESS != ret) {
        LOG_DEBUG("kafka producer stock_yield_update init failed");
        assert(0);
    }
}

StockRealInfoKafka::~StockRealInfoKafka() {
    kafka_producer_.Close();
}

bool StockRealInfoKafka::ReportStockRealInfo(int time, std::list<StockKafkaInfo*>& stock_list){
    if( stock_list.size() <= 0 ){
        return false;
    }
    int ret = PUSH_DATA_SUCCESS;
    base_logic::DictionaryValue stock_dict;
    base_logic::ListValue *list_value = new base_logic::ListValue();
    std::list<StockKafkaInfo*>::iterator iter(stock_list.begin());
    for( ; iter != stock_list.end(); ++iter ){
        base_logic::DictionaryValue *dict = new base_logic::DictionaryValue();
        (*iter)->SerializeValue(*dict);
        list_value->Append(dict);
    }
    stock_dict.SetInteger("current_trade_time", time);
    stock_dict.Set("stock_real_time_list", list_value);

    ret = kafka_producer_.PushData(&stock_dict);
    if( PUSH_DATA_SUCCESS != ret ){
        LOG_ERROR("ReportStockRealInfo kafka producer failed");
        return false;
    }
    return true;
}

} /* namespace stock_logic */
