//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2017/1/9 Author: zjc

#include "user_logic.h"

#include <sstream>
#include <limits>

#include "basic/scoped_ptr.h"
#include "logic/base_values.h"
#include "logic/logic_comm.h"
#include "logic/message.h"
#include "strade_share/strade_share_engine.h"
#include "logic/comm_head.h"
#include "logic/logic_unit.h"
#include "logic/stock_util.h"


#define DEFAULT_CONFIG_PATH     "./plugins/user/user_config.xml"
#define SHARE_LIB_PATH          "./strade_share/strade_share.so"
#define SHARE_LIB_HANDLER       "GetStradeShareEngine"

using base_logic::ValueSerializer;


namespace strade_user {

using stock_logic::StockUtil;
using strade_share::SSEngine;
using strade_logic::StockTotalInfo;

SSEngine* engine = NULL;

UserLogic *UserLogic::instance_ = NULL;

UserLogic* UserLogic::GetInstance() {
  if (NULL == instance_)
    instance_ = new UserLogic();
  return instance_;
}

void UserLogic::FreeInstance() {
  delete instance_;
  instance_ = NULL;
}

UserLogic::UserLogic() {
  SSEngine* (* handler)(void);
  handler = (strade_share::SSEngine* (*)(void))
      base_logic::SomeUtils::GetLibraryFunction(
          SHARE_LIB_PATH, SHARE_LIB_HANDLER);
  engine = (*handler)();
  if (engine == NULL)
    assert(0);
  engine->Init();
  LOG_DEBUG2("engine: %p", engine);
}

UserLogic::~UserLogic() {
}

bool UserLogic::OnUserConnect(struct server *srv, const int socket) {
  return true;
}


DictionaryValue* UserLogic::HttpDeserialize(const char* msg, int len) {
  if (NULL == msg || len <= 0) {
    return NULL;
  }

  if ('\n' != msg[len-1]) {
    LOG_ERROR("error msg");
    return NULL;
  }

  int error_code = 0;
  std::string error_str;
  std::string data(msg, len);

  scoped_ptr<ValueSerializer> engine(
      ValueSerializer::Create(base_logic::IMPL_HTTP, &data));

  Value* value = engine->Deserialize(&error_code, &error_str);
  if (0 != error_code || NULL == value) {
    LOG_ERROR2("deserialize error, err_code: %d, err_msg: %s",
               error_code, error_str.c_str());
    return NULL;
  }

  return (DictionaryValue*) value;
}

bool UserLogic::SendResponse(int socket, Status& status) {
  DictionaryValue dict;
  if (!status.Serialize(dict)) {
    LOG_ERROR("serialize status msg error");
    return false;
  }

  std::string json_msg;
  scoped_ptr<ValueSerializer> engine(
      ValueSerializer::Create(base_logic::IMPL_JSON, &json_msg));
  engine->Serialize(dict);
  LOG_DEBUG2("status:\n%s, size:%d", json_msg.data(), json_msg.size());
  send_message_by_size(socket, json_msg);
  return true;
}

bool UserLogic::SendResponse(int socket, ResHead& msg) {
  DictionaryValue dict;
  if (!msg.StartSerialize(dict)) {
    LOG_ERROR("serialize response msg error");
    return false;
  }

  std::string json_msg;
  scoped_ptr<ValueSerializer> engine(
      ValueSerializer::Create(base_logic::IMPL_JSON, &json_msg));
  engine->Serialize(dict);
  LOG_DEBUG2("json_msg:\n%s, size:%d", json_msg.data(), json_msg.size());
  send_message_by_size(socket, json_msg);
  return true;
}

bool UserLogic::OnUserMessage(struct server *srv, const int socket,
                                const void *msg, const int len) {
  std::string data((const char*)msg, len);
  LOG_DEBUG2("recv msg:\n%s", data.c_str());

  scoped_ptr<DictionaryValue> dict(HttpDeserialize((const char*)msg, len));
  if (dict == NULL) {
    return false;
  }

  ReqHead head;
  bool rc = head.Deserialize(*dict);

  if (USER_API != head.type) {
    return false;
  }

  if (!rc) {
    Status status = Status::ERROR_MSG;
    SendResponse(socket, status);
    return false;
  }

  if (NULL == engine->GetUser(head.user_id)) {
    Status status = Status::USER_NOT_EXIST;
    SendResponse(socket, status);
    return false;
  }

  switch (head.opcode) {
    case CreateGroupReq::ID:
      OnCreateGroup(socket, *dict);
      break;
    case AddStockReq::ID:
      OnAddStock(socket, *dict);
      break;
    case DelStockReq::ID:
      OnDelStock(socket, *dict);
      break;
    case QueryGroupReq::ID:
      OnQueryGroup(socket, *dict);
      break;
    case QueryStocksReq::ID:
      OnQueryStock(socket, *dict);
      break;
    case GroupStockHoldingReq::ID:
      OnQueryGroupHoldingStock(socket, *dict);
      break;
    case QueryHoldingStocksReq::ID:
      OnQueryHoldingStock(socket, *dict);
      break;
    case QueryTodayOrdersReq::ID:
      OnQueryTodayOrder(socket, *dict);
      break;
    case QueryTodayFinishedOrdersReq::ID:
      OnQueryTodayFinishedOrder(socket, *dict);
      break;
    case QueryHistoryFinishedOrdersReq::ID:
      OnQueryHistoryFinishedOrder(socket, *dict);
      break;
    case QueryStatementReq::ID:
      OnQueryStatement(socket, *dict);
      break;
    case SubmitOrderReq::ID:
      OnSubmitOrder(socket, *dict);
      break;
    case AvailableStockCountReq::ID:
      OnAvailableStockCount(socket, *dict);
      break;
    case CancelOrderReq::ID:
      OnCancelOrder(socket, *dict);
      break;
    case ProfitAndLossOrderNumReq::ID:
      OnProfitAndLossOrderNum(socket, *dict);
      break;
    default:
      Status status = Status::UNKNOWN_OPCODE;
      SendResponse(socket, status);
      break;
  }
  return true;
}

bool UserLogic::OnUserClose(struct server *srv, const int socket) {
  return true;
}

bool UserLogic::OnBroadcastConnect(struct server *srv, const int socket,
                                    const void *msg, const int len) {
  return true;
}

bool UserLogic::OnBroadcastMessage(struct server *srv, const int socket,
                                    const void *msg, const int len) {
  return true;
}

bool UserLogic::OnBroadcastClose(struct server *srv, const int socket) {
  return true;
}

bool UserLogic::OnIniTimer(struct server *srv) {
  return true;
}

bool UserLogic::OnTimeout(struct server *srv, char *id, int opcode, int time) {
  return true;
}

void UserLogic::OnCreateGroup(int socket, DictionaryValue& dict) {
  CreateGroupReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  CreateGroupRes res;
  UserInfo* user = engine->GetUser(msg.user_id);
  GroupId gid = 0;
  res.status.state =
      user->CreateGroup(msg.group_name, msg.code_list, &gid);
  res.group_id = gid;
  SendResponse(socket, res);
}

void UserLogic::OnAddStock(int socket, DictionaryValue& dict) {
  AddStockReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  UserInfo* user = engine->GetUser(msg.user_id);
  Status status = user->AddStock(msg.group_id, msg.code_list);

  SendResponse(socket, status);
}

void UserLogic::OnDelStock(int socket, DictionaryValue& dict) {
  DelStockReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  UserInfo* user = engine->GetUser(msg.user_id);
  Status status = user->DelStock(msg.group_id, msg.code_list);
  SendResponse(socket, status);
}

void UserLogic::OnQueryGroup(int socket, DictionaryValue& dict) {
  QueryGroupReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  QueryGroupsRes res;
  UserInfo* user = engine->GetUser(msg.user_id);
  StockGroupList gs = user->GetAllGroups();
  for (size_t i = 0; i < gs.size(); ++i) {
    QueryGroupsRes::GroupInfo g;
    g.id = gs[i].id();
    g.name = gs[i].name();
    res.group_list.push_back(g);
  }
  SendResponse(socket, res);
}

void UserLogic::OnQueryStock(int socket, DictionaryValue& dict) {
  QueryStocksReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  QueryStocksRes res;
  StockTotalInfo info;
  StockCodeList stocks;
  UserInfo* user = engine->GetUser(msg.user_id);
  res.status.state = user->GetGroupStock(msg.group_id, stocks);
  for (size_t i = 0; i < stocks.size(); ++i) {
    QueryStocksRes::StockInfo s;
    s.code = stocks[i];

    if (!engine->GetStockTotalInfoByCode(s.code, info)) {
      LOG_ERROR2("NOT FIND stock:%s in share", s.code.data());
      continue;
    }
    // TODO:
//    s.visit_heat =;
//    s.price = 11.53;
//    s.change = ;
//    s.volume = 6868;
//    s.industry = "房地产开发";
    res.stock_list.push_back(s);
  }
  SendResponse(socket, res);
}

void UserLogic::OnQueryGroupHoldingStock(int socket, DictionaryValue& dict) {
  GroupStockHoldingReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  StockTotalInfo info;
  GroupStockHoldingRes res;
  UserInfo* user = engine->GetUser(msg.user_id);
  GroupStockPositionList l = user->GetAllGroupStockPosition();
  for (size_t i = 0; i < l.size(); ++i) {
    GroupStockHoldingRes::StockInfo s;
    s.code = l[i].code();
    if (!engine->GetStockTotalInfoByCode(s.code, info)) {
      LOG_ERROR2("NOT FIND stock:%s in share", s.code.data());
      continue;
    }
    // TODO:
//    s.name =
    s.holding = l[i].count();
    res.stock_list.push_back(s);
  }
  SendResponse(socket, res);
}

void UserLogic::OnQueryHoldingStock(int socket, DictionaryValue& dict) {
  QueryHoldingStocksReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  StockTotalInfo info;
  QueryHoldingStocksRes res;
  UserInfo* user = engine->GetUser(msg.user_id);
  GroupStockPositionList l = user->GetAllGroupStockPosition();
  for (size_t i = 0; i < l.size(); ++i) {
    QueryHoldingStocksRes::StockInfo s;
    s.code = l[i].code();
    s.holding = l[i].count();
    s.available = l[i].available();
    s.cost = l[i].cost();
    // TODO
//    s.price = 11.53;
//    s.market_value = 7678.98;
    s.profit = -82.68;
    s.profit_ratio = -2.68;
    s.position = 6.8;
    res.stock_list.push_back(s);
  }
  SendResponse(socket, res);
}

void UserLogic::OnQueryTodayOrder(int socket, DictionaryValue& dict) {
  QueryTodayOrdersReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  StockTotalInfo info;
  QueryTodayOrdersRes res;
  StockUtil *util = StockUtil::Instance();
  UserInfo* user = engine->GetUser(msg.user_id);
  OrderFilterList filters;
  time_t begin_time = util->to_timestamp(0);
  time_t end_time = util->to_timestamp(235959);
  filters.push_back(new OrderCreateTimeFilter(begin_time, end_time));
  OrderList orders = user->FindOrders(filters);
  for (size_t i = 0; i < orders.size(); ++i) {
    QueryTodayOrdersRes::OrderInfo o;
    o.id = orders[i].id();
    o.code = orders[i].code();
    if (!engine->GetStockTotalInfoByCode(o.code, info)) {
      LOG_ERROR2("NOT FIND stock:%s in share", o.code.data());
      continue;
    }
    // TODO:
//    o.name =
    o.op = orders[i].operation();
    o.order_price = orders[i].order_price();
    o.order_nums = orders[i].order_num();
    o.order_time = orders[i].craete_time();
    o.status = orders[i].status();
    res.order_list.push_back(o);
  }
  SendResponse(socket, res);
}
void UserLogic::OnQueryTodayFinishedOrder(int socket, DictionaryValue& dict) {
  QueryTodayFinishedOrdersReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  QueryTodayFinishedOrdersRes res;
  StockUtil *util = StockUtil::Instance();
  UserInfo* user = engine->GetUser(msg.user_id);
  OrderFilterList filters;
  time_t begin_time = util->to_timestamp(0);
  time_t end_time = util->to_timestamp(235959);
  filters.push_back(new OrderCreateTimeFilter(begin_time, end_time));
  filters.push_back(new OrderStatusFilter(FINISHED));
  OrderList orders = user->FindOrders(filters);
  for (size_t i = 0; i < orders.size(); ++i) {
    QueryTodayFinishedOrdersRes::OrderInfo o;
    o.code = orders[i].code();
    o.op = orders[i].operation();
    o.order_price = orders[i].deal_price();
    o.order_nums = orders[i].deal_num();
    o.order_time = orders[i].deal_time();
    o.amount = orders[i].amount();
    res.order_list.push_back(o);
  }
  SendResponse(socket, res);
}

void UserLogic::OnQueryHistoryFinishedOrder(int socket, DictionaryValue& dict) {
  QueryHistoryFinishedOrdersReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  QueryHistoryFinishedOrdersRes res;
  StockUtil *util = StockUtil::Instance();
  UserInfo* user = engine->GetUser(msg.user_id);
  OrderFilterList filters;
  time_t b = util->to_timestamp(msg.begin_time);
  time_t e = util->to_timestamp(msg.end_time);
  filters.push_back(new OrderDealTimeFilter(b, e));
  filters.push_back(new OrderStatusFilter(FINISHED));
  OrderList orders = user->FindOrders(filters);
  for (size_t i = 0; i < filters.size(); ++i) {
    delete filters[i];
  }

  for (size_t i = 0; i < orders.size(); ++i) {
    QueryHistoryFinishedOrdersRes::OrderInfo o;
    o.code = orders[i].code();
    o.op = orders[i].operation();
    o.order_price = orders[i].deal_price();
    o.order_nums = orders[i].deal_num();
    o.order_time = orders[i].deal_time();
    o.amount = orders[i].amount();
    res.order_list.push_back(o);
  }
  SendResponse(socket, res);
}
void UserLogic::OnQueryStatement(int socket, DictionaryValue& dict) {
  QueryStatementReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  QueryStatementRes res;
  StockUtil *util = StockUtil::Instance();
  UserInfo* user = engine->GetUser(msg.user_id);
  OrderFilterList filters;
  time_t b = util->to_timestamp(msg.begin_time);
  time_t e = util->to_timestamp(msg.end_time);
  filters.push_back(new OrderDealTimeFilter(b, e));
  filters.push_back(new OrderStatusFilter(FINISHED));
  OrderList orders = user->FindOrders(filters);
  for (size_t i = 0; i < filters.size(); ++i) {
    delete filters[i];
  }

  for (size_t i = 0; i < orders.size(); ++i) {
    QueryStatementRes::StatementRecord s;
    s.code = orders[i].code();
    s.op = orders[i].operation();
    s.order_price = orders[i].deal_price();
    s.order_nums = orders[i].deal_num();
    s.commission = orders[i].commission();
    s.stamp_duty = orders[i].stamp_duty();
    s.transfer_fee = orders[i].transfer_fee();
    s.amount = orders[i].amount();
    s.available_capital = orders[i].available_capital();
    res.statement_list.push_back(s);
  }
  SendResponse(socket, res);
}

void UserLogic::OnSubmitOrder(int socket, DictionaryValue& dict) {
  SubmitOrderReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  SubmitOrderRes res;
  UserInfo* user = engine->GetUser(msg.user_id);
  res.status.state = user->SubmitOrder(msg);

  SendResponse(socket, res);
}

void UserLogic::OnAvailableStockCount(int socket, DictionaryValue& dict) {
  AvailableStockCountReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  StockTotalInfo info;
  AvailableStockCountRes res;
  if (!engine->GetStockTotalInfoByCode(msg.code, info)) {
    LOG_ERROR2("NOT FIND stock:%s in share", msg.code.data());
    res.status.state = Status::STOCK_NOT_EXIST;
    SendResponse(socket, res);
    return ;
  }

  UserInfo* user = engine->GetUser(msg.user_id);
  size_t count = user->available_capital();
  res.code = msg.code;
  // TODO:
//  res.name =
//  res.count = 10000;

  SendResponse(socket, res);
}

void UserLogic::OnCancelOrder(int socket, DictionaryValue& dict) {
  CancelOrderReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  UserInfo* user = engine->GetUser(msg.user_id);
  Status status = user->OnCancelOrder(msg.order_id);
  SendResponse(socket, status);
}

void UserLogic::OnProfitAndLossOrderNum(int socket, DictionaryValue& dict) {
  ProfitAndLossOrderNumReq msg;
  if (!msg.StartDeserialize(dict)) {
    return ;
  }

  std::ostringstream oss;
  msg.StartDump(oss);
  LOG_DEBUG2("%s", oss.str().data());

  ProfitAndLossOrderNumRes res;
  UserInfo* user = engine->GetUser(msg.user_id);
  OrderFilterList filters;

  double min = 0.1;
  double max = std::numeric_limits<double>::max();
  filters.push_back(new OrderProfitFilter(min, max));
  OrderList orders = user->FindOrders(filters);
  res.profit_num = orders.size();
  delete filters[0];
  filters.clear();

  min = std::numeric_limits<double>::min();
  max = 0.0;
  filters.push_back(new OrderProfitFilter(min, max));
  orders = user->FindOrders(filters);
  res.loss_num = orders.size();
  delete filters[0];

  SendResponse(socket, res);
}

} /* namespace strade_user */
