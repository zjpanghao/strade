/*
 * StockholderInfo.cpp
 *
 *  Created on: 2016年8月25日
 *      Author: Maibenben
 */

#include "StockholderInfo.h"
#include "logic/logic_comm.h"

namespace stock_logic {

StockholderPerQuarter::StockholderPerQuarter() {

  date_ = "";
  holder_count_ = 0;
  compare_change_ = 0;
  float_stock_num_ = 0;
  top_float_stock_num_ = 0;
  top_float_stock_ratio_ = 0;
  top_stock_num_ = 0;
  top_stock_ratio_ = 0;
  institution_stock_num_ = 0;
  institution_stock_ratio_ = 0;
}
StockholderPerQuarter::~StockholderPerQuarter() {

}

StockholderInfo::StockholderInfo() {

  stock_code_ = "";
  identifier_ = 0;
  shares_price_ = 0;
  shares_number_ = 0;
  float_number_ = 0;
  total_number_ = 0;
  ratio_ = 0;
}

StockholderInfo::~StockholderInfo() {
  // YGTODO Auto-generated destructor stub
}

double StockholderPerQuarter::getHolderCount() const {
  return holder_count_;
}

void StockholderPerQuarter::setHolderCount(double holderCount) {
  holder_count_ = holderCount;
}

double StockholderPerQuarter::getCompareChange() const {
  return compare_change_;
}

void StockholderPerQuarter::setCompareChange(double compareChange) {
  compare_change_ = compareChange;
}

const std::string& StockholderPerQuarter::getDate() const {
  return date_;
}

void StockholderPerQuarter::setDate(const std::string& date) {
  date_ = date;
}

double StockholderPerQuarter::getFloatStockNum() const {
  return float_stock_num_;
}

void StockholderPerQuarter::setFloatStockNum(double floatStockNum) {
  float_stock_num_ = floatStockNum;
}

double StockholderPerQuarter::getInstitutionStockNum() const {
  return institution_stock_num_;
}

void StockholderPerQuarter::setInstitutionStockNum(double institutionStockNum) {
  institution_stock_num_ = institutionStockNum;
}

double StockholderPerQuarter::getInstitutionStockRatio() const {
  return institution_stock_ratio_;
}

void StockholderPerQuarter::setInstitutionStockRatio(
    double institutionStockRatio) {
  institution_stock_ratio_ = institutionStockRatio;
}

double StockholderPerQuarter::getTopFloatStockNum() const {
  return top_float_stock_num_;
}

void StockholderPerQuarter::setTopFloatStockNum(double topFloatStockNum) {
  top_float_stock_num_ = topFloatStockNum;
}

double StockholderPerQuarter::getTopFloatStockRatio() const {
  return top_float_stock_ratio_;
}

void StockholderPerQuarter::setTopFloatStockRatio(double topFloatStockRatio) {
  top_float_stock_ratio_ = topFloatStockRatio;
}

double StockholderPerQuarter::getTopStockNum() const {
  return top_stock_num_;
}

void StockholderPerQuarter::setTopStockNum(double topStockNum) {
  top_stock_num_ = topStockNum;
}

double StockholderPerQuarter::getTopStockRatio() const {
  return top_stock_ratio_;
}

void StockholderPerQuarter::setTopStockRatio(double topStockRatio) {
  top_stock_ratio_ = topStockRatio;
}

void StockholderPerQuarter::ValueSerialization(base_logic::DictionaryValue* dict) {
  dict->GetString(L"date", &date_);
  dict->GetReal(L"holder_count", &holder_count_);
  dict->GetReal(L"compare_change", &compare_change_);
  dict->GetReal(L"float_stock_num", &float_stock_num_);
  dict->GetReal(L"top_float_stock_num", &top_float_stock_num_);
  dict->GetReal(L"top_float_stock_ratio", &top_float_stock_ratio_);
  dict->GetReal(L"top_stock_num", &top_stock_num_);
  dict->GetReal(L"top_stock_ratio", &top_stock_ratio_);
  dict->GetReal(L"institution_stock_num", &institution_stock_num_);
  dict->GetReal(L"institution_stock_ratio", &institution_stock_ratio_);
}

void StockholderPerQuarter::printSelf() {
  LOG_MSG2("date=%s,holder_count=%f,compare_change=%f,float_stock_num=%f",
           date_.c_str(),
           holder_count_,
           compare_change_,
           float_stock_num_);
  LOG_MSG2("top_float_stock_num=%f,top_float_stock_ratio=%f,top_stock_num=%f",
           top_float_stock_num_,
           top_float_stock_ratio_,
           top_stock_num_);
  LOG_MSG2("top_stock_ratio=%f,institution_stock_num=%f,institution_stock_ratio=%f",
           top_stock_ratio_,
           institution_stock_num_,
           institution_stock_ratio_);
  LOG_MSG2("address=%p", this);
}

double StockholderInfo::getFloatNumber() const {
  return float_number_;
}

void StockholderInfo::setFloatNumber(double floatNumber) {
  float_number_ = floatNumber;
}

int StockholderInfo::getIdentifier() const {
  return identifier_;
}

void StockholderInfo::setIdentifier(int identifier) {
  identifier_ = identifier;
}

double StockholderInfo::getRatio() const {
  return ratio_;
}

void StockholderInfo::setRatio(double ratio) {
  ratio_ = ratio;
}

double StockholderInfo::getSharesNumber() const {
  return shares_number_;
}

void StockholderInfo::setSharesNumber(double sharesNumber) {
  shares_number_ = sharesNumber;
}

double StockholderInfo::getSharesPrice() const {
  return shares_price_;
}

void StockholderInfo::setSharesPrice(double sharesPrice) {
  shares_price_ = sharesPrice;
}

const std::string& StockholderInfo::getStockCode() const {
  return stock_code_;
}

void StockholderInfo::setStockCode(const std::string& stockCode) {
  stock_code_ = stockCode;
}

const std::map<std::string, StockholderPerQuarter>& StockholderInfo::getStockholderMap() const {
  return stockholder_map_;
}

void StockholderInfo::setStockholderMap(
    const std::map<std::string, StockholderPerQuarter>& stockholderMap) {
  stockholder_map_ = stockholderMap;
}

double StockholderInfo::getTotalNumber() const {
  return total_number_;
}

void StockholderInfo::setTotalNumber(double totalNumber) {
  total_number_ = totalNumber;
}

void StockholderInfo::ValueSerialization(base_logic::DictionaryValue* dict) {
  dict->GetString(L"stock_code", &stock_code_);
  dict->GetInteger(L"identifier", &identifier_);
  dict->GetReal(L"shares_price", &shares_price_);
  dict->GetReal(L"shares_number", &shares_number_);
  dict->GetReal(L"float_number", &float_number_);
  dict->GetReal(L"total_number", &total_number_);
  dict->GetReal(L"ratio", &ratio_);
  if ("000333" == stock_code_) {
    LOG_MSG2("stock_code_=%s,identifier_=%d,shares_price=%f,shares_number=%f,float_number=%f,total_number=%f,ratio=%f",
             stock_code_.c_str(),
             identifier_,
             shares_price_,
             shares_number_,
             float_number_,
             total_number_,
             ratio_);
  }
}

void StockholderInfo::HolderMapValueSerialization(base_logic::DictionaryValue* dict) {
  dict->GetString(L"stock_code", &stock_code_);
  std::string date = "";
  dict->GetString(L"date", &date);
  if ("" != date) {
    StockholderPerQuarter holder_per_quarter;
    holder_per_quarter.ValueSerialization(dict);
    stockholder_map_[date] = holder_per_quarter;
    if ("000333" == stock_code_)
      stockholder_map_[date].printSelf();
  }
}

} /* namespace stock_logic */
