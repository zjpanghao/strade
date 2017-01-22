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
  static void OnTimeLoadStockVisitCallback(
      int column_num, MYSQL_ROWS_VEC& rows_vec);

 private:
  static strade_share::SSEngine* ss_engine_;
};


} /* namespace strade_share_logic */

#endif //STRADE_STRADE_SHARE_TIMER_H
