/*
 * KenshoKafka.h
 *
 *  Created on: 2016年9月15日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_KENSHOKAFKA_H_
#define GEORGE_MASTER_PLUGINS_STOCK_KENSHOKAFKA_H_

#include <list>
#include "queue/kafka_producer.h"
#include "queue/kafka_consumer.h"

using namespace std;

namespace stock_logic {

class KenshoKafka {
 public:
  KenshoKafka();
  virtual ~KenshoKafka();

 private:
  void Init();
  void InitPorducer();
  void InitConsumer();
 public:
  bool AddKafkaConditionInfo(base_logic::DictionaryValue* value);
  bool FectchBatchKenshoResult(std::list<base_logic::DictionaryValue*>& list);

  void test();

 private:
  kafka_producer     kafka_producer_;
  kafka_consumer     kafka_consumer_;
  kafka_consumer     kafka_consumer1_;
  kafka_consumer     kafka_consumer2_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_KENSHOKAFKA_H_ */
