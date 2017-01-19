//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2017/1/10 Author: zjc

#include "order_info.h"

#include <stdlib.h>

#include <vector>
#include <sstream>

#include "message.h"
#include "comm_head.h"
#include "logic/logic_comm.h"
#include "strade_share/strade_share_engine.h"
#include "logic/strade_basic_info.h"
#include "logic/user_engine.h"

namespace strade_user {

using strade_share::SSEngine;
using strade_share::STOCKS_MAP;
using strade_share::STOCK_REAL_MAP;
using strade_logic::StockTotalInfo;

OrderInfo::OrderInfo() {
  data_ = new Data();
}

OrderInfo::OrderInfo(UserId user_id,
                     OrderId order_id,
                     OrderType type) {
  data_ = new Data();
  data_->user_id_ = user_id;
  data_->id_ = order_id;
  data_->type_ = type;
}

REFCOUNT_DEFINE(OrderInfo)

std::string OrderInfo::GetUserOrderSql(UserId user_id) {
  std::ostringstream oss;
  oss << "SELECT `id`, `userId`, `groupId`, `stock`, `price`, "
      << "`lossOrProfitPrice`, `tradeType`, `status`, `count`, "
      << "`needFunds`, `tradeTime`, `tradePrice`, `tradeCount`, "
      << "`stampDuty`, `commission`, `transferFee`, `type`, "
      << "`amount`, `profit`, `availableCapital` "
      << "FROM `delegation_record`"
      << "WHERE "
      << "`userId` = " << user_id << " AND `status` = 0";
  return oss.str();
}

std::string OrderInfo::GetPendingOrderSql(UserId user_id) {
  return std::string("");
}

std::string OrderInfo::GetFinishedOrderSql(UserId user_id) {
  return std::string("");
}

void OrderInfo::Init(const SubmitOrderReq& req) {
  data_->group_id_ = req.group_id;
  data_->code_ = req.code;
  data_->op_ = req.op;
  data_->order_price_ = req.order_price;
  data_->order_num_ = req.order_nums;
  data_->expected_price_ = req.expected_price;
}

void OrderInfo::Init(const OrderInfo& order) {
  data_->group_id_ = order.group_id();
  data_->code_ = order.code();
  data_->op_ = SELL;
  data_->order_price_ = order.expected_price();
  data_->order_num_ = order.deal_num();
}

bool OrderInfo::Init(MYSQL_ROW row) {
  if (NULL != row[ID]) {
    data_->id_ = atoi(row[ID]);
  }

  if (NULL != row[USER_ID]) {
    data_->user_id_ = atoi(row[USER_ID]);
  }

  if (NULL != row[GROUP_ID]) {
    data_->group_id_ = atoi(row[GROUP_ID]);
  }

  if (NULL != row[STOCK]) {
    data_->code_ = row[STOCK];
  }

  if (NULL != row[ORDER_PRICE]) {
    data_->order_price_ = atof(row[ORDER_PRICE]);
  }

  if (NULL != row[EXPECTED_PRICE]) {
    data_->expected_price_ = atof(row[EXPECTED_PRICE]);
  }

  if (NULL != row[ORDER_OPERATION]) {
    data_->op_ = (OrderOperation)atoi(row[ORDER_OPERATION]);
  }

  if (NULL != row[ORDER_STATUS]) {
    data_->status_ = (OrderStatus)atoi(row[ORDER_STATUS]);
  }

  if (NULL != row[ORDER_COUNT]) {
    data_->order_num_ = atoi(row[ORDER_COUNT]);
  }

  if (NULL != row[FROZEN]) {
    data_->frozen_ = atof(row[FROZEN]);
  }

  if (NULL != row[ORDER_TYPE]) {
    data_->type_  = (OrderType)atoi(row[ORDER_TYPE]);
  }

  if (FINISHED != data_->status_) {
    return true;
  }

  if (NULL != row[DEAL_TIME]) {
    data_->deal_time_ = atoi(row[DEAL_TIME]);
  }

  if (NULL != row[DEAL_PRICE]) {
    data_->deal_price_ = atof(row[DEAL_PRICE]);
  }

  if (NULL != row[DEAL_COUNT]) {
    data_->deal_num_ = atoi(row[DEAL_COUNT]);
  }

  if (NULL != row[STAMP_DUTY]) {
    data_->stamp_duty_ = atof(row[STAMP_DUTY]);
  }

  if (NULL != row[COMMISSION]) {
    data_->commission_ = atof(row[COMMISSION]);
  }

  if (NULL != row[TRANSFER_FEE]) {
    data_->transfer_fee_ = atof(row[TRANSFER_FEE]);
  }

  if (NULL != row[AMOUNT]) {
    data_->amount_ = atof(row[AMOUNT]);
  }

  if (NULL != row[PROFIT]) {
    data_->profit_ = atof(row[PROFIT]);
  }

  if (NULL != row[AVAILABLE_CAPITAL]) {
    data_->available_capital_ = atof(row[AVAILABLE_CAPITAL]);
  }
  return true;
}

bool OrderInfo::InitPendingOrder(MYSQL_ROW row) {
  return true;
}

bool OrderInfo::InitFinishedOrder(MYSQL_ROW row) {
  return true;
}

void OrderInfo::Update(int opcode) {
  switch (opcode) {
    case strade_logic::REALTIME_MARKET_VALUE_UPDATE:
      OnStockUpdate();
      break;
    default:
      break;
  }
}

bool OrderInfo::MakeADeal(double price) {
  if (!can_deal(price)) {
    return false;
  }

  data_->status_ = FINISHED;
  data_->deal_time_ = time(NULL);
  data_->deal_price_ = price;
  data_->deal_num_ = data_->order_num_;

  double amount = data_->deal_price_ * data_->deal_num_;
  double commission = data_->amount_ * COMMISSION_RATE;
  data_->commission_ = ROUND_COMMISSION(commission);
  data_->amount_ += data_->commission_;

  if (IS_SH_CODE(data_->code_)) {
    data_->transfer_fee_ = TRANSFER_FEE(data_->deal_num_);
  }
  data_->amount_ += data_->transfer_fee_;

  if (SELL == data_->op_) {
    data_->stamp_duty_ = amount * STAMP_DUTY_RATE;
  }

  data_->amount_ = amount +
      data_->commission_ +
      data_->transfer_fee_ +
      data_->stamp_duty_;

  UserEngine* engine = UserEngine::GetUserEngine();
  UserInfo* user = engine->GetUser(data_->user_id_);
  assert(NULL != user);
  user->OnOrderDone(this);

  return true;
}

void OrderInfo::OnStockUpdate() {
  if (FINISHED == data_->op_) {
    LOG_ERROR2("fatal error: finished order, user_id:%d, group_id:%d, "
        "order_id:%d, code:%s, num:%d, create_time:%d, deal_time:%d",
        data_->user_id_, data_->group_id_, data_->id_, data_->code_.data(),
        data_->order_num_, data_->create_time_, data_->deal_time_);
    return ;
  }

  SSEngine* engine = GetStradeShareEngine();
  STOCKS_MAP stocks = engine->GetAllStockTotalMapCopy();
  STOCKS_MAP::iterator it = stocks.find(data_->code_);
  if (stocks.end() != it) {
    LOG_ERROR2("stock:%s NOT EXIST", data_->code_.data());
    return ;
  }

  STOCK_REAL_MAP stock = it->second.GetStockRealMap();
  STOCK_REAL_MAP::reverse_iterator deal_it = stock.rend();
  for (STOCK_REAL_MAP::reverse_iterator rit = stock.rbegin();
      stock.rend() != rit; ++rit) {
    if (rit->first >= data_->create_time_
        && can_deal(rit->second.price())) {
      deal_it = rit;
    }
  }

  // cannot deal
  if (stock.rend() == deal_it) {
    return ;
  }

  // now can remove from observer list
  stale_ = true;

  // can make a deal
  MakeADeal(deal_it->second.price());
}

void OrderInfo::OnOrderCancel() {
  data_->status_ = CANCEL;
  stale_ = true;
}

} /* namespace strade_user */
