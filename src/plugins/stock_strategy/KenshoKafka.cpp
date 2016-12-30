/*
 * KenshoKafka.cpp
 *
 *  Created on: 2016年9月15日
 *      Author: Maibenben
 */

#include "KenshoKafka.h"
#include "logic/logic_comm.h"
#include <set>

namespace stock_logic {

KenshoKafka::KenshoKafka() {
  Init();
}

KenshoKafka::~KenshoKafka() {
  kafka_producer_.Close();
  kafka_consumer_.Close();
}

void KenshoKafka::Init() {
  InitPorducer();
  InitConsumer();
}

void KenshoKafka::InitPorducer() {
  //if (PRODUCER_INIT_SUCCESS
  // != kafka_producer_.Init(0, "backtesting_filter", "192.168.1.85:9092",
  //                       NULL))
  if (PRODUCER_INIT_SUCCESS
      != kafka_producer_.Init(
          0,
          "backtesting_test",
          "61.147.114.85:9092",
          NULL))
    LOG_ERROR("producer backtesting_test init failed");
  else
    LOG_ERROR("producer backtesting_test init success");
}

void KenshoKafka::InitConsumer() {
  if (CONSUMER_INIT_SUCCESS
      != kafka_consumer_.Init(
          0,
          "backtesting_test",
          "61.147.114.85:9092",
          NULL))
    LOG_ERROR("kafka consumer backtesting_test init failed");
  else
    LOG_MSG("kafka consumer backtesting_test init success");
  if (CONSUMER_INIT_SUCCESS
        != kafka_consumer1_.Init(
            1,
            "backtesting_test",
            "192.168.1.85:9091,192.168.1.80:9091,192.168.1.81:9091,192.168.1.82:9091,192.168.1.84:9091",
            NULL))
      LOG_ERROR("kafka kafka_consumer1_ backtesting_test init failed");
    else
      LOG_MSG("kafka kafka_consumer1_ backtesting_test init success");
  if (CONSUMER_INIT_SUCCESS
        != kafka_consumer2_.Init(
            2,
            "backtesting_test",
            "192.168.1.85:9091,192.168.1.80:9091,192.168.1.81:9091,192.168.1.82:9091,192.168.1.84:9091",
            NULL))
      LOG_ERROR("kafka kafka_consumer2_ backtesting_test init failed");
    else
      LOG_MSG("kafka kafka_consumer2_ backtesting_test init success");
}

bool KenshoKafka::AddKafkaConditionInfo(base_logic::DictionaryValue* value) {

  /*{"id":560, "attrid":14, "depth":3, "cur_depth":2, "method":2, "url":"http://tech.caijing.com.cn/index.html"}
   * */
  int re = PUSH_DATA_SUCCESS;
  re = kafka_producer_.PushData(value);
  if (PUSH_DATA_SUCCESS == re)
    return true;
  else {
    LOG_ERROR("kafka producer send data failed");
    return false;
  }
}

bool KenshoKafka::FectchBatchKenshoResult(
    std::list<base_logic::DictionaryValue*>& list) {
  std::set < std::string > data_list;
  std::string data = "";

  for (int i = 0; i < 2; i++) {
    sleep(1);
    int pull_re = kafka_consumer_.PullData(data);
    if (0 == pull_re) {
      LOG_MSG2("PULL_DATA_DATA_NULL,pull_re=%d", pull_re);
    }
    if (PULL_DATA_TIMEOUT == pull_re) {
      LOG_MSG2("consumer get url timeout,pull_re=%d", pull_re);
      //break;
    }
    if (data.size() > 0)
      LOG_MSG2("kafka_consumer_ get data=%s", data.c_str());
    data = "";
    data_list.insert(data);
  }
  for (int i = 0; i < 2; i++) {
      sleep(1);
      int pull_re = kafka_consumer1_.PullData(data);
      if (0 == pull_re) {
        LOG_MSG2("kafka_consumer1_ PULL_DATA_DATA_NULL,pull_re=%d", pull_re);
      }
      if (PULL_DATA_TIMEOUT == pull_re) {
        LOG_MSG2("kafka_consumer1_ get url timeout,pull_re=%d", pull_re);
        //break;
      }
      if (data.size() > 0)
        LOG_MSG2("kafka_consumer1_ get data=%s", data.c_str());
      data="";
      data_list.insert(data);
    }
  for (int i = 0; i < 2; i++) {
      sleep(1);
      int pull_re = kafka_consumer2_.PullData(data);
      if (0 == pull_re) {
        LOG_MSG2("kafka_consumer2_ PULL_DATA_DATA_NULL,pull_re=%d", pull_re);
      }
      if (PULL_DATA_TIMEOUT == pull_re) {
        LOG_MSG2("kafka_consumer2_ get url timeout,pull_re=%d", pull_re);
        //break;
      }
      if (data.size() > 0)
        LOG_MSG2("kafka_consumer2_ get data=%s", data.c_str());
      data="";
      data_list.insert(data);
    }
  base_logic::ValueSerializer* engine = base_logic::ValueSerializer::Create(
      base_logic::IMPL_JSON, &data);
  for (std::set<std::string>::iterator it = data_list.begin();
      it != data_list.end(); it++) {
    std::string data = *it;
    int error_code = 0;
    std::string error_str;
    base_logic::DictionaryValue* value = (base_logic::DictionaryValue*) engine
        ->Deserialize(&error_code, &error_str);
    if (0 != error_code || NULL == value)
      continue;
    list.push_back(value);
  }

  base_logic::ValueSerializer::DeleteSerializer(base_logic::IMPL_JSON, engine);
  return true;
}

void KenshoKafka::test() {

  int message_count=0;
  char message_count_str[20];
  while(true) {
    LOG_MSG("KenshoKafkatest");
    base_logic::DictionaryValue* value = new base_logic::DictionaryValue();
    string test_str = "eee";
    sprintf(message_count_str, "%d", message_count);
    test_str += message_count_str;
    value->SetString(L"test", test_str);
    this->AddKafkaConditionInfo(value);
    delete value;
    //sleep(3);
    std::list<base_logic::DictionaryValue*> test_list;
    this->FectchBatchKenshoResult(test_list);
    message_count++;
  }

}

} /* namespace stock_logic */
