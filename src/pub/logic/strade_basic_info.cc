//
// Created by Harvey on 2017/1/7.
//

#include "strade_basic_info.h"

namespace strade_logic {

StockRealInfo::StockRealInfo() {
  data_ = new Data();
}

StockRealInfo::StockRealInfo(const StockRealInfo& rhs)
    : data_(rhs.data_) {
  if (NULL != data_) {
    data_->AddRef();
  }
}

StockRealInfo& StockRealInfo::operator=(const StockRealInfo& rhs) {
  if (this == &rhs) {
    return (*this);
  }
  if (NULL == rhs.data_) {
    rhs.data_->AddRef();
  }
  if (NULL != data_) {
    data_->AddRef();
  }
  data_ = rhs.data_;
  return (*this);
}

StockRealInfo::~StockRealInfo() {
  if (NULL != data_) {
    data_->Release();
  }
}

void StockRealInfo::Deserialize(base_logic::DictionaryValue& dict) {
  dict.GetReal(L"changepercent", &data_->change_percent_);
  dict.GetReal(L"trade", &data_->trade_);
  dict.GetReal(L"open", &data_->open_);
  dict.GetReal(L"high", &data_->high_);
  dict.GetReal(L"low", &data_->low_);
  dict.GetReal(L"settlement", &data_->settlement_);
  dict.GetBigInteger(L"time", &data_->trade_time_);
  dict.GetReal(L"volume", &data_->volume_);
}

StockHistInfo::StockHistInfo() {
  data_ = new Data();
}

StockHistInfo::StockHistInfo(const StockHistInfo& rhs)
    : data_(rhs.data_) {
  if (NULL != data_) {
    data_->AddRef();
  }
}

StockHistInfo& StockHistInfo::operator=(const StockHistInfo& rhs) {
  if (this == &rhs) {
    return (*this);
  }
  if (NULL != rhs.data_) {
    rhs.data_->AddRef();
  }
  if (NULL != data_) {
    data_->Release();
  }
  data_ = rhs.data_;
  return (*this);
}

StockHistInfo::~StockHistInfo() {
  if (NULL != data_) {
    data_->Release();
  }
}

void StockHistInfo::Deserialize(base_logic::DictionaryValue& dict) {
  double qfq_close = 0.0;
  dict.GetString(L"date", &data_->date_);
  dict.GetReal(L"open", &data_->open_);
  dict.GetReal(L"high", &data_->high_);
  dict.GetReal(L"close", &data_->close_);
  dict.GetReal(L"low", &data_->low_);
  dict.GetReal(L"qfq_close", &qfq_close);
  if (qfq_close > 1) {
    data_->qfq_close_ = qfq_close;
  } else {
    data_->qfq_close_ = data_->close_;
  }
  data_->mid_price_ = (data_->high_ + data_->low_ + data_->close_) / 3;
}

StockBasicInfo::StockBasicInfo() {
  data_ = new Data();
}

StockBasicInfo::StockBasicInfo(const StockBasicInfo& rhs)
    : data_(rhs.data_) {
  if (NULL != data_) {
    data_->AddRef();
  }
}

StockBasicInfo& StockBasicInfo::operator=(const StockBasicInfo& rhs) {
  if (this == &rhs) {
    return (*this);
  }
  if (NULL != rhs.data_) {
    rhs.data_->AddRef();
  }
  if (NULL == data_) {
    data_->Release();
  }
  data_ = rhs.data_;
  return (*this);
}

StockBasicInfo::~StockBasicInfo() {
  if (NULL != data_) {
    data_->Release();
  }
}

void StockBasicInfo::Deserialize(base_logic::DictionaryValue& dict) {
  dict.GetString(L"code", &data_->code_);
  dict.GetString(L"name", &data_->name_);
  dict.GetReal(L"totalAssets", &data_->outstanding_);
  dict.GetReal(L"bvps", &data_->bvps_);
  dict.GetReal(L"pb", &data_->pb_);
  data_->market_value_ = data_->outstanding_ * data_->bvps_ * data_->pb_;
}

StockTotalInfo::StockTotalInfo() {
  data_ = new Data();
}

StockTotalInfo::StockTotalInfo(const StockTotalInfo& rhs)
    : data_(rhs.data_) {
  if (NULL != data_) {
    data_->AddRef();
  }
}

StockTotalInfo& StockTotalInfo::operator=(const StockTotalInfo& rhs) {
  if (this == &rhs) {
    return (*this);
  }
  if (NULL != rhs.data_) {
    rhs.data_->AddRef();
  }
  if (NULL != data_) {
    data_->Release();
  }
  data_ = rhs.data_;
  return (*this);
}

StockTotalInfo::~StockTotalInfo() {
  if (NULL != data_) {
    data_->Release();
  }
}

void StockTotalInfo::DeserializeStockBasicInfo(
    base_logic::DictionaryValue& dict) {
  dict.GetString(L"code", &data_->code_);
  data_->stock_basic_info_.Deserialize(dict);
  LOG_DEBUG2("load stock_code=%s", data_->code_.c_str());
}

void StockTotalInfo::ClearRealMap() {
  data_->stock_real_map_.clear();
}

const STOCK_HIST_MAP& StockTotalInfo::GetStockHistMap() const {
  return data_->stock_hist_map_;
}

const STOCK_REAL_MAP& StockTotalInfo::GetStockRealMap() const {
  return data_->stock_real_map_;
}

bool StockTotalInfo::AddStockHistInfoByDate(
    const std::string& date, const StockHistInfo& stock_hist_info) {
//  LOG_DEBUG2("AddStockHistInfoByDate stock_code=%s, date=%s",
//             (data_->code_).c_str(), date.c_str());
  STOCK_HIST_MAP::iterator iter(data_->stock_hist_map_.find(date));
  if (iter == data_->stock_hist_map_.end()) {
    ReplaceStockHistInfo(date, stock_hist_info);
    return true;
  }
  return false;
}

bool StockTotalInfo::AddStockHistVec(
    std::vector<StockHistInfo>& stock_hist_vec) {
  std::vector<StockHistInfo>::iterator iter(stock_hist_vec.begin());
  for (; iter != stock_hist_vec.end(); ++iter) {
    const std::string& date = (*iter).GetHistDate();
    ReplaceStockHistInfo(date, (*iter));
  }
  return true;
}

bool StockTotalInfo::GetStockHistInfoByDate(
    const std::string& date, StockHistInfo** stock_hist_info) {
  STOCK_HIST_MAP::iterator iter(data_->stock_hist_map_.find(date));
  if (iter != data_->stock_hist_map_.end()) {
    *stock_hist_info = &(iter->second);
    return true;
  }
  return false;
}

bool StockTotalInfo::ReplaceStockHistInfo(
    const std::string& date, const StockHistInfo& stock_hist_info) {
  data_->stock_hist_map_[date] = stock_hist_info;
  return true;
}

bool StockTotalInfo::AddStockRealInfoByTime(
    const time_t trade_time, const StockRealInfo& stock_real_info) {
  STOCK_REAL_MAP::iterator iter(data_->stock_real_map_.find(trade_time));
  if (iter == data_->stock_real_map_.end()) {
    ReplaceStockRealInfo(trade_time, stock_real_info);
    return true;
  }
  return false;
}

bool StockTotalInfo::GetStockRealInfoByTradeTime(
    const time_t trade_time, StockRealInfo** stock_real_info) {
  STOCK_REAL_MAP::iterator iter(data_->stock_real_map_.find(trade_time));
  if (iter != data_->stock_real_map_.end()) {
    *stock_real_info = &(iter->second);
    return true;
  }
  return false;
}

bool StockTotalInfo::ReplaceStockRealInfo(
    const time_t trade_time, const StockRealInfo& stock_real_info) {
  data_->stock_real_map_[trade_time] = stock_real_info;
  return true;
}

} /* namespace strade_logic */