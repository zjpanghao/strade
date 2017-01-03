//  Copyright (c) 2015-2015 The geo Authors. All rights reserved.
//  Created on: 2016年5月23日 Author: kerry

#ifndef GEORGE_VIP_VIP_PACKET_PROCESS_H_
#define GEORGE_VIP_VIP_PACKET_PROCESS_H_

#include "net/packet_process.h"
namespace stock_logic {

namespace http_packet {

class PacketProcess {
 public:
  PacketProcess();
  virtual ~PacketProcess() {
  }
  ;
 public:
  static void PacketPocessGet(const int socket,
                              base_logic::DictionaryValue* dict,
                              george_logic::PacketHead* packet);
};

}

}
#endif
