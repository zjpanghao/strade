//
// Created by Harvey on 2017/1/19.
//

#ifndef STRADE_STRADE_SHARE_TIMER_H
#define STRADE_STRADE_SHARE_TIMER_H

#include "strade_share/strade_share_engine.h"
#include "storage/mysql_engine.h"

namespace strade_share_logic {

class StradeShareTimer {
 private:
  StradeShareTimer();

 public:
  static StradeShareTimer* GetInstance();
  bool InitParam();

  bool OnTimeLoadStockVisit();

 private:
  static void OnTimeLoadStockVisitCallback(std::vector<MYSQL_ROW>& rows_vec);

 private:
  base_logic::MysqlEngine* mysql_engine_;
  static strade_share::SSEngine* ss_engine_;
};


} /* namespace strade_share_logic */

#endif //STRADE_STRADE_SHARE_TIMER_H
