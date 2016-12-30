/*
 * StockRealInfoKafka.h
 *
 *  Created on: 2016年8月17日
 *      Author: harvey
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_STOCKREALINFOKAFKA_H_
#define GEORGE_MASTER_PLUGINS_STOCK_STOCKREALINFOKAFKA_H_

#include "logic/base_values.h"
#include "queue/kafka_producer.h"

#include <list>
#include <map>

namespace stock_logic {

class StockKafkaInfo{
 public:
    StockKafkaInfo(const std::string& stock_code)
         : stock_code_(stock_code),
           change_percent_(0.0){

    }

    void Reset(double change_percent){
        change_percent_ = change_percent;
    }

 private:
    StockKafkaInfo(const StockKafkaInfo& rhs);
    StockKafkaInfo& operator=(const StockKafkaInfo& rhs);

 public:
    void SerializeValue(base_logic::DictionaryValue& dict){
        dict.SetString(L"stock_code", stock_code_);
        dict.SetReal("change_percent", change_percent_);
    }

 public:
    std::string     stock_code_;            //股票代码
    double          change_percent_;        //涨跌幅
};

class StockRealInfoKafka {
 public:
    StockRealInfoKafka();
    ~StockRealInfoKafka();

 public:
    bool ReportStockRealInfo(int time, std::list<StockKafkaInfo*>& stock_list);

 private:
    kafka_producer  kafka_producer_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_STOCKREALINFOKAFKA_H_ */
