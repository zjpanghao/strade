//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月17日 Author: kerry

#include "net/comm_head.h"
#include "basic/basictypes.h"
#include  <string>
#include <list>
#include <iostream>
#include "core/common.h"
#include "logic/logic_comm.h"
#include "stock_logic.h"
#include "stock_packet_process.h"

#include "stock_redis_db.h"

#define DEFAULT_CONFIG_PATH     "./plugins/stock/stock_config.xml"

namespace stock_logic {

StockLogic* StockLogic::instance_ = NULL;

StockLogic::StockLogic() {
  if (!Init())
    assert(0);
}

StockLogic::~StockLogic() {
  if (factory_) {
    delete factory_;
    factory_ = NULL;
  }

  if (packet_) {
    delete packet_;
    packet_ = NULL;
  }
}

bool StockLogic::Init() {

  packet_ = new george_logic::http_packet::PacketProcess();
  factory_ = stock_logic::StockFactory::GetInstance();
  config::FileConfig* config = config::FileConfig::GetFileConfig();
  std::string path = DEFAULT_CONFIG_PATH;
  if (config == NULL)
    return false;
  bool r = config->LoadConfig(path);
  if (!r)
    return r;
  factory_->InitParam(config);
  factory_->Notify(0);
  return true;
}

StockLogic*
StockLogic::GetInstance() {
  if (instance_ == NULL)
    instance_ = new StockLogic();
  return instance_;
}

void StockLogic::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

bool StockLogic::OnStockConnect(struct server *srv, const int socket) {
  return true;
}

bool StockLogic::OnStockMessage(struct server *srv, const int socket,
                                const void *msg, const int len) {
  bool r = false;

  if (srv == NULL || socket < 0 || msg == NULL || len < 0)
    return false;

  packet_->UnpackPacket(
      socket, msg, len, george_logic::STO_TYPE,
      stock_logic::http_packet::PacketProcess::PacketPocessGet);

  return true;
}

bool StockLogic::OnStockClose(struct server *srv, const int socket) {
  return true;
}

bool StockLogic::OnBroadcastConnect(struct server *srv, const int socket,
                                    const void *msg, const int len) {
  return true;
}

bool StockLogic::OnBroadcastMessage(struct server *srv, const int socket,
                                    const void *msg, const int len) {
  return true;
}

bool StockLogic::OnBroadcastClose(struct server *srv, const int socket) {
  return true;
}

bool StockLogic::OnIniTimer(struct server *srv) {
  srv->add_time_task(srv, "stock", UPDATE_REALTIME_STOCK_INFO, 45, -1);
  srv->add_time_task(srv, "stock", UPDATE_LIMIT_DATA_TO_DB, 60, -1);
  srv->add_time_task(srv, "stock", DELETE_OLD_LIMIT_DATA, 30 * 60, -1);
  srv->add_time_task(srv, "stock", UPDATE_LIMIT_DATA_TO_MEMORY, 60, -1);
  srv->add_time_task(srv, "stock", UPDATE_INDUSTRY_HIST_DATA, 60, -1);
  srv->add_time_task(srv, "stock", UPDATE_STOCK_HIST_DATA, 60 * 60, -1);
  srv->add_time_task(srv, "stock", UPDATE_STOCK_K_LINE, 24 * 60 * 60, -1);
  srv->add_time_task(srv, "stock", UPDATE_EVENTS_DATA, 60 * 60, -1);
  srv->add_time_task(srv, "stock", UPDATE_YIELD_DATA_TO_DB, 30 * 60, -1);
  srv->add_time_task(srv, "stock", DELETE_OLD_YIELD_DATA, 2 * 60, -1);
  srv->add_time_task(srv, "stock", LOAD_CONSTOM_EVENT, 60, -1);
  srv->add_time_task(srv, "stock", UPDATE_STOCK_OFFLINE_VISIT_DATA, 24 * 60 * 60, -1);

  LOG_DEBUG("add time task success");
  return true;
}

bool StockLogic::OnTimeout(struct server *srv, char *id, int opcode, int time) {
  LOG_MSG2("call OnTimeout opcode=%d",opcode);
  FuncTimeCount test_func_time(opcode);
  switch (opcode) {

    case UPDATE_REALTIME_STOCK_INFO: {
      LOG_MSG("call OnUpdateRealtimeStockInfo");
      if (StockUtil::Instance()->is_trading_time())
        factory_->OnUpdateRealtimeStockInfo();
      break;
    }
    case UPDATE_LIMIT_DATA_TO_DB: {
      factory_->TimeWriteLimitData(time);
      break;
    }
    case DELETE_OLD_LIMIT_DATA: {
      factory_->TimeDeleteOldLimitData();
      break;
    }
    case UPDATE_LIMIT_DATA_TO_MEMORY: {
      factory_->OnUpdateLimitData();
      break;
    }
    case UPDATE_INDUSTRY_HIST_DATA: {
      factory_->TimeUpdateWeekMonthData();
      break;
    }
    case UPDATE_STOCK_HIST_DATA: {
      factory_->OnUpdateStockHistData();
      break;
    }
    case UPDATE_STOCK_K_LINE: {
      factory_->OnUpdateStockKLineData();
      break;
    }
    case UPDATE_EVENTS_DATA: {
      factory_->OnUpdateEventsData();
      break;
    }
    case UPDATE_YIELD_DATA_TO_DB: {
      //factory_->OnUpdateYieldDataToDB();
      break;
    }
    case DELETE_OLD_YIELD_DATA: {
      factory_->OnDeleteOldYieldData();
      break;
    }
    case LOAD_CONSTOM_EVENT: {
      //factory_->OnLoadCustom_Event();
      break;
    }
    case UPDATE_STOCK_OFFLINE_VISIT_DATA: {
      factory_->OnUpdateOfflineVisitData();
      break;
    }
    default:
      break;
  }
  factory_->Notify(opcode);
  return true;
}

}
