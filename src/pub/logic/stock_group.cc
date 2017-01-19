//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2017/1/10 Author: zjc

#include "stock_group.h"

#include <stdlib.h>
#include <sstream>

#include "logic/logic_comm.h"
#include "strade_share/strade_share_engine.h"

using strade_share::SSEngine;

namespace strade_user {

StockGroup::StockGroup() {
  data_ = new Data();
}

StockGroup::StockGroup(UserId user_id,
                       GroupId id,
                       const std::string& name) {
  data_ = new Data();
  data_->id_ = id;
  data_->user_id_ = user_id;
  data_->name_ = name;
}

REFCOUNT_DEFINE(StockGroup)

std::string StockGroup::GetUserGroupSql(UserId user_id) {
  std::ostringstream oss;
  oss << "SELECT `groupId`, `groupName`, `status`"
      "FROM `group_info` "
      "WHERE "
      << "`userId` = " << user_id << " AND " << "`status` != 0";
  return oss.str();
}

std::string StockGroup::GetGroupStockSql(UserId group_id) {
  std::ostringstream oss;
  oss << "SELECT `stockCode`, `addTime` FROM `group_info` "
      "WHERE `groupId` = " << group_id;
  return oss.str();
}

GroupId StockGroup::CreateGroup(UserId user_id,
                                const std::string& name,
                                Status status) {
  std::ostringstream oss;
  oss << "CALL `proc_CreateStockGroup`("
      << user_id << ","
      << "'" << name << "'" << ","
      << (int)status << ")";

  std::vector<MYSQL_ROW> row;
  SSEngine* engine = GetStradeShareEngine();
  if (!engine->ExcuteStorage(oss.str(), row)) {
    LOG_ERROR2("user:%d create stock group error", user_id);
    return INVALID_GROUPID;
  }

  assert(!row.empty());
  GroupId id = atoi(row[0][0]);
  LOG_MSG2("user:%d create stock group, id:%d, name:%s",
           user_id, id, name.data());
  return id;
}

bool StockGroup::Init(const MYSQL_ROW row) {
  if (NULL != row[0]) {
    data_->id_ = atoi(row[0]);
  }
  if (NULL != row[1]) {
    data_->name_ = row[1];
  }
  if (NULL != row[2]) {
    data_->status_ = (Status)atoi(row[2]);
  }
  return InitStockList();
}

bool StockGroup::InitStockList() {
  SSEngine* engine = GetStradeShareEngine();
  std::vector<MYSQL_ROW> rows;
  if (!engine->ReadDataRows(GetGroupStockSql(data_->id_), rows)) {
    LOG_ERROR("init user info error");
    return false;
  }

  for (size_t i = 0; i < rows.size(); ++i) {
    if (NULL == rows[i]) {
      continue;
    }
    data_->stock_list_.push_back(rows[i][0]);
    data_->stock_set_.insert(rows[i][0]);
  }
  LOG_MSG2("group:%s init %d stocks", data_->name_.data(), rows.size());
  return true;
}

bool StockGroup::AddStocks(StockCodeList& stocks) {
  bool rc = true;
  size_t n = stocks.size();
  StockCodeList s;
  for (size_t i = 0; i < stocks.size(); ++i) {
    if (data_->stock_set_.count(stocks[i])) {
      LOG_ERROR2("stock:%s exist in group:%s",
                 stocks[i].data(), data_->name_.data());
      continue;
    }
    data_->stock_set_.insert(stocks[i]);
    data_->stock_list_.push_back(stocks[i]);
    s.push_back(stocks[i]);
  }

  // update mysql
  std::ostringstream oss;
  oss << "INSERT INTO `group_stock`(`groupId`, `stockCode`) VALUES";
  for (size_t i = 0; i < s.size(); ++i) {
    oss << "(" << data_->id_ << ",'" << s[i] << "'),";
  }
  std::string sql = oss.str();
  sql.erase(sql.size()-1);
  LOG_DEBUG2("add stock list sql: %s", sql.data());

  SSEngine* engine = GetStradeShareEngine();
  if (!engine->WriteData(sql)) {
    rc = false;
    LOG_ERROR2("user:%d add stock to group:%s error",
               data_->user_id_, data_->name_.data());
  }

  stocks.swap(s);
  return rc;
}


bool StockGroup::DelStocks(StockCodeList& stocks) {
  size_t n = stocks.size();
  StockCodeList s;
  for (size_t i = 0; i < stocks.size(); ++i) {
    if (!data_->stock_set_.count(stocks[i])) {
      LOG_ERROR2("stock:%s not exist in group:%s",
                 stocks[i].data(), data_->name_.data());
      continue;
    }
    data_->stock_set_.erase(stocks[i]);
    s.push_back(stocks[i]);
  }

  StockCodeList t;
  for (size_t i = 0; i < data_->stock_list_.size(); ++i) {
    bool find = false;
    for (size_t j = 0; j < s.size(); ++j) {
      if (s[j] == data_->stock_list_[i]) {
        find = true;
      }
    }
    if (!find) {
      t.push_back(data_->stock_list_[i]);
    }
  }

  // update mysql
  // update mysql
  std::ostringstream oss;
  oss << "REPLACE INTO `group_stock`(`groupId`, `stockCode`, `status`) VALUES";
  for (size_t i = 0; i < s.size(); ++i) {
    oss << "(" << data_->id_ << ","
        << "'" << s[i] << "'" << ","
        << int(INVALID) << "),";
  }
  std::string sql = oss.str();
  sql.erase(sql.size()-1);
  LOG_DEBUG2("del stock list sql: %s", sql.data());

  SSEngine* engine = GetStradeShareEngine();
  if (!engine->WriteData(sql)) {
    LOG_ERROR2("user:%d del stock to group:%s error",
               data_->user_id_, data_->name_.data());
  }

  stocks.swap(s);
  data_->stock_list_.swap(t);
  return stocks.size() == n;
}

} /* namespace strade_user */
