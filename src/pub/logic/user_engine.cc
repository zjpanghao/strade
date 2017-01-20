//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2017/1/12 Author: zjc

#include "user_engine.h"

#include <mysql.h>

#include "logic/logic_comm.h"
#include "strade_share/strade_share_engine.h"

using strade_share::SSEngine;

namespace strade_user {

UserEngine::UserEngine() {
}

UserEngine::~UserEngine() {
}

bool UserEngine::Init() {
  SSEngine* engine = GetStradeShareEngine();
  std::vector<MYSQL_ROW> rows;
  if (!engine->ReadDataRows(UserInfo::kGetAllUserInfoSql, rows)) {
    LOG_ERROR("init user info error");
    return false;
  }
  for (size_t i = 0; i < rows.size(); ++i) {
    UserInfo user;
    if (!user.Init(rows[i])) {
      continue;
    }
    user_id_map_.insert(UserIdMap::value_type(user.id(), user));
  }
  LOG_MSG2("init %d user info", rows.size());
  return true;
}

UserInfo* UserEngine::GetUser(UserId id) {
  UserIdMap::iterator it = user_id_map_.find(id);
  if (user_id_map_.end() == it) {
    return NULL;
  }
  return &it->second;
}

} /* namespace strade_user */
