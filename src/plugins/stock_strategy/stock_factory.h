//  Copyright (c) 2016-2016 The george Authors. All rights reserved.
//  Created on: 2016年5月21日 Author: kerry
#ifndef GEORGE_VIP_FACTORY_H__
#define GEORGE_VIP_FACTORY_H__

#include "basic/basictypes.h"
#include "basic/scoped_ptr.h"
#include "config/config.h"
#include "net/packet_process.h"
#include "stock_basic_info.h"
#include "stock_db.h"
#include "stock_proto_buf.h"
#include "stock_user_mgr.h"
#include "Subject.h"
#include "Observer.h"

namespace stock_logic {

class StockDB;

class StockFactory: public Subject {
 public:
  StockFactory();
  virtual ~StockFactory();
 public:
  void Init();
  void Dest();
 private:
  static StockFactory* instance_;
 public:
  static StockFactory* GetInstance();

  static void FreeInstance();

  void InitParam(config::FileConfig* config);
 public:

  void OnVIPGetLimitData(const int socket, base_logic::DictionaryValue* dict,
                         george_logic::PacketHead* packet);

  void OnVIPGetHotDiagramData(const int socket,
                              base_logic::DictionaryValue* dict,
                              george_logic::PacketHead* packet);

  void OnUpdateRealtimeStockInfo();

  void OnUpdateStockHistData();

  void OnUpdateStockKLineData();

  void OnUpdateEventsData();

  void OnUpdateYieldDataFromDB();

  void OnUpdateYieldDataToDB();

  void OnDeleteOldYieldData();

  void OnLoadCustom_Event();

  void OnUpdateLimitData();

  void ProcessHotDiagramEventData(int socket, george_logic::PacketHead* packet,
                                  base_logic::DictionaryValue* dict);

  void ProcessHotDiagramIndustryData(int socket, std::string type,
                                     std::string format,
                                     george_logic::PacketHead* packet,
                                     base_logic::DictionaryValue* dict);

  void ProcessHotDiagramByIndustry(int socket, std::string type,
                                   std::string industry_name);

  void ProcessStockKLine(int socket, std::string stock_code, std::string format,
                         george_logic::PacketHead* packet,
                         base_logic::DictionaryValue* dict,
                         std::string& cycle_type,
                         std::string& start_date,
                         std::string& end_date,
                         std::string& name);

  void ProcessEventYieldByName(int socket,
                               std::string& start_date,
                               std::string& end_date,
                               std::string& industry_name,
                               george_logic::PacketHead* packet,
                               std::string& cycle_type,
                               std::string& name);

  void TimeWriteLimitData(int current_trade_time);

  void TimeDeleteOldLimitData();

  void TimeUpdateWeekMonthData();

  void OnUpdateOfflineVisitData();

  bool CheckStockValid(std::string& stock_code);

  StockUserCache* GetCache();

  STOCKINFO_MAP& GetStockInfoMap();

  StockTotalInfo& GetTotalInfoByCode(std::string& stock_code);

  StockBasicInfo& GetBasicInfoByCode(std::string& stock_code);

  std::map<std::string, HistDataPerDay>& GetHistDataByCode(std::string stock_code);

  bool GetHistDataByDate(std::string stock_code, std::string date, HistDataPerDay& hist_data);

  std::map<std::string, DataPerDay>& GetDataPerDayByCode(std::string stock_code);

  std::map<std::string, BasicIndustryInfo>& GetIndustryMap();

  BasicIndustryInfo& getBasicIndustryInfoByName(std::string& industry_name);

  StockUserManager* stock_usr_mgr_;
  stock_logic::StockDB* stock_db_;
};
}
#endif
