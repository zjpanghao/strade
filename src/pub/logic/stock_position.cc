//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2017/1/10 Author: zjc

#include "stock_position.h"

#include <stdlib.h>
#include <sstream>
#include <algorithm>

#include "strade_share/strade_share_engine.h"

namespace strade_user {

using strade_share::SSEngine;

FakeStockPosition::FakeStockPosition() {
  data_ = new Data();
}

FakeStockPosition::FakeStockPosition(StockPositionId id, OrderInfo* order) {
  data_ = new Data();
  data_->id_ = id;
  data_->count_ = order->deal_num();
  data_->order_ = order;
}

REFCOUNT_DEFINE(FakeStockPosition)

std::string FakeStockPosition::GetFakeStockPositionSql(
                                           UserId user_id,
                                           GroupId group_id,
                                           std::string stock) {
  std::ostringstream oss;
  oss << "SELECT `holdingId`, `userId`, `stock`, `num`, `groupId`, `delegate_id` "
      << "FROM `holding_record` "
      << "WHERE "
      << "`userId` = " << user_id << " AND "
      << "`groupId` = " << group_id << " AND "
      << "`stock` = " << "'" << stock << "'";
  return oss.str();
}

bool FakeStockPosition::Init(MYSQL_ROW row) {
  if (NULL != row[HOLDING_ID]) {
    data_->id_ = atoi(row[HOLDING_ID]);
  }

  if (NULL != row[NUM]) {
    data_->count_ = atoi(row[NUM]);
  }

  if (NULL != row[ORDER_ID]) {
    data_->order_id_ = atoi(row[ORDER_ID]);
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////
GroupStockPosition::GroupStockPosition() {
  data_ = new Data();
}

GroupStockPosition::GroupStockPosition(UserId user_id,
                                       GroupId group_id,
                                       const std::string& code) {
  data_ = new Data();
  data_->user_id_ = user_id;
  data_->group_id_ = group_id;
  data_->code_ = code;
}

REFCOUNT_DEFINE(GroupStockPosition)

std::string GroupStockPosition::GetGroupStockPositionSql(UserId user_id) {
  std::ostringstream oss;
  oss << "SELECT DISTINCT `stock`, `groupId` FROM `holding_record` WHERE "
      << "`userId` = " << user_id << " AND `num` > 0";
  return oss.str();
}

bool GroupStockPosition::Init(MYSQL_ROW row) {
  if (NULL != row[0]) {
    data_->code_ = row[0];
  }

  if (NULL != row[1]) {
    data_->group_id_ = atoi(row[1]);
  }
  return InitFakeStockPosition();
}

bool GroupStockPosition::InitFakeStockPosition() {
  SSEngine* engine = GetStradeShareEngine();
  std::vector<MYSQL_ROW> rows;
  std::string sql =
      FakeStockPosition::GetFakeStockPositionSql(data_->user_id_,
                                                 data_->group_id_,
                                                 data_->code_);
  if (!engine->ReadDataRows(sql, rows)) {
    LOG_ERROR2("init user:%d fake stock position error", data_->user_id_);
    return false;
  }

  for (size_t i = 0; i < rows.size(); ++i) {
    FakeStockPosition p;
    if (!p.Init(rows[i])) {
      continue;
    }
    data_->count_ += p.count();
    data_->fake_stock_position_list_.push_back(p);
  }
  LOG_MSG2("user:%d init %d fake stock position",
           data_->user_id_, rows.size());
  return true;
}
double GroupStockPosition::cost() const {
  double sum = 0.0;
  size_t count = 0;
  for (size_t i = 0; i < data_->fake_stock_position_list_.size(); ++i) {
    count += data_->fake_stock_position_list_[i].count();
    sum += data_->fake_stock_position_list_[i].order()->deal_price() *
        data_->fake_stock_position_list_[i].count();
  }
  return sum / count;
}

bool GroupStockPosition::AddFakeStockPosition(const FakeStockPosition& p) {
  for (size_t i = 0; i < data_->fake_stock_position_list_.size(); ++i) {
    if (p.id() == data_->fake_stock_position_list_[i].id()) {
      return false;
    }
  }
  data_->fake_stock_position_list_.push_back(p);
  data_->count_ += p.count();

  return true;
}

bool GroupStockPosition::Delegate(uint32 n) {
  if (data_->available_ < n) {
    return false;
  }
  data_->available_ -= n;
  return true;
}

void GroupStockPosition::OnOrderCancel(uint32 n) {
  data_->available_ += n;
}

void GroupStockPosition::OnOrderDone(uint32 n,
                                     FakeStockPositionList& fp_list) {
  int32 m = n;
  data_->count_ -= n;
  FakeStockPositionList& fps = data_->fake_stock_position_list_;
  std::sort(fps.begin(), fps.end(), FakeStockPosition::comp);
  for (size_t i = 0; i < fps.size(); ++i) {
    fp_list.push_back(fps[i]);
    m = fps[i].sell(m);
    if (m >= 0) {
      break;
    }
    m = -m;
  }

  // clear
  FakeStockPositionList t;
  for (size_t i = 0; i < fps.size(); ++i) {
    if (fps[i].count()) {
      t.push_back(fps[i]);
    }
  }
  fps.swap(t);
}
///////////////////////////////////////////////////////////////////////////////

StockPosition::StockPosition(const std::string& code, uint32 count) {
  data_ = new Data();
  data_->code_ = code;
  data_->count_ = count;
}

REFCOUNT_DEFINE(StockPosition)

} /* namespace strade_user */
