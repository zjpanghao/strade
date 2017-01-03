/*
 * CustomWeightObserver.cpp
 *
 *  Created on: 2016年8月10日
 *      Author: Maibenben
 */

#include "CustomWeightObserver.h"
#include "stock_factory.h"
#include "basic/basic_util.h"

namespace stock_logic {

CustomWeightObserver::CustomWeightObserver(stock_logic::Subject* subject) {
  set_name("CustomWeightObserver");
  subject_ = subject;

}

CustomWeightObserver::~CustomWeightObserver() {
  // YGTODO Auto-generated destructor stub
}

void CustomWeightObserver::Update(int opcode, stock_logic::Subject* subject) {

}

base_logic::DictionaryValue* CustomWeightObserver::Request(base_logic::DictionaryValue* params) {
  std::string start_date = ",2016-08-01";
  std::string end_date = ",2046-08-10";
  bool r = false;
  r = params->GetString(L"start_date", &start_date);
  if (true == r) {
    std::string decode_start_date = "";
    base::BasicUtil::UrlDecode(start_date, decode_start_date);
    start_date = decode_start_date;
    LOG_MSG2("get start_date=%s", start_date.c_str());
  }
  if (start_date.size() > 1)
    start_date.erase(start_date.begin());

  r = params->GetString(L"end_date", &end_date);
  if (true == r) {
    std::string decode_end_date = "";
    base::BasicUtil::UrlDecode(end_date, decode_end_date);
    end_date = decode_end_date;
    LOG_MSG2("get end_date=%s", end_date.c_str());
  }
  if (end_date.size() > 1)
    end_date.erase(end_date.begin());

  std::map<std::string, double> stocks_info_map = this->GetStocksInfo(params);
  return this->GetCustomStocksField(stocks_info_map,
                                    start_date,
                                    end_date);

}

base_logic::DictionaryValue* CustomWeightObserver::GetCustomStocksField(
    std::map<std::string, double>& stocks_info_map,
    std::string& start_date,
    std::string& end_date) {
  stock_logic::StockFactory* factory = (stock_logic::StockFactory*)subject_;
  std::map<std::string, double>::iterator stocks_info_iter =
      stocks_info_map.begin();
  for (; stocks_info_iter != stocks_info_map.end(); stocks_info_iter++) {
    LOG_MSG2("stock_code=%s,weight=%f",
             stocks_info_iter->first.c_str(),
             stocks_info_iter->second);
  }
  BasicIndustryInfo industry_info;
  industry_info.add_stocks_weight(stocks_info_map);
  industry_info.set_stocks_price_info(stocks_info_map);
  industry_info.update_hist_data_by_weight();
  industry_info.count_stock_visit_num();

  std::map<std::string, HistDataPerDay>& industry_hist_data =
      industry_info.industry_hist_data_info_.stock_hist_data_;
  std::map<std::string, HistDataPerDay>& hs300_hist_data = factory->GetHistDataByCode(HSSANBAI);

  return StockUtil::Instance()->get_backtest_of_hist_data(
      hs300_hist_data,
      start_date,
      end_date,
      industry_hist_data);
}

std::map<std::string, double> CustomWeightObserver::GetStocksInfo(base_logic::DictionaryValue* params) {
  StockFactory* factory = (StockFactory*)subject_;
  bool r = false;
  std::string stocks_info = "";
  r = params->GetString(L"stocks_info", &stocks_info);
  if (true == r) {
    std::string encode_stocks_info = "";
    base::BasicUtil::UrlDecode(stocks_info, encode_stocks_info);
    stocks_info = encode_stocks_info;
    LOG_MSG2("get stocks_info=%s", stocks_info.c_str());
  }

  std::map<std::string, double> stocks_info_map;
  std::vector<std::string> stocks_info_vec;
  std::string split_chars = ",";
  StockUtil::Instance()->stock_split(stocks_info, split_chars, stocks_info_vec);
  for (int i = 0; i < stocks_info_vec.size() - 1; i++) {
    double weight = atof(stocks_info_vec[i+1].c_str());
    if (weight < 0.01)
      continue;
    std::string& stock_code = stocks_info_vec[i];
    if (factory->CheckStockValid(stock_code))
      stocks_info_map[stock_code] = weight;
  }

  return stocks_info_map;
}

} /* namespace stock_logic */
