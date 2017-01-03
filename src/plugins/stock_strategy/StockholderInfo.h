/*
 * StockholderInfo.h
 *
 *  Created on: 2016年8月25日
 *      Author: Maibenben
 */

#ifndef GEORGE_MASTER_PLUGINS_STOCK_STOCKHOLDERINFO_H_
#define GEORGE_MASTER_PLUGINS_STOCK_STOCKHOLDERINFO_H_

#include "basic/basictypes.h"
#include "logic/base_values.h"
#include <map>
#include <string>

using namespace std;

namespace stock_logic {

class StockholderPerQuarter {
 public:
  StockholderPerQuarter();
  ~StockholderPerQuarter();

  double getHolderCount() const;
  void setHolderCount(double holderCount);
  double getCompareChange() const;
  void setCompareChange(double compareChange);
  const std::string& getDate() const;
  void setDate(const std::string& date);
  double getFloatStockNum() const;
  void setFloatStockNum(double floatStockNum);
  double getInstitutionStockNum() const;
  void setInstitutionStockNum(double institutionStockNum);
  double getInstitutionStockRatio() const;
  void setInstitutionStockRatio(double institutionStockRatio);
  double getTopFloatStockNum() const;
  void setTopFloatStockNum(double topFloatStockNum);
  double getTopFloatStockRatio() const;
  void setTopFloatStockRatio(double topFloatStockRatio);
  double getTopStockNum() const;
  void setTopStockNum(double topStockNum);
  double getTopStockRatio() const;
  void setTopStockRatio(double topStockRatio);

  void ValueSerialization(base_logic::DictionaryValue* dict);

  void printSelf();

  std::string date_;
  double holder_count_;                        //股东人数（户）
  double compare_change_;                      //股东人数（较上期变化）
  double float_stock_num_;                     //股东人数（人均流通股）
  double top_float_stock_num_;                 //前十大流通股东（持股数量/万股）
  double top_float_stock_ratio_;               //前十大流通股东（占流通股本比例）
  double top_stock_num_;                       //十大股东（持股数量/万股）
  double top_stock_ratio_;                     //十大股东（占总股本比例）
  double institution_stock_num_;               //机构持仓（持股数量/万股）
  double institution_stock_ratio_;             //机构持仓（占流通A股本比例）
};

class StockholderInfo {
 public:
  StockholderInfo();
  virtual ~StockholderInfo();
  double getFloatNumber() const;
  void setFloatNumber(double floatNumber);
  int getIdentifier() const;
  void setIdentifier(int identifier);
  double getRatio() const;
  void setRatio(double ratio);
  double getSharesNumber() const;
  void setSharesNumber(double sharesNumber);
  double getSharesPrice() const;
  void setSharesPrice(double sharesPrice);
  const std::string& getStockCode() const;
  void setStockCode(const std::string& stockCode);
  const std::map<std::string, StockholderPerQuarter>& getStockholderMap() const;
  void setStockholderMap(
      const std::map<std::string, StockholderPerQuarter>& stockholderMap);
  double getTotalNumber() const;
  void setTotalNumber(double totalNumber);

  void ValueSerialization(base_logic::DictionaryValue* dict);

  void HolderMapValueSerialization(base_logic::DictionaryValue* dict);

  std::string stock_code_;                     //股票代码
  int identifier_;                             //1 为 增持，0 为减持
  double shares_price_;                        //（增持或减持）股份金额（万）
  double shares_number_;                       //（增持或减持）股份数量（股）
  double float_number_;                        //流通股本（万股）
  double total_number_;                        //总股本（万股）
  double ratio_;                               //占流通股本比例（%）
  std::map<std::string, StockholderPerQuarter> stockholder_map_;
};

} /* namespace stock_logic */

#endif /* GEORGE_MASTER_PLUGINS_STOCK_STOCKHOLDERINFO_H_ */
