// author: pangh
// date: Mon Jun 20 16:50:31 2016

#include "realinfo_logic.h"
#include "core/common.h"
#include "base/logic/logic_comm.h"
#include "base/logic/base_values.h"
#include "public/config/config.h"

#include "logic/logic_unit.h"
#include "net/operator_code.h"
#include "realinfo/realinfo_cache.h"
#include "realinfo/realinfo_pro.h"
//#include "realinfo/src/realinfo_singlestock.h"
//#include "realinfo/src/realinfo_compositeindex.h"
#include "dic/base_dic_redis_auto.h"
#include "strade_share/strade_share_engine.h"
#include <sstream>

#define DEFAULT_CONFIG_PATH                    "./plugins/realinfo/realinfo_config.xml"
#define TIME_REALINFO_UPDATE_ALL                          10000
#define SEND_HTTP_ERROR(b) \  
do {
  std::string response = b;
  base_logic::LogicUnit::SendMessageBySize(socket, response);
}while(0)

#define SEND_ERROR_INFO(c, i) \  
do {
  SendRealInfoLatestProtocol error_pro;
  error_pro.SetErrorState(c, i);
  std::string response = error_pro.GetJson();
  base_logic::LogicUnit::SendMessageBySize(socket, response);
}while(0)
#define SEND_HTTP_INFO(b) \  
do {
  std::string response = b;
  base_logic::LogicUnit::SendMessageBySize(socket, response);
}while(0)

namespace realinfo {

RealInfoLogic* RealInfoLogic::instance_ = NULL;

RealInfoLogic::RealInfoLogic() {
  if (!Init())
    assert(0);
}

RealInfoLogic::~RealInfoLogic() {
}

bool RealInfoLogic::Init() {
  bool r = false;
  srand (time(NULL));engine_
  = GetStradeShareEngine();
  std::string path = DEFAULT_CONFIG_PATH;
  config::FileConfig* config = config::FileConfig::GetFileConfig();
  if (config == NULL) {
    return false;
  }
  r = config->LoadConfig(path);
  // assert(base_dic::KunDicPool::GetInstance()->Init(config->redis_list_, 100));
  return true;
}

RealInfoLogic* RealInfoLogic::GetInstance() {
  if (instance_ == NULL)
    instance_ = new RealInfoLogic();
  return instance_;
}

void RealInfoLogic::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

bool RealInfoLogic::OnRealInfoConnect(struct server *srv, const int socket) {
  return true;
}

bool RealInfoLogic::OnRealInfoMessage(struct server *srv, const int socket,
                                      const void *msg, const int len) {
  LOG_MSG2("Recv message %s\n", "hello, word");
  bool r = true;
  do {
    const char* packet = reinterpret_cast<const char*>(msg);
    if (NULL == packet) {
      r = false;
      break;
    }
    std::string http_str(packet, len);
    std::string error_str;
    int error_code = 0;
    scoped_ptr < base_logic::ValueSerializer
        > serializer(
            base_logic::ValueSerializer::Create(base_logic::IMPL_HTTP,
                                                &http_str));
    NetBase* value = (NetBase*) (serializer.get()->Deserialize(&error_code,
                                                               &error_str));
    LOG_DEBUG2("http_str:%s", http_str.c_str());
    if (NULL == value) {
      error_code = STRUCT_ERROR;
      // send_error(error_code, socket);
      LOG_MSG("error struct ");
      SEND_ERROR_INFO(STRUCT_ERROR, "error struct");
      r = true;
      break;
    }
    scoped_ptr < RecvPacketBase > head_packet(new RecvPacketBase(value));
    int32 type = head_packet->GetType();
    switch (type) {
      case REALINFO_LATESTN: {
        r = OnSingleStockLatestRecords(srv, socket, value, msg, len);
        break;
      }
      case REALINFO_INDEX: {
        r = OnMarketIndexInfo(srv, socket, value, msg, len);
        break;
      }
      default: {
        r = false;
        break;
      }
    }
  } while (0);
  return r;
}

bool RealInfoLogic::OnRealInfoClose(struct server *srv, const int socket) {
  return true;
}

bool RealInfoLogic::OnBroadcastConnect(struct server *srv, const int socket,
                                       const void *msg, const int len) {
  return true;
}

bool RealInfoLogic::OnBroadcastMessage(struct server *srv, const int socket,
                                       const void *msg, const int len) {
  return true;
}

bool RealInfoLogic::OnBroadcastClose(struct server *srv, const int socket) {
  return true;
}

bool RealInfoLogic::OnIniTimer(struct server *srv) {
  if (srv->add_time_task != NULL) {
    srv->add_time_task(srv, "realinfo", TIME_REALINFO_UPDATE_ALL, 600, -1);
  }
  return true;
}

bool RealInfoLogic::OnTimeout(struct server *srv, char *id, int opcode,
                              int time) {
  if (opcode == TIME_REALINFO_UPDATE_ALL) {
    LOG_MSG("Update realinfo");
  }
  return true;
}

//bool RealInfoLogic::GetStockData(
//    const strade_share::STOCK_HIST_MAP &share_map,
//    STOCK_HISTORY_MAP *history_map) {
//  return true;
//}

static StockDealNInfo get_test_data() {

  StockDealNInfo info;
  for (int i = 0; i < 5; i++) {
    StockDealInfo a = { 1.0 + i, 200 * (i + 1) };
    StockDealInfo b = { 1.0 + 2 * i, 400 * (i + 1) };
    info.buy.push_back(a);
    info.sell.push_back(b);
  }
  return info;

}

static StockRealInfo get_test_real_data() {

  StockRealInfo info;
  info.open_price = 3.0;
  info.yesterday_close_price = 2.5;
  info.now_price = 3.1;
  info.low_price = 1.0;
  info.high_price = 5.0;
  info.limit_down = info.open_price * 0.9;
  info.limit_up = info.open_price * 1.1;
  info.amount = 4455;
  info.turnover_rate = 0.84;
  return info;
}

bool RealInfoLogic::GetRealInfo(const strade_logic::StockRealInfo &info,
                                StockRealInfo *real_info,
                                StockDealNInfo *deal_info) {
  real_info->now_price = rand() % 20 + 1;
  return true;
}

std::string RealInfoLogic::GetStradeDay(time_t stamp) {
  time_t seconds = stamp;
  struct tm day;
  localtime_r(&seconds, &day);
  char buf[64];
  snprintf(buf, 64, "%04d-%02d-%02d", day.tm_year + 1900, day.tm_mon + 1,
           day.tm_mday);
  LOG_MSG2("The day is %s", buf);
  return buf;
}

bool RealInfoLogic::CheckCacheDataValid(const std::string &cache_result) {
  std::string result = cache_result;
  scoped_ptr < base_logic::ValueSerializer
      > serializer(
          base_logic::ValueSerializer::Create(base_logic::IMPL_JSON, &result));
  int error_code;
  std::string error_str;
  NetBase *dic = (NetBase*) serializer->Deserialize(&error_code, &error_str);
  time_t timestamp = 0;
  bool time_flag = false;
  if (dic && dic->GetBigInteger(L"timestamp", &timestamp))
    time_flag = true;
  LOG_MSG2("%u", timestamp);
  if (time_flag && timestamp + 60 > time(NULL)) {
    return true;
  }
  return false;
}

std::list<StockRealInfo> build_index_data() {
  std::list<StockRealInfo> ll;
  std::string names[] = { "上证指数", "深证指数", "沪深300" };
  for (int i = 0; i < 3; i++) {
    StockRealInfo info;
    int k = rand() % 20;
    info.now_price = 3940 + k * 300;
    info.change_rate = 0.0004 + (double) k / 10000 * 2;
    info.change_price = 0.03 + k * 0.005;
    info.name = names[i];
    ll.push_back(info);
  }
  return ll;
}

bool RealInfoLogic::OnMarketIndexInfo(struct server *srv, const int socket,
                                      NetBase* netbase, const void *msg,
                                      const int len) {
  SendRealInfoLatestProtocol index_pro;
  RealInfoCache cache("test");
  std::string cache_result;
  if (cache.GetRealInfoIndexCache(&cache_result) == 0
      && CheckCacheDataValid(cache_result)) {
    SEND_HTTP_INFO(cache_result.c_str());
    return true;
  }
  std::list<StockRealInfo> index_list = build_index_data();
  index_pro.SetIndexInfo(index_list);
  std::string json = index_pro.GetJson();
  LOG_MSG2("The json %s\n", json.c_str());
  cache.UpdateRealInfoIndexCache(json);
  SEND_HTTP_INFO(json);
  return true;

}

bool RealInfoLogic::OnSingleStockLatestRecords(struct server *srv,
                                               const int socket,
                                               NetBase* netbase,
                                               const void* msg, const int len) {
  bool r = true;
  int err = 0;
  std::string stock_code;
  int64 record_type;
  r = netbase->GetString(L"stock_code", &stock_code);
  if (!r) {
    err = -1;
    LOG_ERROR("OnSingleCandle error");
    SEND_ERROR_INFO(-100, "error stock_code");
    return false;
  }
  stock_code = stock_code.substr(0, 6);
  LOG_MSG2("stockcodes %s\n", stock_code.c_str());
  RealInfoCache cache("test");
  std::string cache_result;
  if (cache.GetRealInfoLatestCache(stock_code, &cache_result) == 0
      && CheckCacheDataValid(cache_result)) {
    SEND_HTTP_INFO(cache_result.c_str());
    return true;
  }

  LOG_MSG2("%s %d", __FILE__, __LINE__);
  SendRealInfoLatestProtocol candle_pro;
  StockDealNInfo info;
  strade_logic::StockRealInfo stock_real_info;
#if 0
  if (engine_->GetStockCurrRealMarketInfo(stock_code, stock_real_info) == false) {
    LOG_ERROR2("%s", "Get stock error");
    LOG_MSG2("%s", "Get stock error");
    SEND_ERROR_INFO("error GetStockCurrRealMarketInfo error");
    return false;
  }
#endif
  info = get_test_data();
  StockRealInfo real_info = get_test_real_data();
#if 1 
#if 0
  if (engine_->GetStockHistInfoByDate(
          stock_code, GetStradeDay(time(NULL) - 86400), his_info) == false) {
    LOG_MSG2("get his info error %s", GetStradeDay(time(NULL) - 86400).c_str());
    SEND_ERROR_INFO(NULL_DATA, "get his info error");
    return false;
  }
#endif
  strade_logic::STOCK_HIST_MAP hist_map = engine_->GetStockHistMapByCodeCopy(
      stock_code);
  if (hist_map.size() == 0) {
    LOG_MSG2("get his info error %s", stock_code);
    SEND_ERROR_INFO(NULL_DATA, "get his info error");
    return false;
  } else if (hist_map.size() == 1) {
    SEND_ERROR_INFO(NULL_DATA, "only one day data!");
    return false;
  } else {
    strade_logic::STOCK_HIST_MAP::iterator mit = hist_map.end();
    mit--;
    mit--;
    strade_logic::StockHistInfo &his_info = mit->second;
    real_info.yesterday_close_price = his_info.get_close();
  }

#endif
  //LOG_MSG2("HIHI %.2f", his_info.get_close()); 
  candle_pro.set_latest_info(info, real_info);
  std::string json = candle_pro.GetJson();
  LOG_MSG2("The json %s\n", json.c_str());
  cache.UpdateRealInfoLatestCache(stock_code, json);
  SEND_HTTP_INFO(json);
  return r;
}

}  // realinfo

