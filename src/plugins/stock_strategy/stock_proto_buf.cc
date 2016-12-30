//  Copyright (c) 2015-2015 The GEORGE Authors. All rights reserved.
//  Created on: 2016年5月25日 Author: kerry

#include "stock_proto_buf.h"

namespace stock_logic {

namespace net_request {

void VIPNews::set_http_packet(base_logic::DictionaryValue* value) {
  bool r = false;
  int64 uid = 0;
  int32 pos = 0;
  int32 count = 10;
  std::string token;

  r = value->GetBigInteger(L"uid", &uid);
  if (r)
    set_uid(uid);

  r = value->GetString(L"token", &token);
  if (r)
    set_token(token);

  r = value->GetInteger(L"count", &count);
  if (r)
    set_count(count);

  r = value->GetInteger(L"pos", &pos);
  if (r)
    set_pos(pos);
}
}
}
