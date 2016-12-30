//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2016年5月23日 Author: kerry
#include "operator_code.h"
#include "logic/logic_comm.h"
#include "stock_packet_process.h"

#include "stock_factory.h"
namespace stock_logic {

namespace http_packet {

PacketProcess::PacketProcess() {
}

void PacketProcess::PacketPocessGet(const int socket,
                                    base_logic::DictionaryValue* dict,
                                    george_logic::PacketHead* packet) {
  int16 operate_code = packet->operate_code();
  stock_logic::StockFactory* factory = stock_logic::StockFactory::GetInstance();

  LOG_MSG2("operate_code=%d,type=%d", operate_code, packet->type());
  switch (operate_code) {
    case STOCK_GET_LIMIT_DATA: {
      factory->OnVIPGetLimitData(socket, dict, packet);
      break;
    }

    case STOCK_GET_HOT_DIAGRAM: {
      factory->OnVIPGetHotDiagramData(socket, dict, packet);
      break;
    }
    default:
      break;
  }
}

}

}
