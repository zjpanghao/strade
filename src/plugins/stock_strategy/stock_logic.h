//  Copyright (c) 2016-2016 The george Authors. All rights reserved.
//  Created on: 2016年5月17日 Author: kerry

#ifndef GEORGE_VIP_LOGIC_
#define GEORGE_VIP_LOGIC_
#include "core/common.h"
#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"
#include "stock_factory.h"
#include "stock_packet_process.h"
#include "stock_user_mgr.h"

#define STOCK_INIT_SUCCESS                   0
#define UPDATE_REALTIME_STOCK_INFO           55000
#define UPDATE_LIMIT_DATA_TO_DB              55001
#define UPDATE_LIMIT_DATA_TO_MEMORY          55002
#define UPDATE_INDUSTRY_HIST_DATA            55003
#define UPDATE_STOCK_HIST_DATA               55004
#define DELETE_OLD_LIMIT_DATA                55005
#define UPDATE_STOCK_K_LINE                  55006
#define UPDATE_EVENTS_DATA                   55007
#define UPDATE_YIELD_DATA_TO_DB              55008
#define DELETE_OLD_YIELD_DATA                55009
#define LOAD_CONSTOM_EVENT                   55010
#define UPDATE_STOCK_OFFLINE_VISIT_DATA      55011

namespace stock_logic {

class StockLogic {
 public:
  StockLogic();
  virtual ~StockLogic();

 private:
  static StockLogic *instance_;

 public:
  static StockLogic *GetInstance();
  static void FreeInstance();

 public:

  bool OnStockConnect(struct server *srv, const int socket);

  bool OnStockMessage(struct server *srv, const int socket, const void *msg,
                      const int len);

  bool OnStockClose(struct server *srv, const int socket);

  bool OnBroadcastConnect(struct server *srv, const int socket,
                          const void *data, const int len);

  bool OnBroadcastMessage(struct server *srv, const int socket, const void *msg,
                          const int len);

  bool OnBroadcastClose(struct server *srv, const int socket);

  bool OnIniTimer(struct server *srv);

  bool OnTimeout(struct server *srv, char* id, int opcode, int time);
 private:
  bool Init();
 private:
  stock_logic::StockFactory* factory_;
  george_logic::http_packet::PacketProcess* packet_;

};

}  // namespace vip_logic

#endif

